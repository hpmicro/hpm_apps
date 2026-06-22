/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "hpm_soc.h"
#include "hpm_clock_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_common.h"
#include "hpm_gptmr_drv.h"
#include "common.h"
#include "netconf.h"
#include "sys_arch.h"
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "monitor_log.h"
#include "monitor_task.h"
#include "monitor_profile.h"
#include "monitor.h"

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT]; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT]; /* Ethernet Tx DMA Descriptor */

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;
uint8_t mac[ENET_MAC_SIZE];

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag;
#endif

struct netif gnetif;

static struct tcp_pcb *send_newpcb = NULL;
static bool monitor_enet_send_done;

/** Bytes copied into lwIP per monitor_handle iteration (keeps RX/command path responsive). */
#define ENET_TCP_PUMP_BUDGET 4096U

typedef struct {
    const uint8_t *data;
    uint32_t len;
    uint32_t offset;
    bool active;
} enet_tcp_send_ctx_t;

static enet_tcp_send_ctx_t s_tcp_send = {0};

typedef void (*monitor_enet_timer_cb)(void);
static monitor_enet_timer_cb enet_timer_cb;

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t enet_init(ENET_Type *ptr)
{
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;
    enet_tx_control_config_t enet_tx_control_config;

#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_config_t phy_config;
#else
    rtl8211_config_t phy_config;
#endif
#else
#if defined(__USE_DP83848) && __USE_DP83848
    dp83848_config_t phy_config;
#else
    rtl8201_config_t phy_config;
#endif
#endif

    /* Initialize td, rd and the corresponding buffers */
    memset((uint8_t *)dma_tx_desc_tab, 0x00, sizeof(dma_tx_desc_tab));
    memset((uint8_t *)dma_rx_desc_tab, 0x00, sizeof(dma_rx_desc_tab));
    memset((uint8_t *)rx_buff, 0x00, sizeof(rx_buff));
    memset((uint8_t *)tx_buff, 0x00, sizeof(tx_buff));

    desc.tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_tx_desc_tab);
    desc.rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_rx_desc_tab);

    desc.tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tx_buff);
    desc.tx_buff_cfg.count = ENET_TX_BUFF_COUNT;
    desc.tx_buff_cfg.size = ENET_TX_BUFF_SIZE;

    desc.rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rx_buff);
    desc.rx_buff_cfg.count = ENET_RX_BUFF_COUNT;
    desc.rx_buff_cfg.size = ENET_RX_BUFF_SIZE;

    /*Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(ENET, &enet_tx_control_config);

    /* Set the control config for tx descriptor */
    memcpy(&desc.tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get MAC address */
    enet_get_mac_address(mac);

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[5] << 8 | mac[4];
    enet_config.mac_addr_low[0] = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    enet_config.valid_max_count = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ENET);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Enable Enet IRQ */
    board_enable_enet_irq(ENET);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);
#endif

    /* Initialize enet controller */
    if (enet_controller_init(ptr, ENET_INF_TYPE, &desc, &enet_config, &int_config) != status_success)
    {
        return status_fail;
    }

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ENET);
#endif

/* Initialize phy */
#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_reset(ptr, DP83867_ADDR);
#if defined(__DISABLE_AUTO_NEGO) && __DISABLE_AUTO_NEGO
    dp83867_set_mdi_crossover_mode(ENET, DP83867_ADDR, enet_phy_mdi_crossover_manual_mdix);
#endif
    dp83867_basic_mode_default_config(ptr, &phy_config);
    if (dp83867_basic_mode_init(ptr, DP83867_ADDR, &phy_config) == true)
    {
#else
    rtl8211_reset(ptr, RTL8211_ADDR);
    rtl8211_basic_mode_default_config(ptr, &phy_config);
    if (rtl8211_basic_mode_init(ptr, RTL8211_ADDR, &phy_config) == true)
    {
#endif
#else
#if defined(__USE_DP83848) && __USE_DP83848
    dp83848_reset(ptr, DP83848_ADDR);
    dp83848_basic_mode_default_config(ptr, &phy_config);
    if (dp83848_basic_mode_init(ptr, DP83848_ADDR, &phy_config) == true)
    {
#else
    rtl8201_reset(ptr, RTL8201_ADDR);
    rtl8201_basic_mode_default_config(ptr, &phy_config);
    if (rtl8201_basic_mode_init(ptr, RTL8201_ADDR, &phy_config) == true)
    {
#endif
#endif
        printf("Enet phy init passed !\n");
        return status_success;
    }
    else
    {
        printf("Enet phy init failed !\n");
        return status_fail;
    }
}

static err_t tcpecho_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    uint32_t drop, expect;
    (void)arg;

    if (p != NULL)
    {
        tcp_recved(tpcb, p->tot_len);
        monitor_task_input_process(p->payload, p->tot_len, &drop, &expect);
        pbuf_free(p);
    }
    else if (err == ERR_OK)
    {
        send_newpcb = NULL;
        s_tcp_send.active = false;
        s_tcp_send.data = NULL;
        monitor_enet_send_done = true;
        return tcp_close(tpcb);
    }
    return ERR_OK;
}

static err_t tcpecho_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    (void)arg;
    (void)err;

    tcp_recv(newpcb, tcpecho_recv);
    send_newpcb = newpcb;
    s_tcp_send.active = false;
    s_tcp_send.data = NULL;
    monitor_enet_send_done = true;
    return ERR_OK;
}

static void enet_tcp_send_fail(void)
{
    if (send_newpcb != NULL)
    {
        tcp_close(send_newpcb);
        send_newpcb = NULL;
    }
    s_tcp_send.active = false;
    s_tcp_send.data = NULL;
    s_tcp_send.len = 0;
    s_tcp_send.offset = 0;
    monitor_enet_send_done = true;
}

static err_t enet_tcp_try_write_chunk(void)
{
    uint32_t budget = ENET_TCP_PUMP_BUDGET;

    if (!s_tcp_send.active || send_newpcb == NULL)
    {
        return ERR_OK;
    }

    while (s_tcp_send.offset < s_tcp_send.len && budget > 0U)
    {
        uint32_t space = tcp_sndbuf(send_newpcb);
        if (space == 0U)
        {
            tcp_output(send_newpcb);
            return ERR_MEM;
        }

        uint32_t remain = s_tcp_send.len - s_tcp_send.offset;
        uint16_t chunk = remain > space ? (uint16_t)space : (uint16_t)remain;
        if (chunk > budget)
        {
            chunk = (uint16_t)budget;
        }

        /* COPY: stream DMA buffers are reused immediately after send; zero-copy would corrupt frames. */
        err_t ret = tcp_write(send_newpcb, s_tcp_send.data + s_tcp_send.offset, chunk, TCP_WRITE_FLAG_COPY);
        if (ret == ERR_OK)
        {
            s_tcp_send.offset += chunk;
            budget -= chunk;
            continue;
        }
        if (ret == ERR_MEM)
        {
            tcp_output(send_newpcb);
            return ERR_MEM;
        }

        return ret;
    }

    if (s_tcp_send.offset >= s_tcp_send.len)
    {
        tcp_output(send_newpcb);
        s_tcp_send.active = false;
        s_tcp_send.data = NULL;
        monitor_enet_send_done = true;
    }

    return ERR_OK;
}

static void enet_tcp_send_pump(void)
{
    err_t ret;

    if (!s_tcp_send.active)
    {
        return;
    }

    ret = enet_tcp_try_write_chunk();
    if (ret != ERR_OK && ret != ERR_MEM)
    {
        enet_tcp_send_fail();
    }
}

static int enet_tcp_block_output(uint8_t *buff, uint32_t len)
{
    err_t ret;

    if (send_newpcb == NULL || buff == NULL || len == 0U)
    {
        return -1;
    }

    if (s_tcp_send.active)
    {
        return -1;
    }

    s_tcp_send.data = buff;
    s_tcp_send.len = len;
    s_tcp_send.offset = 0U;
    s_tcp_send.active = true;
    monitor_enet_send_done = false;

    ret = enet_tcp_try_write_chunk();
    if (ret != ERR_OK && ret != ERR_MEM)
    {
        enet_tcp_send_fail();
        return -1;
    }

    return 0;
}

void tcp_echo_init(void)
{
    struct tcp_pcb *pcb = NULL;

    pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, MONITOR_TCP_LOCAL_PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, tcpecho_accept);
}

void monitor_enet_timer_isr(void)
{
    if (gptmr_check_status(MONITOR_ENET_TIMER, GPTMR_CH_RLD_STAT_MASK(MONITOR_ENET_TIMER_CH))) {
        gptmr_clear_status(MONITOR_ENET_TIMER, GPTMR_CH_RLD_STAT_MASK(MONITOR_ENET_TIMER_CH));
        enet_timer_cb();
    }
}
SDK_DECLARE_EXT_ISR_M(MONITOR_ENET_TIMER_IRQ, monitor_enet_timer_isr)


static void monitor_enet_timer_create(uint32_t ms, monitor_enet_timer_cb cb)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    enet_timer_cb = cb;
    gptmr_channel_get_default_config(MONITOR_ENET_TIMER, &config);

    clock_add_to_group(MONITOR_ENET_TIMER_CLK_NAME, 0);
    gptmr_freq = clock_get_frequency(MONITOR_ENET_TIMER_CLK_NAME);

    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(MONITOR_ENET_TIMER, MONITOR_ENET_TIMER_CH, &config, false);
    gptmr_enable_irq(MONITOR_ENET_TIMER, GPTMR_CH_RLD_IRQ_MASK(MONITOR_ENET_TIMER_CH));
    intc_m_enable_irq_with_priority(MONITOR_ENET_TIMER_IRQ, 1);

    gptmr_start_counter(MONITOR_ENET_TIMER, MONITOR_ENET_TIMER_CH);
}


/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int monitor_enet_init(void)
{
    monitor_enet_send_done = true;
    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    /* Reset an enet PHY */
    board_reset_enet_phy(ENET);

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: TCP Echo (Interrupt Usage)\n");
#else
    printf("This is an ethernet demo: TCP Echo (Polling Usage)\n");
#endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

/* Set RGMII clock delay */
#if defined(RGMII) && RGMII
    board_init_enet_rgmii_clock_delay(ENET);
#elif defined(RMII) && RMII
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ENET, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
#endif

    /* Start a board timer */
    monitor_enet_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    /* Initialize MAC and DMA */
    if (enet_init(ENET) == 0)
    {
        /* Initialize the Lwip stack */
        lwip_init();
        netif_config(&gnetif);

        /* Start services */
        enet_services(&gnetif);
        /* Initialize TCP echo */
        tcp_echo_init();
    }
    else
    {
        printf("Enet initialization fails !!!\n");
        while (1)
        {
        }
    }

    return 0;
}

int monitor_init(void)
{
    monitor_enet_init();

    monitor_task_init();

    monitor_task_output_register(enet_tcp_block_output);
    return 0;
}

void monitor_handle(void)
{
    enet_common_handler(&gnetif);
    enet_tcp_send_pump();
    monitor_task_handle();
}

bool monitor_send_is_idle(void)
{
    return (!s_tcp_send.active) && monitor_enet_send_done;
}