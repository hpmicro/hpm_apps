/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "common.h"
#include "netconf.h"
#include "sys_arch.h"
#include "lwip.h"
#include "lwip/init.h"
#include "udp_echo.h"
#include "hpm_tsw_drv.h"
#include "hpm_jl1111.h"
#include "hpm_gptmr_drv.h"

struct netif gnetif;
uint8_t mac[TSW_ENET_MAC];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t send_buff[TSW_SEND_DESC_COUNT][TSW_SEND_BUFF_LEN];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(TSW_SOC_DATA_BUS_WIDTH) uint8_t recv_buff[TSW_RECV_DESC_COUNT][TSW_RECV_BUFF_LEN];

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
volatile bool rx_flag;
tsw_frame_t frame[TSW_FRAME_BUFF_COUNT];
#endif

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t tsw_init(TSW_Type *ptr)
{
    jl1111_config_t phy_config;
    tsw_dma_config_t config;

    /* Disable all MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Get Mac address */
    tsw_get_mac_address(mac);
    mac[5] = MAC5;

    /* Set Mac Address */
    tsw_ep_set_mac_addr(ptr, TSW_TSNPORT_PORT1, mac, true);
    tsw_ep_set_mac_addr(ptr, TSW_TSNPORT_PORT2, mac, true);

    /* Set MAC Mode: MII, CLKSEL: refclk */
    tsw_ep_set_mac_mode(ptr, TSW_TSNPORT_PORT1, tsw_mac_mode_mii);
    tsw_ep_set_mac_mode(ptr, TSW_TSNPORT_PORT2, tsw_mac_mode_mii);

    /* Set port PHY interface */
    tsw_set_port_interface(ptr, TSW_TSNPORT_PORT1, tsw_port_phy_itf_mii);
    tsw_set_port_interface(ptr, TSW_TSNPORT_PORT2, tsw_port_phy_itf_mii);

    tsw_set_port_speed(ptr, TSW_TSNPORT_PORT1, tsw_port_speed_100mbps);
    tsw_set_port_speed(ptr, TSW_TSNPORT_PORT2, tsw_port_speed_100mbps);

    /* Enable all MACs(TX/RX) */
    tsw_ep_enable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Clear CAM */
    tsw_clear_cam(ptr);

    /* Wait for cam clearing completion */
    board_delay_ms(10);

    /* Enable VLAN-ID 1 at all ports */
    tsw_set_cam_vlan_port(ptr);

    /* Set system cycle numbers */
    // tsw_cb_frer_set_msec_cycles(ptr, 0);

    /* Get the default DMA config */
    tsw_get_default_dma_config(&config);

    /* Initialize DMA for sending */
    tsw_init_send(ptr, &config);

    /* Initialize DMA for receiving */
#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
    config.irq = true;
#endif
    tsw_init_recv(ptr, &config);

    for (uint8_t i = 0; i < TSW_RECV_DESC_COUNT; i++) {
        tsw_commit_recv_desc(ptr, recv_buff[i], TSW_RECV_BUFF_LEN, i);
    }

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
    intc_m_enable_irq(IRQn_TSW_0);  /* Enable TSW CPU Port IRQ */
#endif

    /* Set broadcast frame and unknown frame actions */
    tsw_set_broadcast_frame_action(ptr, tsw_dst_port_cpu);
    tsw_set_unknown_frame_action(ptr, tsw_dst_port_1 | tsw_dst_port_2);
    tsw_set_internal_frame_action(ptr, tsw_dst_port_null);

    uint64_t cpu_mac = ((uint64_t)mac[5])<<0 | ((uint64_t)mac[4])<<8 | ((uint64_t)mac[3])<<16 | ((uint64_t)mac[2])<<24 | ((uint64_t)mac[1])<<32 | ((uint64_t)mac[0])<<40;

    tsw_set_lookup_table(ptr, 0, tsw_dst_port_cpu, cpu_mac);

    /* Set MDC clock frequency to 2.5MHz */
    tsw_ep_set_mdio_config(ptr, TSW_TSNPORT_PORT1, 19);
    tsw_ep_set_mdio_config(ptr, TSW_TSNPORT_PORT2, 19);
#if 1
    /* Set RTC increment */
    tsw_set_rtc_time_increment(ptr, (10 << 24));

    tsw_cb_stmid_entry_t cb_stmid_entry_ingress = {0};
    cb_stmid_entry_ingress.idx = 0;
    cb_stmid_entry_ingress.sid = 0;
    cb_stmid_entry_ingress.seqnum = 0;
    cb_stmid_entry_ingress.seqgen = true;
    cb_stmid_entry_ingress.actctl = tsw_stmid_actctl_disabled;
    cb_stmid_entry_ingress.smac = tsw_stmid_control_lookup_by_src_mac;
    cb_stmid_entry_ingress.mode = tsw_stmid_lookup_mode_all;
    cb_stmid_entry_ingress.enable = true;
    cb_stmid_entry_ingress.lookup_mac.vid = 1;
    cb_stmid_entry_ingress.lookup_mac.mach = MAC_HI(mac);
    cb_stmid_entry_ingress.lookup_mac.macl = MAC_LO(mac);
    tsw_cb_stmid_ingress_set_entry(ptr, &cb_stmid_entry_ingress);
    tsw_cb_frer_ingress_enable_rtag(ptr);

    tsw_cb_stmid_entry_t cb_stmid_entry_egress = {0};
    cb_stmid_entry_egress.idx = 0;
    cb_stmid_entry_egress.sid = 0;
    cb_stmid_entry_egress.seqnum = 0;
    cb_stmid_entry_egress.seqgen = false;
    cb_stmid_entry_egress.actctl = tsw_stmid_actctl_disabled;
    cb_stmid_entry_egress.smac = tsw_stmid_control_lookup_by_dest_mac;
    cb_stmid_entry_egress.mode = tsw_stmid_lookup_mode_all;
    cb_stmid_entry_egress.enable = true;
    cb_stmid_entry_egress.lookup_mac.vid = 1;
    cb_stmid_entry_egress.lookup_mac.mach = MAC_HI(mac);
    cb_stmid_entry_egress.lookup_mac.macl = MAC_LO(mac);
    tsw_cb_stmid_egress_set_entry(ptr, &cb_stmid_entry_egress);

    cb_stmid_entry_egress.idx = 1;
    cb_stmid_entry_egress.sid = 1;
    cb_stmid_entry_egress.seqnum = 0;
    cb_stmid_entry_egress.seqgen = false;
    cb_stmid_entry_egress.actctl = tsw_stmid_actctl_disabled;
    cb_stmid_entry_egress.smac = tsw_stmid_control_lookup_by_dest_mac;
    cb_stmid_entry_egress.mode = tsw_stmid_lookup_mode_all;
    cb_stmid_entry_egress.enable = true;
    cb_stmid_entry_egress.lookup_mac.vid = 1;
    cb_stmid_entry_egress.lookup_mac.mach = 0x0000ffff;
    cb_stmid_entry_egress.lookup_mac.macl = 0xffffffff;
    tsw_cb_stmid_egress_set_entry(ptr, &cb_stmid_entry_egress);

    /* set func 1 */
    tsw_cb_frer_recovery_func_config_t xrfunc_config;
    xrfunc_config.fidx = 1;
    xrfunc_config.freset = true;
    xrfunc_config.xrfunc = tsw_cb_frer_xfunc_recovery_individual;
    xrfunc_config.algo = tsw_cb_frer_algo_match_recovery;
    xrfunc_config.timeout_in_ms = 1;
    tsw_cb_frer_egress_set_recovery_func(ptr, &xrfunc_config);

    xrfunc_config.fidx = 2;
    tsw_cb_frer_egress_set_recovery_func(ptr, &xrfunc_config);
    
    /* set func 3 */
    xrfunc_config.fidx = 3;
    xrfunc_config.freset = true;
    xrfunc_config.xrfunc = tsw_cb_frer_xfunc_recovery_sequence;
    xrfunc_config.algo = tsw_cb_frer_algo_vector_recovery;
    xrfunc_config.history_len = 2;
    xrfunc_config.timeout_in_ms = 1;

    xrfunc_config.latent_error_dectection_config.enable_detection = true;
    xrfunc_config.latent_error_dectection_config.reset_period = 500;
    xrfunc_config.latent_error_dectection_config.test_period = 100;
    xrfunc_config.latent_error_dectection_config.threshold = 5;
    tsw_cb_frer_egress_set_recovery_func(ptr, &xrfunc_config);

    /* set stream (SID 1)  */
    tsw_cb_frer_sid_func_config_t sid_func_config;
    sid_func_config.sid = 0;
    sid_func_config.irfunc.fen = true;
    sid_func_config.irfunc.fidx = 1;
    sid_func_config.srfunc.fen = true;
    sid_func_config.srfunc.fidx = 3;
    tsw_cb_frer_egress_set_sid_func(ptr, &sid_func_config);

    sid_func_config.sid = 1;
    sid_func_config.irfunc.fen = true;
    sid_func_config.irfunc.fidx = 2;
    sid_func_config.srfunc.fen = true;
    sid_func_config.srfunc.fidx = 3;
    tsw_cb_frer_egress_set_sid_func(ptr, &sid_func_config);
#endif
    jl1111_phy_disable_broadcast_response();
    jl1111_phy_config_led_mode();

    return status_success;
}


void app_lwip_timer_isr(void)
{
    if (gptmr_check_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(1))) {
        gptmr_clear_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(1));

        if(gnetif.flags & NETIF_FLAG_LINK_UP)
            udp_echo_send_test();

    }
}
SDK_DECLARE_EXT_ISR_M(IRQn_GPTMR1, app_lwip_timer_isr);

void app_lwip_timer_create(uint32_t ms)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(HPM_GPTMR1, &config);

    clock_add_to_group(clock_gptmr1, 0);
    gptmr_freq = clock_get_frequency(clock_gptmr1);

    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(HPM_GPTMR1, 1, &config, false);
    gptmr_enable_irq(HPM_GPTMR1, GPTMR_CH_RLD_IRQ_MASK(1));
    intc_m_enable_irq_with_priority(IRQn_GPTMR1, 1);
}
void app_lwip_timer_start(void)
{
    gptmr_start_counter(HPM_GPTMR1, 1);
}

void app_lwip_timer_stop(void)
{
    gptmr_stop_counter(HPM_GPTMR1, 1);
}
/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_tsw_pins(BOARD_TSW);

    //use enet0 mdio set phy
    HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PA30_FUNC_CTL_ETH0_MDIO;
    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PA31_FUNC_CTL_ETH0_MDC;
    clock_add_to_group(clock_eth0, 0);

    /* Reset an TSW PHY */
    board_reset_tsw_phy(BOARD_TSW, TSW_TSNPORT_PORT1);
    board_reset_tsw_phy(BOARD_TSW, TSW_TSNPORT_PORT2);
    board_ungate_mchtmr_at_lp_mode();
    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);


    /* Initialize MAC and DMA */
    if (tsw_init(BOARD_TSW) == 0) {
        /* Initialize the Lwip stack */
        lwip_init();
        netif_config(&gnetif);
        /* Start services */
        tsw_services(&gnetif);

        /* Initialize TCP echo */
        udp_echo_init();

        app_lwip_timer_create(1);

        while (1) {
            tsw_common_handler(&gnetif);
        }
    } else {
        printf("TSW initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
