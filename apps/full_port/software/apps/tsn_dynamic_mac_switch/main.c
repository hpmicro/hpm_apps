/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "global_config.h"

static uint8_t s_u8ITFlagForKeyA = false;
static uint8_t s_u8ITFlagForKeyB = false;
static uint8_t s_u8ITFlagForKeyC = false;
static uint8_t s_u8ITFlagForKeyD = false;
static uint16_t s_u16TimeOutFor10MsCount = 0;
static uint16_t s_u16TimeOutFor100MsCount = 0;
static uint16_t s_u16TimeOutFor1000MsCount = 0;
/* key filter */
static uint8_t s_u8TimeOutForKeyA = 0;
static uint8_t s_u8TimeOutForKeyB = 0;
static uint8_t s_u8TimeOutForKeyC = 0;
static uint8_t s_u8TimeOutForKeyD = 0;	

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) volatile uint8_t recv_buff[TSN_RECV_DEEP][TSN_RECV_LENGTH];

tsn_mac_table_t stMacTable = {.mac[0] = 0x4e000000f050, .depth = 1, .mac_port[0]=0x01};

static void hpm_a_irq_timer_handler_cb(void)
{
	s_u16TimeOutFor10MsCount++;
	s_u16TimeOutFor100MsCount++;
	s_u16TimeOutFor1000MsCount++;

	if(s_u16TimeOutFor10MsCount>=10) {
		s_u16TimeOutFor10MsCount = 10;
	}
	if(s_u16TimeOutFor100MsCount>=100) {
		s_u16TimeOutFor100MsCount = 100;
	}
	if(s_u16TimeOutFor1000MsCount>=1000) {
		s_u16TimeOutFor1000MsCount = 1000;
	}

		

	if(s_u8ITFlagForKeyA) {
		s_u8TimeOutForKeyA++;
		if(s_u8TimeOutForKeyA >= 50) {
			s_u8TimeOutForKeyA = 50;
			s_u8ITFlagForKeyA = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYA_ID)) {
				s_u8TimeOutForKeyA = 0;
			}
		}
	}

	if(s_u8ITFlagForKeyB) {
		s_u8TimeOutForKeyB++;
		if(s_u8TimeOutForKeyB >= 50) {
			s_u8TimeOutForKeyB = 50;
			s_u8ITFlagForKeyB = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYB_ID)) {
				s_u8TimeOutForKeyB = 0;
			}
		}
	}

	if(s_u8ITFlagForKeyC) {
		s_u8TimeOutForKeyC++;
		if(s_u8TimeOutForKeyC >= 50) {
			s_u8TimeOutForKeyC = 50;
			s_u8ITFlagForKeyC = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYC_ID)) {
				s_u8TimeOutForKeyC = 0;
			}
		}
	}

	if(s_u8ITFlagForKeyD) {
		s_u8TimeOutForKeyD++;
		if(s_u8TimeOutForKeyD >= 50) {
			s_u8TimeOutForKeyD = 50;
			s_u8ITFlagForKeyD = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYD_ID)) {
				s_u8TimeOutForKeyD = 0;
			}
		}
	}
}

static void hpm_a_irq_keya_handler_cb(void)
{
	s_u8ITFlagForKeyA = true;
}

static void hpm_a_irq_keyb_handler_cb(void)
{
	s_u8ITFlagForKeyB = true;
}

static void hpm_a_irq_keyc_handler_cb(void)
{
	s_u8ITFlagForKeyC = true;
}

static void hpm_a_irq_keyd_handler_cb(void)
{
	s_u8ITFlagForKeyD = true;
}

static void hpm_a_irq_tsn0_handler_cb(void)
{
	static uint32_t num = 0;
	if(full_port_tsn_get_recv_dma_irq_state()) {
		do {
			uint32_t u32RecvResp = full_port_tsn_get_recv_dma_resp();
			uint8_t u8Id = TSW_S2MM_RESP_ID_GET(u32RecvResp);
			uint16_t u16Len = TSW_S2MM_RESP_LENGTH_GET(u32RecvResp);

			if(!TSW_S2MM_RESP_DECERR_GET(u32RecvResp) && !TSW_S2MM_RESP_SLVERR_GET(u32RecvResp)) {
				uint64_t u64DesMac = ((uint64_t)recv_buff[u8Id][16]) << 40|
									((uint64_t)recv_buff[u8Id][17]) << 32 |
									((uint64_t)recv_buff[u8Id][18]) << 24 |
									((uint64_t)recv_buff[u8Id][19]) << 16 |
									((uint64_t)recv_buff[u8Id][20]) << 8  |
									((uint64_t)recv_buff[u8Id][21]) << 0  ;          
				uint64_t u64SrcMac = ((uint64_t)recv_buff[u8Id][22]) << 40|
									((uint64_t)recv_buff[u8Id][23]) << 32 |
									((uint64_t)recv_buff[u8Id][24]) << 24 |
									((uint64_t)recv_buff[u8Id][25]) << 16 |
									((uint64_t)recv_buff[u8Id][26]) << 8  |
									((uint64_t)recv_buff[u8Id][27]) << 0  ;
				//ARP?
				if((recv_buff[u8Id][28] == 0x08) && (recv_buff[u8Id][29] == 0x06)) {
					uint32_t i=1;
					for(i=1; i<stMacTable.depth; i++) 
					{
						if(stMacTable.mac[i] == u64SrcMac) {
							if((stMacTable.mac_port[i] & (1<<recv_buff[u8Id][0]))) {

							} else {
								full_port_tsn_clear_hit(i);
								stMacTable.mac_port[i] |= (1<<recv_buff[u8Id][0]);
								full_port_tsn_set_lookup_table(i, stMacTable.mac_port[i], stMacTable.mac[i]);
							}
							break; //break for
						}
					}
					if(i==stMacTable.depth) {
						stMacTable.depth++;
						stMacTable.mac_port[i] |= (1<<recv_buff[u8Id][0]);
						stMacTable.mac[i] = u64SrcMac;
						full_port_tsn_set_lookup_table(i, stMacTable.mac_port[i], stMacTable.mac[i]);
					}
				}

				if((u64DesMac == 0xffffffffffff) || (u64DesMac == stMacTable.mac[0])) {
					num++;
					FULL_PORT_LOG_INFO_ARRAY(&recv_buff[u8Id][16], 16, "%s %d ", "packet", num);
				}
				full_port_tsn_recv_desc((uint32_t*)&recv_buff[u8Id][0], TSN_RECV_LENGTH, u8Id);
			} else {
				FULL_PORT_LOG_ERROR("tsn recv err\r\n");
			}
		} while (!TSW_S2MM_DMA_SR_RBUFE_GET(full_port_tsn_get_recv_dma_sr()));
		full_port_tsn_clear_recv_dma_irq_state();
	}
}

int main(void)
{
	uint8_t u8Port1Mac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
	uint8_t u8Port2Mac[6] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
	uint8_t u8Port3Mac[6] = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
	uint16_t u16Value;

    board_init();
    board_timer_create(1, hpm_a_irq_timer_handler_cb);
	printf("dynamic mac switch\r\n");
    full_port_lcd_init();
    full_port_led_init();
    full_port_key_init();
    full_port_tsn_init();
	
	for(int i=0; i<TSN_RECV_DEEP; i++)
	{
		full_port_tsn_set_recv((uint32_t*)(&(recv_buff[i][0])), TSN_RECV_LENGTH, i);
	}

	full_port_tsn_set_broadcast_frame_action(tsw_dst_port_1 | tsw_dst_port_2 | tsw_dst_port_3);
	full_port_tsn_set_unknown_frame_action(tsw_dst_port_null);

	full_port_tsn_clear_all_hit();

    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_MIRROR |= 1;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_MIRROR_TX |= 0xe;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT2].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_MIRROR |= 1;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT2].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_MIRROR_TX |= 0xe;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT3].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_MIRROR |= 1;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT3].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_MIRROR_TX |= 0xe;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG |= 1<<9;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT2].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG |= 1<<9;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT3].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG |= 1<<9;

	full_port_tsn_set_lookup_table(0, tsw_dst_port_cpu, stMacTable.mac[0]);

    board_tsw_phy_set(TSW_TSNPORT_PORT1, true);
    board_tsw_phy_set(TSW_TSNPORT_PORT2, true);
    board_tsw_phy_set(TSW_TSNPORT_PORT3, true);

	full_port_irq_keya_init(hpm_a_irq_keya_handler_cb);
	full_port_irq_keyb_init(hpm_a_irq_keyb_handler_cb);
	full_port_irq_keyc_init(hpm_a_irq_keyc_handler_cb);
	full_port_irq_keyd_init(hpm_a_irq_keyd_handler_cb);
	full_port_irq_tsn0_init(hpm_a_irq_tsn0_handler_cb);


    while (1)
    {
        if(s_u16TimeOutFor1000MsCount>=1000) {
			s_u16TimeOutFor1000MsCount = 0;
            full_port_led_ctrl(FULL_PORT_LED_INDEX_LEDA_ID, FULL_PORT_LED_CTRL_TOGGLE);
        }

        if(s_u16TimeOutFor100MsCount>=100) {
			s_u16TimeOutFor100MsCount = 0;
            full_port_led_ctrl(FULL_PORT_LED_INDEX_LEDB_ID, FULL_PORT_LED_CTRL_TOGGLE);
        }

        if(s_u16TimeOutFor10MsCount>=10) {
			s_u16TimeOutFor10MsCount = 0;

        }

        if(s_u8TimeOutForKeyA>=50) {
			s_u8TimeOutForKeyA = 0;
			FULL_PORT_LOG_INFO("KEYA\r\n");
			FULL_PORT_LOG_INFO("10M\r\n");
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT1, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT1 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT1, 1, 0, 0x100);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT1, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT1 REG0:0x%02x\r\n",u16Value);  
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT2, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT2 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT2, 1, 0, 0x100);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT2, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT2 REG0:0x%02x\r\n",u16Value);  
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT3, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT3 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT3, 1, 0, 0x100);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT3, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT3 REG0:0x%02x\r\n",u16Value);   
			/*PORT1/2/3 GPR设置*/
			full_port_tsn_general_config(TSW_TSNPORT_PORT1, tsw_port_speed_10mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			full_port_tsn_general_config(TSW_TSNPORT_PORT2, tsw_port_speed_10mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			full_port_tsn_general_config(TSW_TSNPORT_PORT3, tsw_port_speed_10mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			/* PORT1/2/3 MAC 设置 */
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT1, u8Port1Mac, tsw_mac_mode_mii, TSW_RXFIFO_E1);
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT2, u8Port2Mac, tsw_mac_mode_mii, TSW_RXFIFO_E1);
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT3, u8Port3Mac, tsw_mac_mode_mii, TSW_RXFIFO_E1);
        }
        if(s_u8TimeOutForKeyB>=50) {
			s_u8TimeOutForKeyB = 0;
			FULL_PORT_LOG_INFO("KEYB\r\n");
			FULL_PORT_LOG_INFO("100M\r\n");
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT1, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT1 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT1, 1, 0, 0x2100);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT1, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT1 REG0:0x%02x\r\n",u16Value);  
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT2, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT2 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT2, 1, 0, 0x2100);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT2, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT2 REG0:0x%02x\r\n",u16Value);  
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT3, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT3 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT3, 1, 0, 0x2100);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT3, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT3 REG0:0x%02x\r\n",u16Value);   
			/*PORT1/2/3 GPR设置*/
			full_port_tsn_general_config(TSW_TSNPORT_PORT1, tsw_port_speed_100mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			full_port_tsn_general_config(TSW_TSNPORT_PORT2, tsw_port_speed_100mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			full_port_tsn_general_config(TSW_TSNPORT_PORT3, tsw_port_speed_100mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			/* PORT1/2/3 MAC 设置 */
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT1, u8Port1Mac, tsw_mac_mode_mii, TSW_RXFIFO_E1);
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT2, u8Port2Mac, tsw_mac_mode_mii, TSW_RXFIFO_E1);
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT3, u8Port3Mac, tsw_mac_mode_mii, TSW_RXFIFO_E1);
        }
        if(s_u8TimeOutForKeyC>=50) {
			s_u8TimeOutForKeyC = 0;
			FULL_PORT_LOG_INFO("KEYC\r\n");
			FULL_PORT_LOG_INFO("1000M\r\n");
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT1, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT1 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT1, 1, 0, 0x1140);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT1, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT1 REG0:0x%02x\r\n",u16Value);  
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT2, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT2 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT2, 1, 0, 0x1140);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT2, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT2 REG0:0x%02x\r\n",u16Value);  
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT3, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT3 REG0:0x%02x\r\n",u16Value);
			full_port_tsn_ep_mdio_write(TSW_TSNPORT_PORT3, 1, 0, 0x1140);
			full_port_tsn_ep_mdio_read(TSW_TSNPORT_PORT3, 1, 0, &u16Value);
			FULL_PORT_LOG_INFO("PORT3 REG0:0x%02x\r\n",u16Value);   
			/*PORT1/2/3 GPR设置*/
			full_port_tsn_general_config(TSW_TSNPORT_PORT1, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			full_port_tsn_general_config(TSW_TSNPORT_PORT2, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			full_port_tsn_general_config(TSW_TSNPORT_PORT3, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
			/* PORT1/2/3 MAC 设置 */
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT1, u8Port1Mac, tsw_mac_mode_gmii, TSW_RXFIFO_E1);
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT2, u8Port2Mac, tsw_mac_mode_gmii, TSW_RXFIFO_E1);
			full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT3, u8Port3Mac, tsw_mac_mode_gmii, TSW_RXFIFO_E1);
        }
        if(s_u8TimeOutForKeyD>=50) {
			s_u8TimeOutForKeyD = 0;
            FULL_PORT_LOG_INFO("s_u8TimeOutForKeyD\r\n"); 
        }
    }
    return 0;
}
