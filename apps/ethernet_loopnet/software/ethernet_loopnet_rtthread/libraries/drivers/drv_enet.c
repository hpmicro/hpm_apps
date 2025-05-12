/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Change Logs:
 * Date         Author      Notes
 * 2022-01-11   HPMicro     First version
 * 2022-07-10   HPMicro     Driver optimization for multiple instances
 */

#include <rtdevice.h>

#ifdef BSP_USING_ETH
#include <rtdbg.h>
#include "drv_enet.h"
#include "hpm_otp_drv.h"
#include "hpm_rtt_interrupt_util.h"
#include "ethernet.h"

#ifdef BSP_USING_ETH0

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t enet0_dma_rx_desc_tab[ENET0_RX_BUFF_COUNT]; /* Ethernet0 Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t enet0_dma_tx_desc_tab[ENET0_TX_BUFF_COUNT]; /* Ethernet0 Tx DMA Descriptor */

ATTR_PLACE_AT_FAST_RAM_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t enet0_rx_buff[ENET0_RX_BUFF_COUNT][ENET0_RX_BUFF_SIZE]; /* Ethernet0 Receive Buffer */

ATTR_PLACE_AT_FAST_RAM_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t enet0_tx_buff[ENET0_TX_BUFF_COUNT][ENET0_TX_BUFF_SIZE]; /* Ethernet0 Transmit Buffer */

struct eth_device eth0_dev;
static enet_device enet0_dev;
static enet_buff_config_t enet0_rx_buff_cfg = {.buffer = (uint32_t)enet0_rx_buff,
                                               .count  = ENET0_RX_BUFF_COUNT,
                                               .size   = ENET0_RX_BUFF_SIZE
                                              };

static enet_buff_config_t enet0_tx_buff_cfg = {.buffer = (uint32_t)enet0_tx_buff,
                                               .count  = ENET0_TX_BUFF_COUNT,
                                               .size   = ENET0_TX_BUFF_SIZE
                                              };

#if __USE_ENET_PTP
static enet_ptp_ts_update_t ptp_timestamp0 = {0, 0};
static enet_ptp_config_t ptp_config0 = {.timestamp_rollover_mode = enet_ts_dig_rollover_control,
                                        .update_method = enet_ptp_time_fine_update,
                                        .addend = 0xffffffff,
                                       };
#endif

static hpm_enet_t enet0 = {.name            = "E0",
                           .base            = HPM_ENET0,
                           .irq_num         = IRQn_ENET0,
                           .inf             = BOARD_ENET0_INF,
                           .eth_dev         = &eth0_dev,
                           .enet_dev        = &enet0_dev,
                           .rx_buff_cfg     = &enet0_rx_buff_cfg,
                           .tx_buff_cfg     = &enet0_tx_buff_cfg,
                           .dma_rx_desc_tab = enet0_dma_rx_desc_tab,
                           .dma_tx_desc_tab = enet0_dma_tx_desc_tab,
#if !BOARD_ENET0_INF
                           .int_refclk      = BOARD_ENET0_INT_REF_CLK,
#else
                           .tx_delay        = BOARD_ENET0_TX_DLY,
                           .rx_delay        = BOARD_ENET0_RX_DLY,
#endif

#if __USE_ENET_PTP
                           .ptp_clk_src     = BOARD_ENET0_PTP_CLOCK,
                           .ptp_config      = &ptp_config0,
                           .ptp_timestamp   = &ptp_timestamp0
#endif
                          };
#endif

mac_init_t mac_init[] = {
    {MAC0_ADDR0, MAC0_ADDR1, MAC0_ADDR2, MAC0_ADDR3, MAC0_ADDR4, MAC0_ADDR5},
    {MAC1_ADDR0, MAC1_ADDR1, MAC1_ADDR2, MAC1_ADDR3, MAC1_ADDR4, MAC1_ADDR5}
};

static uint8_t current_mac[2][6] = {0};
#ifdef BSP_USING_ETH1

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t enet1_dma_rx_desc_tab[ENET1_RX_BUFF_COUNT]; /* Ethernet1 Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t enet1_dma_tx_desc_tab[ENET1_TX_BUFF_COUNT]; /* Ethernet1 Tx DMA Descriptor */

ATTR_PLACE_AT_FAST_RAM_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t enet1_rx_buff[ENET1_RX_BUFF_COUNT][ENET1_RX_BUFF_SIZE]; /* Ethernet1 Receive Buffer */

ATTR_PLACE_AT_FAST_RAM_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t enet1_tx_buff[ENET1_TX_BUFF_COUNT][ENET1_TX_BUFF_SIZE]; /* Ethernet1 Transmit Buffer */

struct eth_device eth1_dev;
static enet_device enet1_dev;
static enet_buff_config_t enet1_rx_buff_cfg = {.buffer = (uint32_t)enet1_rx_buff,
                                               .count  = ENET1_RX_BUFF_COUNT,
                                               .size   = ENET1_RX_BUFF_SIZE
                                              };

static enet_buff_config_t enet1_tx_buff_cfg = {.buffer = (uint32_t)enet1_tx_buff,
                                               .count  = ENET1_TX_BUFF_COUNT,
                                               .size   = ENET1_TX_BUFF_SIZE
                                              };

#if __USE_ENET_PTP
static enet_ptp_ts_update_t ptp_timestamp1 = {0, 0};
static enet_ptp_config_t ptp_config1 = {.timestamp_rollover_mode = enet_ts_dig_rollover_control,
                                        .update_method = enet_ptp_time_fine_update,
                                        .addend = 0xffffffff,
                                       };
#endif

static hpm_enet_t enet1 = {.name            = "E1",
                           .base            = HPM_ENET1,
                           .irq_num         = IRQn_ENET1,
                           .inf             = BOARD_ENET1_INF,
                           .eth_dev         = &eth1_dev,
                           .enet_dev        = &enet1_dev,
                           .rx_buff_cfg     = &enet1_rx_buff_cfg,
                           .tx_buff_cfg     = &enet1_tx_buff_cfg,
                           .dma_rx_desc_tab = enet1_dma_rx_desc_tab,
                           .dma_tx_desc_tab = enet1_dma_tx_desc_tab,
#if !BOARD_ENET1_INF
                           .int_refclk      = BOARD_ENET1_INT_REF_CLK,
#else
                           .tx_delay        = BOARD_ENET1_TX_DLY,
                           .rx_delay        = BOARD_ENET1_RX_DLY,
#endif

#if __USE_ENET_PTP
                           .ptp_clk_src     = BOARD_ENET1_PTP_CLOCK,
                           .ptp_config      = &ptp_config1,
                           .ptp_timestamp   = &ptp_timestamp1
#endif
                          };
#endif

static hpm_enet_t *s_geths[] = {
#ifdef BSP_USING_ETH0
       &enet0,
#endif

#ifdef BSP_USING_ETH1
       &enet1
#endif
};

ATTR_WEAK uint8_t enet_get_mac_address(ENET_Type *ptr, uint8_t *mac)
{
    uint32_t macl, mach;
    uint8_t i;

    i = (ptr == HPM_ENET0) ? 0 : 1;

        if (mac == NULL) {
            return ENET_MAC_ADDR_PARA_ERROR;
        }

        /* load mac address from OTP MAC area */
        if (i == 0) {
            macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX);
            mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);

            mac[0] = (macl >>  0) & 0xff;
            mac[1] = (macl >>  8) & 0xff;
            mac[2] = (macl >> 16) & 0xff;
            mac[3] = (macl >> 24) & 0xff;
            mac[4] = (mach >>  0) & 0xff;
            mac[5] = (mach >>  8) & 0xff;
			memcpy(current_mac[0], mac, 6);
        } else {
            macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);
            mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 2);

            mac[0] = (macl >> 16) & 0xff;
            mac[1] = (macl >> 24) & 0xff;
            mac[2] = (mach >>  0) & 0xff;
            mac[3] = (mach >>  8) & 0xff;
            mac[4] = (mach >> 16) & 0xff;
            mac[5] = (mach >> 24) & 0xff;
			memcpy(current_mac[1], mac, 6);
        }

        if (!IS_MAC_INVALID(mac)) {
            return ENET_MAC_ADDR_FROM_OTP_MAC;
        }

        /* load MAC address from MACRO definitions */
        memcpy(mac, &mac_init[i], ENET_MAC);
		memcpy(current_mac[i], &mac_init[i], 6);
        return ENET_MAC_ADDR_FROM_MACRO;
}

static rt_err_t hpm_enet_init(enet_device *init)
{
    if (init->media_interface == enet_inf_rmii)
    {
        /* Initialize reference clock */
        board_init_enet_rmii_reference_clock(init->instance, init->int_refclk);
    }

#if ENET_SOC_RGMII_EN
    /* Set RGMII clock delay */
   if (init->media_interface == enet_inf_rgmii)
   {
        enet_rgmii_enable_clock(init->instance);
        enet_rgmii_set_clock_delay(init->instance, init->tx_delay, init->rx_delay);
   }
#endif
    /* Get the default interrupt config */
    enet_get_default_interrupt_config(init->instance, &init->int_config);

    /* Initialize eth controller */
    enet_controller_init(init->instance, init->media_interface, &init->desc, &init->mac_config, &init->int_config);

    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(init->instance);

#if __USE_ENET_PTP
   /* initialize PTP Clock */
   board_init_enet_ptp_clock(init->instance);

   /* initialize Ethernet PTP Module */
   init->ptp_config.ssinc = ENET_ONE_SEC_IN_NANOSEC / clock_get_frequency(init->ptp_clk_src);
   enet_init_ptp(init->instance, &init->ptp_config);

   /* set the initial timestamp */
   enet_set_ptp_timestamp(init->instance, &init->ptp_timestamp);
#endif

   /* enable irq */
   intc_m_enable_irq(init->irq_number);

   return RT_EOK;
}

static rt_err_t rt_hpm_eth_init(rt_device_t dev)
{
    uint8_t mac[ENET_MAC];

    enet_device *enet_dev = (enet_device *)dev->user_data;

    /* Initialize GPIOs */
    board_init_enet_pins(enet_dev->instance);

    /* Reset an enet PHY */
    board_reset_enet_phy(enet_dev->instance);

    /* Get MAC address */
    enet_get_mac_address(enet_dev->instance, mac);

    /* Set mac0 address */
    enet_dev->mac_config.mac_addr_high[0] = mac[5] << 8 | mac[4];
    enet_dev->mac_config.mac_addr_low[0] = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    enet_dev->mac_config.valid_max_count = 1;

    /* Initialize MAC and DMA */
    if (hpm_enet_init(enet_dev) == 0)
    {
        LOG_D("Ethernet control initialize successfully\n");
        return RT_EOK;
    }
    else
    {
        LOG_D("Ethernet control initialize unsuccessfully\n");
        return -RT_ERROR;
    }
}

static rt_err_t rt_hpm_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_hpm_eth_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_ssize_t rt_hpm_eth_read(rt_device_t dev, rt_off_t pos, void * buffer, rt_size_t size)
{
    return 0;
}

static rt_ssize_t rt_hpm_eth_write(rt_device_t dev, rt_off_t pos, const void * buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t rt_hpm_eth_control(rt_device_t dev, int cmd, void * args)
{
    uint8_t *mac = (uint8_t *)args;
    enet_device *enet_dev = (enet_device *)dev->user_data;

    switch (cmd)
    {
    case NIOCTL_GADDR:
        if (args != NULL)
        {
            enet_get_mac_address(enet_dev->instance, (uint8_t *)mac);
            SMEMCPY(args, mac, ENET_MAC);
        }
        else
        {
            return -RT_ERROR;
        }
        break;
    default:
        break;
    }

    return RT_EOK;
}

ATTR_RAMFUNC static inline rt_err_t rt_hpm_eth_tx(rt_device_t dev, struct pbuf * p)
{
    uint32_t status;
    enet_device *enet_dev = (enet_device *)dev->user_data;
    uint32_t tx_buff_size = enet_dev->desc.tx_buff_cfg.size;
    struct pbuf *temp_p = NULL;
    uint32_t frame_length = 0;
    rt_tick_t t_start;
    static struct pbuf *old_p[2] = {NULL, NULL};

    if(p == NULL)
    {
        return ERR_BUF;
    }

    if(p->next != NULL)
    {
        printf("tx bad! no support next send!\r\n");
        return ERR_BUF;
    }
    if(p->len > tx_buff_size)
    {
        printf("tx bad! tx this case no support lage data:%d!\r\n", p->len);
        return ERR_BUF;
    }

    t_start = rt_tick_get();
    while (enet_dev->desc.tx_desc_list_cur->tdes0_bm.own)
    {
        if (rt_tick_get() - t_start > RT_TICK_PER_SECOND / 100)
        {
            return ERR_TIMEOUT;
        }
    }

    enet_dev->desc.tx_desc_list_cur->tdes2_bm.buffer1 = core_local_mem_to_sys_address(0, (uint32_t)p->payload);
    frame_length = p->len;
    frame_length += 4;
    status = enet_prepare_transmission_descriptors(enet_dev->instance, &enet_dev->desc.tx_desc_list_cur, frame_length, enet_dev->desc.tx_buff_cfg.size);
    if (status != ENET_SUCCESS)
    {
        printf("Ethernet controller transmit unsuccessfully: %d\n", status);
    }

    if(enet_dev->instance == HPM_ENET0)
    {
        if(old_p[0] == NULL)
        {
            old_p[0] = p;
            p = NULL;
        }
        else
        {
            temp_p = p;
            p = old_p[0];
            old_p[0] = temp_p;
        }
    }
    else
    {
        if(old_p[1] == NULL)
        {
            old_p[1] = p;
            p = NULL;
        }
        else
        {
            temp_p = p;
            p = old_p[1];
            old_p[1] = temp_p;
        }
    }

    return ERR_OK;
}

ATTR_RAMFUNC static inline struct pbuf *rt_hpm_loopnet_process(enet_device* enet_dev, struct pbuf* p)
{
    uint8_t index = 0;
    struct eth_hdr *ethhdr;
    if(p == NULL)
        return NULL;
    do
    {
        if(enet_dev == NULL)
            break;
        if(s_geths[0]->enet_dev == enet_dev)
        {
            index = 0;
        }
        else if(s_geths[1]->enet_dev == enet_dev)
        {
            index = 1;
        }
        else
        {
            break;
        }
        if (p->len <= SIZEOF_ETH_HDR)
        {
            /* a packet with only an ethernet header (or less) is not valid for us */
            break;
        }
        /* points to packet payload, which starts with an Ethernet header */
        ethhdr = (struct eth_hdr *)p->payload;
        if(memcmp(ethhdr->dest.addr, current_mac[0], ETH_HWADDR_LEN) == 0 ||
           memcmp(ethhdr->dest.addr, current_mac[1], ETH_HWADDR_LEN) == 0)
        {
            return p;
        }
        if (ethhdr->dest.addr[0] & 1)
        {
            rt_hpm_eth_tx(&(s_geths[index == 0 ? 1 : 0]->eth_dev->parent), p);
            return p;
        }
        else
        {
            rt_hpm_eth_tx(&(s_geths[index == 0 ? 1 : 0]->eth_dev->parent), p);
        }

    } while(0);

    pbuf_free(p);
    return NULL;
}

ATTR_RAMFUNC static inline struct pbuf *rt_hpm_eth_rx(rt_device_t dev)
{
    struct pbuf *p = NULL;// = &recv_pbuf;
    struct pbuf *ret_p = NULL;
    enet_device *enet_dev = (enet_device *)dev->user_data;
    enet_rx_desc_t *dma_rx_desc = NULL;
    uint32_t rx_buff_size = enet_dev->desc.rx_buff_cfg.size;
    enet_frame_t frame = {0, 0, 0};

    /* Get a received frame */
    frame = enet_get_received_frame_interrupt(&enet_dev->desc.rx_desc_list_cur,
                                              &enet_dev->desc.rx_frame_info,
                                              enet_dev->desc.rx_buff_cfg.count);
    do
    {
        if(frame.length <= 0)
        {
            break;
        }
        dma_rx_desc = frame.rx_desc;
        if(frame.length > rx_buff_size)
        {
            printf("bad! this case no support large data, len:%d!\r\n", frame.length);
            break;
        }
        p = pbuf_alloc(PBUF_RAW, (u16_t)frame.length, PBUF_POOL);
        if(p == NULL)
        {
           printf("bad pbuf alloc failed!\r\n");
           break;
        }
        p->tot_len = frame.length;
        p->len = frame.length;
        p->payload = (void *)sys_address_to_core_local_mem(0, (uint32_t)frame.buffer);
        ret_p = rt_hpm_loopnet_process(enet_dev, p);
    } while(0);

    if(dma_rx_desc != NULL)
    {
        /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
        for (int i = 0; i < enet_dev->desc.rx_frame_info.seg_count; i++)
        {
            dma_rx_desc->rdes0_bm.own = 1;
            dma_rx_desc = (enet_rx_desc_t*)(dma_rx_desc->rdes3_bm.next_desc);
        }

        /* Clear Segment_Count */
        enet_dev->desc.rx_frame_info.seg_count = 0;
    }

    /* Resume Rx Process */
    if (ENET_DMA_STATUS_RU_GET(enet_dev->instance->DMA_STATUS))
    {
        enet_dev->instance->DMA_STATUS = ENET_DMA_STATUS_RU_MASK;
        enet_dev->instance->DMA_RX_POLL_DEMAND = 1;
    }

    return ret_p;
}

static void eth_rx_callback(struct eth_device* dev)
{
    rt_err_t result;
    result = eth_device_ready(dev);
    if (result != RT_EOK)
    {
        LOG_I("Receive callback error = %d\n", result);
    }
}

void isr_enet(hpm_enet_t *obj)
{
    uint32_t status;

    status = obj->base->DMA_STATUS;

    if (ENET_DMA_STATUS_GLPII_GET(status)) {
        obj->base->LPI_CSR;
    }

    if (ENET_DMA_STATUS_RI_GET(status)) {
        obj->base->DMA_STATUS |= ENET_DMA_STATUS_RI_SET(ENET_DMA_STATUS_RI_GET(status));
        eth_rx_callback(obj->eth_dev);
    }
}

#ifdef BSP_USING_ETH0
void isr_enet0(void)
{
    isr_enet(&enet0);
}
RTT_DECLARE_EXT_ISR_M(IRQn_ENET0, isr_enet0)
#endif

#ifdef BSP_USING_ETH1
void isr_enet1(void)
{
    isr_enet(&enet1);
}
RTT_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
#endif

int rt_hw_eth_init(void)
{
    rt_err_t err = RT_ERROR;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_geths); i++)
    {
        /* Clear memory */
        memset((uint8_t *)s_geths[i]->dma_rx_desc_tab, 0x00, sizeof(enet_rx_desc_t) * s_geths[i]->rx_buff_cfg->count);
        memset((uint8_t *)s_geths[i]->dma_tx_desc_tab, 0x00, sizeof(enet_tx_desc_t) * s_geths[i]->tx_buff_cfg->count);

        memset((uint8_t *)s_geths[i]->rx_buff_cfg->buffer, 0x00, sizeof(s_geths[i]->rx_buff_cfg->size));
        memset((uint8_t *)s_geths[i]->tx_buff_cfg->buffer, 0x00, sizeof(s_geths[i]->tx_buff_cfg->size));

        /* Set list heads */
        s_geths[i]->enet_dev->desc.tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)s_geths[i]->dma_tx_desc_tab);
        s_geths[i]->enet_dev->desc.rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)s_geths[i]->dma_rx_desc_tab);

        s_geths[i]->enet_dev->desc.tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, s_geths[i]->tx_buff_cfg->buffer);
        s_geths[i]->enet_dev->desc.tx_buff_cfg.count = s_geths[i]->tx_buff_cfg->count;
        s_geths[i]->enet_dev->desc.tx_buff_cfg.size = s_geths[i]->tx_buff_cfg->size;

        s_geths[i]->enet_dev->desc.rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, s_geths[i]->rx_buff_cfg->buffer);
        s_geths[i]->enet_dev->desc.rx_buff_cfg.count = s_geths[i]->rx_buff_cfg->count;
        s_geths[i]->enet_dev->desc.rx_buff_cfg.size = s_geths[i]->rx_buff_cfg->size;

        /* Set DMA PBL */
        s_geths[i]->enet_dev->mac_config.dma_pbl = board_get_enet_dma_pbl(s_geths[i]->base);

        /* Set instance */
        s_geths[i]->enet_dev->instance = s_geths[i]->base;

        /* Set media interface */
        s_geths[i]->enet_dev->media_interface = s_geths[i]->inf ? enet_inf_rgmii : enet_inf_rmii;

        if (s_geths[i]->enet_dev->media_interface == enet_inf_rmii)
        {
            /* Set refclk  */
            s_geths[i]->enet_dev->int_refclk = s_geths[i]->int_refclk;
        } else {
            /* Set TX/RX delay */
            s_geths[i]->enet_dev->tx_delay = s_geths[i]->tx_delay;
            s_geths[i]->enet_dev->rx_delay = s_geths[i]->rx_delay;
        }


#if __USE_ENET_PTP
        /* Set PTP function */
        s_geths[i]->enet_dev->ptp_clk_src   = s_geths[i]->ptp_clk_src;
        s_geths[i]->enet_dev->ptp_config    = *s_geths[i]->ptp_config;
        s_geths[i]->enet_dev->ptp_timestamp = *s_geths[i]->ptp_timestamp;
#endif

        /* Set the irq number */
        s_geths[i]->enet_dev->irq_number = s_geths[i]->irq_num;

        /* Set the parent parameters */
        s_geths[i]->eth_dev->parent.init      = rt_hpm_eth_init;
        s_geths[i]->eth_dev->parent.open      = rt_hpm_eth_open;
        s_geths[i]->eth_dev->parent.close     = rt_hpm_eth_close;
        s_geths[i]->eth_dev->parent.read      = rt_hpm_eth_read;
        s_geths[i]->eth_dev->parent.write     = rt_hpm_eth_write;
        s_geths[i]->eth_dev->parent.control   = rt_hpm_eth_control;

        s_geths[i]->eth_dev->parent.user_data = s_geths[i]->enet_dev;

        s_geths[i]->eth_dev->eth_rx = rt_hpm_eth_rx;
        s_geths[i]->eth_dev->eth_tx = rt_hpm_eth_tx;

        err = eth_device_init(s_geths[i]->eth_dev, s_geths[i]->name);

        if (RT_EOK == err)
        {
            LOG_D("Ethernet device %d initialize successfully!\n", i);
        }
        else
        {
            LOG_D("Ethernet device %d initialize unsuccessfully!\n");
            return err;
        }
    }

    return err;

}
INIT_DEVICE_EXPORT(rt_hw_eth_init);
#endif /* BSP_USING_ETH */
