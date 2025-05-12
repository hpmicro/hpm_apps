/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_TSN_DRV_H_
#define __FULL_PORT_TSN_DRV_H_

#define FULL_PORT_IRQ_TSN0_IRQ                  IRQn_TSW_0
#define FULL_PORT_IRQ_TSN0_PRIORITY             10
#define FULL_PORT_IRQ_TSN1_IRQ                  IRQn_TSW_1
#define FULL_PORT_IRQ_TSN1_PRIORITY             10
#define FULL_PORT_IRQ_TSN2_IRQ                  IRQn_TSW_2
#define FULL_PORT_IRQ_TSN2_PRIORITY             10
#define FULL_PORT_IRQ_TSN3_IRQ                  IRQn_TSW_3
#define FULL_PORT_IRQ_TSN3_PRIORITY             10

#define MASTER 0
#define SLAVER 1

typedef struct{
    uint64_t mac[128];
    uint8_t  mac_port[128];
    uint32_t depth;
} tsn_mac_table_t;

void full_port_tsn_general_config(uint8_t u8Port, uint8_t u8Speed, uint8_t u8Itf, uint8_t u8TxDelay, uint8_t u8RxDelay);
void full_port_tsn_ep_mac_config(uint8_t u8Port, uint8_t *pu8Mac, uint8_t u8MacMode, uint8_t u8MacSelect);
void full_port_tsn_set_broadcast_frame_action(uint8_t u8DestPort);
void full_port_tsn_set_unknown_frame_action(uint8_t u8DestPort);
hpm_stat_t full_port_tsn_ep_mdio_write(uint8_t u8Port, uint32_t u32PhyAddr, uint32_t u32RegAddr, uint16_t u16Data);
hpm_stat_t full_port_tsn_ep_mdio_read(uint8_t u8Port, uint32_t u32PhyAddr, uint32_t u32RegAddr, uint16_t* pu16Data);
void full_port_tsn_set_lookup_table(uint16_t u16Entry, uint8_t u8DestPort, uint64_t u64DestMac);
void full_port_tsn_enable_recv_dma_irq(bool bEnable);
uint8_t full_port_tsn_get_recv_dma_irq_state(void);
void full_port_tsn_clear_recv_dma_irq_state(void);
uint32_t full_port_tsn_get_recv_dma_sr(void);
uint32_t full_port_tsn_get_recv_dma_resp(void);
void full_port_tsn_set_recv(uint8_t *pu8buf, uint32_t u32Len, uint8_t u8Id);
void full_port_tsn_set_send(uint8_t *pu8buf, uint32_t u32Len, uint8_t u8Id);
/**
 * @brief 清除所有命中标志位; clear all hit
 */
void full_port_tsn_clear_all_hit(void);
/**
 * @brief 清除命中标志位; clear hit
 * @param[in] u8Entry mac 表的入口; mac table entry
 */
void full_port_tsn_clear_hit(uint8_t u8Entry);
void full_port_tsn_set_rtc_inc(uint32_t u32TimerInc);
void full_port_tsn_set_rtc_timer_a(uint32_t u32TimerAPeriod, bool bEnable);
void full_port_tsn_enable_rtc_timer_a_irq(bool bEnable);
uint8_t full_port_tsn_get_rtc_timer_a_irq_state(void);
void full_port_tsn_set_rtc_offset(uint32_t u32Ns, uint32_t u32SSl, uint32_t u32SSh, uint32_t u32Ch);
void full_port_tsn_set_bin_txdata(uint8_t u8Port, uint8_t u8Index, uint32_t* pu32TxData, uint8_t u8Len);
void full_port_tsn_init(void);
void full_port_irq_tsn0_init(full_port_irq_cb_func cb);


#endif // __FULL_PORT_TSN_H_