/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "drv_enet_tsn.h"
#include "common.h"
#include "hpm_swap.h"

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT]; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT]; /* Ethernet Tx DMA Descriptor */

ATTR_PLACE_AT_FAST_RAM_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

ATTR_PLACE_AT_FAST_RAM_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;
uint8_t mac[_ENET_PORT_COUNT][ENET_MAC_SIZE];

static void set_lookup_table(TSW_Type *ptr, uint16_t entry_num, uint8_t dest_port, uint64_t dest_mac, uint8_t vid)
{
    uint64_t dest_mac_temp;

    /* Create a new ALMEM entry. This will specify what will be done with those detected frames */
    if (TSW_APB2AXIS_ALMEM_STS_RDY_GET(ptr->APB2AXIS_ALMEM_STS)) {

        ptr->APB2AXIS_ALMEM_REQDATA_1 = TSW_APB2AXIS_ALMEM_REQDATA_1_WR_NRD_SET(1) | TSW_APB2AXIS_ALMEM_REQDATA_1_ENTRY_NUM_SET(entry_num);

        /* set forward to destination port, use PCP field, UTAG 1 and trigger the interface for sending the data */
        ptr->APB2AXIS_ALMEM_REQDATA_0 = TSW_APB2AXIS_ALMEM_REQDATA_0_UTAG_SET(1) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_QSEL_SET(0) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_DROP_SET(0) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_QUEUE_SET(0) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_DEST_SET(dest_port);
    }

    /* Create a new CAM entry */
    dest_mac_temp = __bswapdi2(dest_mac) >> 16;

    ptr->APB2AXIS_CAM_REQDATA_2 = TSW_APB2AXIS_CAM_REQDATA_2_VID_SET(vid) | TSW_APB2AXIS_CAM_REQDATA_2_DESTMAC_HI_SET((dest_mac_temp >> 32));
    ptr->APB2AXIS_CAM_REQDATA_1 = TSW_APB2AXIS_CAM_REQDATA_1_DESTMAC_LO_PORT_VEC_SET(dest_mac_temp);
    ptr->APB2AXIS_CAM_REQDATA_0 = TSW_APB2AXIS_CAM_REQDATA_0_ENTRY_NUM_SET(entry_num) |
                                 TSW_APB2AXIS_CAM_REQDATA_0_TYPE_SET(1) |   /* Set one DEST_MAC/VLAN_ID entry */
                                 TSW_APB2AXIS_CAM_REQDATA_0_CH_SET(0);      /* CAM APB2AXIS channel selection. Always 0 for writing to DEST_MAC/VLAN_ID */
}

hpm_stat_t enet_init(ENET_Type *ptr)
{
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;
    enet_tx_control_config_t enet_tx_control_config;

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
    enet_get_default_tx_control_config(ptr, &enet_tx_control_config);

    /* Set the control config for tx descriptor */
    memcpy(&desc.tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[0][5] << 8 | mac[0][4];
    enet_config.mac_addr_low[0]  = mac[0][3] << 24 | mac[0][2] << 16 | mac[0][1] << 8 | mac[0][0];
    enet_config.valid_max_count  = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ptr);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ptr, &int_config);

    /* Initialize enet controller */
    if (enet_controller_init(ptr, enet_inf_rgmii, &desc, &enet_config, &int_config) != status_success) {
        LOG_E("Enet controller init failed !\n");  
        return status_fail;
    }

    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ptr);

    board_init_enet_rgmii_clock_delay(ptr);

    enet_set_line_speed(ptr, enet_line_speed_1000mbps);

    enet_set_duplex_mode(ptr, enet_full_duplex);

    return status_success;

}

hpm_stat_t tsw_init(TSW_Type *ptr)
{
    yt8531c_config_t phy_config;

    /* Disable all MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Set Mac Address */
    tsw_ep_set_mac_addr(ptr, BOARD_TSN_P1_PORT, mac[0], true);
    tsw_ep_set_mac_addr(ptr, BOARD_TSN_P2_PORT, mac[0], true);
    tsw_ep_set_mac_addr(ptr, BOARD_TSN_P3_PORT, mac[0], true);

    tsw_ep_set_mac_mode(ptr, BOARD_TSN_P1_PORT, tsw_mac_mode_gmii);
    tsw_ep_set_mac_mode(ptr, BOARD_TSN_P2_PORT, tsw_mac_mode_gmii);
    tsw_ep_set_mac_mode(ptr, BOARD_TSN_P3_PORT, tsw_mac_mode_gmii);

    tsw_port_gpr(ptr, BOARD_TSN_P1_PORT, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0, 0);
    tsw_port_gpr(ptr, BOARD_TSN_P2_PORT, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0, 0);
    tsw_port_gpr(ptr, BOARD_TSN_P3_PORT, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0, 0);

    /* Set port PHY interface */
    tsw_set_port_interface(ptr, BOARD_TSN_P1_PORT, tsw_port_phy_itf_rgmii);
    tsw_set_port_interface(ptr, BOARD_TSN_P2_PORT, tsw_port_phy_itf_rgmii);
    tsw_set_port_interface(ptr, BOARD_TSN_P3_PORT, tsw_port_phy_itf_rgmii);

    tsw_set_port_clock_delay(ptr, BOARD_TSN_P1_PORT, 8, 0);
    tsw_set_port_clock_delay(ptr, BOARD_TSN_P2_PORT, 8, 0);
    tsw_set_port_clock_delay(ptr, BOARD_TSN_P3_PORT, 0, 8);

    tsw_disable_store_forward_mode(ptr, BOARD_TSN_P1_PORT);
    tsw_disable_store_forward_mode(ptr, BOARD_TSN_P2_PORT);
    tsw_disable_store_forward_mode(ptr, BOARD_TSN_P3_PORT);

    /* Enable all MACs(TX/RX) */
    tsw_ep_enable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Clear CAM */
    tsw_clear_cam(ptr);

    /* Wait for cam clearing completion */
    board_delay_ms(10);

    /* Get MAC address */
#if _ENET_PORT_COUNT == 2
    enet_get_mac_address(mac[0], 0);
    enet_get_mac_address(mac[1], 1);
#else
    enet_get_mac_address(mac[0], 0);
#endif

#if _ENET_PORT_COUNT == 2
    ptr->TSNPORT[BOARD_TSN_P1_PORT].SW_CTRL_PORT_MAIN_TAGGING = 11; //PORT1 PVID
    ptr->TSNPORT[BOARD_TSN_P2_PORT].SW_CTRL_PORT_MAIN_TAGGING = 12; //PORT2 PVID
    ptr->TSNPORT[BOARD_TSN_P3_PORT].SW_CTRL_PORT_MAIN_TAGGING = 1;  //PORT3 PVID

    ptr->APB2AXIS_CAM_REQDATA_1 = 0x0C;
    ptr->APB2AXIS_CAM_REQDATA_0 = (12 << 16)  /* PORT2 PORT3 VID = 12 */
                               | (1 << 8)   /* 1: Set one VLAN_PORT entry */
                               | (1 << 0);  /* CAM APB2AXIS channel selection. Always 1 for writing to VLAN_PORT table. */
    board_delay_ms(10);  
    ptr->APB2AXIS_CAM_REQDATA_1 = 0x0A;
    ptr->APB2AXIS_CAM_REQDATA_0 = (11 << 16)  /* PORT1 PORT3 VID = 11 */
                               | (1 << 8)   /* 1: Set one VLAN_PORT entry */
                               | (1 << 0);  /* CAM APB2AXIS channel selection. Always 1 for writing to VLAN_PORT table. */
#else
    /* Enable VLAN-ID 1 at all ports */
    tsw_set_cam_vlan_port(ptr);
#endif

    /* Set system cycle numbers */
    tsw_cb_frer_set_msec_cycles(ptr, 0);

    /* Set broadcast frame and unknown frame actions */
    tsw_set_broadcast_frame_action(ptr, tsw_dst_port_1 | tsw_dst_port_2 | tsw_dst_port_3);
    tsw_set_unknown_frame_action(ptr, tsw_dst_port_1 | tsw_dst_port_2);
    tsw_set_internal_frame_action(ptr, tsw_dst_port_null);
    
    uint64_t mac_addr = 0;
    #if _ENET_PORT_COUNT == 2
    mac_addr = (uint64_t)mac[0][0]<<40 |
               (uint64_t)mac[0][1]<<32 |
               (uint64_t)mac[0][2]<<24 |
               (uint64_t)mac[0][3]<<16 |
               (uint64_t)mac[0][4]<<8  |
               (uint64_t)mac[0][5]<<0;
    set_lookup_table(ptr, 1, tsw_dst_port_3, mac_addr, 11);
    mac_addr = (uint64_t)mac[1][0]<<40 |
               (uint64_t)mac[1][1]<<32 |
               (uint64_t)mac[1][2]<<24 |
               (uint64_t)mac[1][3]<<16 |
               (uint64_t)mac[1][4]<<8  |
               (uint64_t)mac[1][5]<<0;
    set_lookup_table(ptr, 2, tsw_dst_port_3, mac_addr, 12);
    #else
    mac_addr = (uint64_t)mac[0][0]<<40 |
               (uint64_t)mac[0][1]<<32 |
               (uint64_t)mac[0][2]<<24 |
               (uint64_t)mac[0][3]<<16 |
               (uint64_t)mac[0][4]<<8  |
               (uint64_t)mac[0][5]<<0;
    set_lookup_table(ptr, 1, tsw_dst_port_3, mac_addr, 1);
    #endif

    /* Set MDC clock frequency to 2.5MHz */
    tsw_ep_set_mdio_config(ptr, BOARD_TSN_P1_PORT, 19);
    tsw_ep_set_mdio_config(ptr, BOARD_TSN_P2_PORT, 19);

    /* Initialize PHY */
    yt8531c_reset(ptr, BOARD_TSN_P1_PORT, YT8531C_P1_ADDR);
    yt8531c_reset(ptr, BOARD_TSN_P2_PORT, YT8531C_P2_ADDR);
    yt8531c_basic_mode_default_config(ptr, &phy_config);
    if (yt8531c_basic_mode_init(ptr, BOARD_TSN_P1_PORT, YT8531C_P1_ADDR, &phy_config) == true) {
        LOG_I("TSN P1 phy init passed !\r\n");
    } else {
        LOG_E("TSN P1 phy init failed !\r\n");
        return status_fail;
    }
    if (yt8531c_basic_mode_init(ptr, BOARD_TSN_P2_PORT, YT8531C_P2_ADDR, &phy_config) == true) {
        LOG_I("TSN P2 phy init passed !\r\n");
    } else {
        LOG_E("TSN P2 phy init failed !\r\n");
        return status_fail;
    }

    return status_success;
}


void drv_enet_tsn_init(void)
{
    board_init_tsw();
    board_init_enet(BOARD_ENET);
    tsw_init(BOARD_TSN);
    enet_init(BOARD_ENET);
}
