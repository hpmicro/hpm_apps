/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "full_port_config_drv.h"
#include "full_port_tsn_as_drv.h"

FULL_PORT_PTP_Based_TSN_Sync_For_TX_st stPTPSyncForTX = {
    .des_mac = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}, //this is needed des mac
    .src_mac = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}, //src mac
    .protocol = {0x88, 0xf7}, //ptp protocol
    .stPtpProtocolHead={
        .offset0_bm = { .messageId = 0, .transportSpecific = 1 },
        .offset1_bm = { .versionPTP = 2 },
        .messageLen = { 0, 44 },
        .domainNum = 1,
        .control = 0,
    }
};

FULL_PORT_PTP_Based_TSN_FollowUp_For_TX_st stPTPFollowUpForTX = {
    .des_mac = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}, //this is needed des mac
    .src_mac = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}, //src mac
    .protocol = {0x88, 0xf7}, //ptp protocol
    .stPtpProtocolHead={
        .offset0_bm = { .messageId = 8, .transportSpecific = 1 },
        .offset1_bm = { .versionPTP = 2 },
        .messageLen = { 0, 44 },
        .domainNum = 1,
        .control = 2,
    }
};

FULL_PORT_PTP_Based_TSN_DelayReq_For_TX_st stPTPDelayReqForTX = {
    .des_mac = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}, //this is needed des mac
    .src_mac = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}, //src mac
    .protocol = {0x88, 0xf7}, //ptp protocol
    .stPtpProtocolHead={
        .offset0_bm = { .messageId = 1, .transportSpecific = 1 },
        .offset1_bm = { .versionPTP = 2 },
        .messageLen = { 0, 44 },
        .domainNum = 1,
        .control = 1,
    }
};

FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st stPTPDelayRespForTX = {
    .des_mac = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}, //this is needed des mac
    .src_mac = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}, //src mac
    .protocol = {0x88, 0xf7}, //ptp protocol
    .stPtpProtocolHead={
        .offset0_bm = { .messageId = 9, .transportSpecific = 1 },
        .offset1_bm = { .versionPTP = 2 },
        .messageLen = { 0, 54 },
        .domainNum = 1,
        .control = 3,
    }
};

uint32_t au32RxPtp[60];

static void full_port_tsn_pin_init(void)
{
    board_init_tsw();
}

void full_port_tsn_general_config(uint8_t u8Port, uint8_t u8Speed, uint8_t u8Itf, uint8_t u8TxDelay, uint8_t u8RxDelay)
{
    tsw_port_gpr(BOARD_TSN, u8Port, u8Speed, u8Itf, u8TxDelay, u8RxDelay);
}

void full_port_tsn_ep_mac_config(uint8_t u8Port, uint8_t *pu8Mac, uint8_t u8MacMode, uint8_t u8MacSelect)
{
    tsw_ep_disable_mac_ctrl(BOARD_TSN, u8Port, u8MacSelect);
    tsw_ep_set_mac_addr(BOARD_TSN, u8Port, pu8Mac, true);
    tsw_ep_set_mac_mode(BOARD_TSN, u8Port, u8MacMode);
    tsw_ep_set_mdio_config(BOARD_TSN, u8Port, 19);
    tsw_ep_enable_mac_ctrl(BOARD_TSN, u8Port, u8MacSelect);
}

void full_port_tsn_set_broadcast_frame_action(uint8_t u8DestPort)
{
    tsw_set_broadcast_frame_action(BOARD_TSN, u8DestPort);
}

void full_port_tsn_set_unknown_frame_action(uint8_t u8DestPort)
{
    tsw_set_unknown_frame_action(BOARD_TSN, u8DestPort);
}

hpm_stat_t full_port_tsn_ep_mdio_write(uint8_t u8Port, uint32_t u32PhyAddr, uint32_t u32RegAddr, uint16_t u16Data)
{
    return tsw_ep_mdio_write(BOARD_TSN, u8Port, u32PhyAddr, u32RegAddr, u16Data);
}

hpm_stat_t full_port_tsn_ep_mdio_read(uint8_t u8Port, uint32_t u32PhyAddr, uint32_t u32RegAddr, uint16_t* pu16Data)
{
    return tsw_ep_mdio_read(BOARD_TSN, u8Port, u32PhyAddr, u32RegAddr, pu16Data);
}

void full_port_tsn_set_lookup_table(uint16_t u16Entry, uint8_t u8DestPort, uint64_t u64DestMac)
{
    tsw_set_lookup_table(BOARD_TSN, u16Entry, u8DestPort, u64DestMac);
}

void full_port_tsn_enable_recv_dma_irq(bool bEnable)
{
    if(bEnable) {
        BOARD_TSN->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_IRQEN_MASK;        
    } else {
        BOARD_TSN->S2MM_DMA_CR &= ~TSW_S2MM_DMA_CR_IRQEN_MASK;   
    }
}

uint8_t full_port_tsn_get_recv_dma_irq_state(void)
{
    return (BOARD_TSN->S2MM_DMA_SR & TSW_S2MM_DMA_SR_IRQ_MASK) ? 1 : 0;
}

void full_port_tsn_clear_recv_dma_irq_state(void)
{
    HPM_TSW->S2MM_DMA_SR = 1<<3;
}

uint32_t full_port_tsn_get_recv_dma_sr(void)
{
    return HPM_TSW->S2MM_DMA_SR;
}

uint32_t full_port_tsn_get_recv_dma_resp(void)
{
    return BOARD_TSN->S2MM_RESP;
}

void full_port_tsn_recv_desc(uint32_t *pu32buf, uint32_t u32Len, uint8_t u8Id)
{
    tsw_commit_recv_desc(BOARD_TSN, (uint8_t*)pu32buf, u32Len, u8Id);    
}

void full_port_tsn_set_recv(uint8_t *pu8buf, uint32_t u32Len, uint8_t u8Id)
{
    /* Enable DMA-CTRL: Write S2MM_DMA_CR (Run=1, SOE=0, IRQEN=1) */
    BOARD_TSN->S2MM_DMA_CR &= ~TSW_S2MM_DMA_CR_SOE_MASK; /* SOE=0 */

    BOARD_TSN->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_RUN_MASK;

    /* Set ADDRL */
    BOARD_TSN->S2MM_ADDRLO = (uint32_t)pu8buf;

    /* Set Length */
    BOARD_TSN->S2MM_LENGTH = u32Len;

    /* Set Ctrl */
    BOARD_TSN->S2MM_CTRL &= ~TSW_S2MM_CTRL_ID_MASK;
    BOARD_TSN->S2MM_CTRL |= TSW_S2MM_CTRL_GO_MASK | TSW_S2MM_CTRL_ID_SET(u8Id); 
}

void full_port_tsn_set_send(uint8_t *pu8buf, uint32_t u32Len, uint8_t u8Id)
{
    tsw_send_frame(BOARD_TSN, pu8buf, u32Len, u8Id);
}

void full_port_tsn_clear_all_hit(void)
{
    HPM_TSW->HITMEM[0] = 0;
    HPM_TSW->HITMEM[1] = 0;
    HPM_TSW->HITMEM[2] = 0;
    HPM_TSW->HITMEM[3] = 0;
}

void full_port_tsn_clear_hit(uint8_t u8Entry)
{
    uint8_t u8HitReg=u8Entry/32;

    if(u8HitReg == TSW_HITMEM_HITMEM_REG_1) {
        HPM_TSW->HITMEM[0] &= ~(1ul << (u8Entry-0));
    } else if (u8HitReg == TSW_HITMEM_HITMEM_REG_2) {
        HPM_TSW->HITMEM[1] &= ~(1ul << (u8Entry-32));
    } else if (u8HitReg == TSW_HITMEM_HITMEM_REG_3) {
        HPM_TSW->HITMEM[2] &= ~(1ul << (u8Entry-64));
    } else if (u8HitReg == TSW_HITMEM_HITMEM_REG_4) {
        HPM_TSW->HITMEM[3] &= ~(1ul << (u8Entry-96));
    }
}

void full_port_tsn_set_rtc_inc(uint32_t u32TimerInc)
{
    BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_TIMER_INCR = u32TimerInc;
}

void full_port_tsn_set_rtc_timer_a(uint32_t u32TimerAPeriod, bool bEnable)
{
    BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_TIMER_A_PERIOD = u32TimerAPeriod;
    if(bEnable) {
        BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_CR |= TSW_TSNPORT_RTC_CR_TAEN_MASK;
    } else {
        BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_CR &= ~TSW_TSNPORT_RTC_CR_TAEN_MASK;
    }

}

void full_port_tsn_enable_rtc_timer_a_irq(bool bEnable)
{
    if(bEnable) {
        BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_CR |= TSW_TSNPORT_RTC_CR_TAIE_MASK;      
    } else {
        BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_CR &= ~TSW_TSNPORT_RTC_CR_TAIE_MASK;
    }
}

uint8_t full_port_tsn_get_rtc_timer_a_irq_state(void)
{
    return (BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_SR  & TSW_TSNPORT_RTC_SR_TAIS_MASK) ? 1 : 0;
}

void full_port_tsn_set_rtc_offset(uint32_t u32Ns, uint32_t u32SSl, uint32_t u32SSh, uint32_t u32Ch)
{
    BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_NS = u32Ns;
    BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_SL = u32SSl;
    BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_SH = u32SSh;
    BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_CH = u32Ch;   
}

void full_port_tsn_set_bin_txdata(uint8_t u8Port, uint8_t u8Index, uint32_t* pu32TxData, uint8_t u8Len)
{
    uint32_t *pu32 = pu32TxData;

    for(int i=0; i<u8Len/4; i++)
    {
        BOARD_TSN->TSNPORT[u8Port].BIN[u8Index].TXDATA[i] = *pu32++;
    }
    BOARD_TSN->TSNPORT[u8Port].BIN[u8Index].TSYN_TXBUF_TQUE_AND_TX_LEN = u8Len;
}

void full_port_tsn_init(void)
{

    //uint32_t u32TsnFreq = clock_get_frequency(clock_tsn1);
    //FULL_PORT_LOG_INFO("u32TsnFreq:%dMhz\r\n",u32TsnFreq);
    full_port_tsn_pin_init();
    FULL_PORT_LOG_INFO("PORT1 TSN_EP_IPCFG 0x%8x\r\n", BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT1].TSN_EP_IPCFG);
    FULL_PORT_LOG_INFO("PORT2 TSN_EP_IPCFG 0x%8x\r\n", BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT2].TSN_EP_IPCFG);
    FULL_PORT_LOG_INFO("PORT3 TSN_EP_IPCFG 0x%8x\r\n", BOARD_TSN->TSNPORT[TSW_TSNPORT_PORT3].TSN_EP_IPCFG);

    /* reset p1 p2 p3 */
    board_tsw_phy_set(TSW_TSNPORT_PORT1, false);
    board_tsw_phy_set(TSW_TSNPORT_PORT2, false);
    board_tsw_phy_set(TSW_TSNPORT_PORT3, false);

    /* Clear CAM */
    tsw_clear_cam(BOARD_TSN);

    /* Wait for cam clearing completion */
    board_delay_ms(1000);

    /* Enable VLAN-ID 1 at all ports */
    tsw_set_cam_vlan_port(BOARD_TSN);
    
    /*PORT1/2/3 GPR设置*/
    full_port_tsn_general_config(TSW_TSNPORT_PORT1, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
    full_port_tsn_general_config(TSW_TSNPORT_PORT2, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
    full_port_tsn_general_config(TSW_TSNPORT_PORT3, tsw_port_speed_1000mbps, tsw_port_phy_itf_rgmii, 0x8, 0);
    
    /* PORT1/2 MAC 设置 */
    uint8_t u8Port1Mac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    uint8_t u8Port2Mac[6] = {0x98, 0x2c, 0xbc, 0xb1, 0x9f, 0x17};
    uint8_t u8Port3Mac[6] = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
    full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT1, u8Port1Mac, tsw_mac_mode_gmii, TSW_RXFIFO_E1);
    full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT2, u8Port2Mac, tsw_mac_mode_gmii, TSW_RXFIFO_E1);
    full_port_tsn_ep_mac_config(TSW_TSNPORT_PORT3, u8Port3Mac, tsw_mac_mode_gmii, TSW_RXFIFO_E1);

}

void full_port_tsn_as_init(uint8_t flag)
{
    uint32_t *pu32;
    pu32 = (uint32_t*)&stPTPSyncForTX;
    for(uint32_t i=0; i<sizeof(stPTPSyncForTX)/4; i++)
    {
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TXDATA[i] = *pu32++;
    }
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TQUE_AND_TX_LEN = sizeof(stPTPSyncForTX);
   
    pu32 = (uint32_t*)&stPTPFollowUpForTX;
    for(uint32_t i=0; i<sizeof(stPTPFollowUpForTX)/4; i++)
    {
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[1].TXDATA[i] = *pu32++;
    }
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[1].TSYN_TXBUF_TQUE_AND_TX_LEN = sizeof(stPTPFollowUpForTX);

    pu32 = (uint32_t*)&stPTPDelayReqForTX;
    for(uint32_t i=0; i<sizeof(stPTPDelayReqForTX)/4; i++)
    {
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[2].TXDATA[i] = *pu32++;
    }
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[2].TSYN_TXBUF_TQUE_AND_TX_LEN = sizeof(stPTPDelayReqForTX);

    pu32 = (uint32_t*)&stPTPDelayRespForTX;
    for(uint32_t i=0; i<sizeof(stPTPDelayRespForTX)/4; i++)
    {
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[3].TXDATA[i] = *pu32++;
    }
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[3].TSYN_TXBUF_TQUE_AND_TX_LEN = sizeof(stPTPDelayRespForTX);    

    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_TIMER_INCR = 10 << 24;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CR = 0x4;

    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_HCLKDIV = 100000;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[0] = 1000;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[1] = 100;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[2] = 1000;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[3] = 1000;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[4] = 1000;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR = 0x1f00;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR = 0x1f00;
    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR = 0;
    if(flag) {
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR = 0x1<<16 | 0x1<<8 | 0x1<<2 | 0x1<<1;
    } else {
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR = 0x1<<2 | 0x1<<1;
    }

    //FULL_PORT_LOG_INFO("sr 0x%x\r\n",HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR);
    //uint32_t t = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_RXBUF_RX_TIME_STAMP_H;
    //FULL_PORT_LOG_INFO("%d\r\n",t);
    //t = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_RXBUF_RX_TIME_STAMP_L;
    //FULL_PORT_LOG_INFO("%d\r\n",t);

    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_TSN1_IRQ, FULL_PORT_IRQ_TSN1_PRIORITY);
}

void full_port_tsn_tas_init(void)
{
    tsw_tas_controllist_entry_t entry[8];
    tsw_tas_config_t config;

    entry[TSW_SHACL_ENT0].interval = 100000000; /* 100ms */
    entry[TSW_SHACL_ENT0].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT0].state = tsw_shap_tas_aclist_state_open_queueu_0;

    entry[TSW_SHACL_ENT1].interval = 100000000;  /* 100ms */
    entry[TSW_SHACL_ENT1].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT1].state = tsw_shap_tas_aclist_state_open_queueu_1;

    entry[TSW_SHACL_ENT2].interval = 300000000;  /* 100ms */
    entry[TSW_SHACL_ENT2].op = tsw_shap_tas_aclist_op_set_and_release_mac;
    entry[TSW_SHACL_ENT2].state = tsw_shap_tas_aclist_state_open_queueu_2;

    entry[TSW_SHACL_ENT3].interval = 100000000; /* 100ms */
    entry[TSW_SHACL_ENT3].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT3].state = tsw_shap_tas_aclist_state_open_queueu_3;

    entry[TSW_SHACL_ENT4].interval = 100000000;  /* 100ms */
    entry[TSW_SHACL_ENT4].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT4].state = tsw_shap_tas_aclist_state_open_queueu_4;

    entry[TSW_SHACL_ENT5].interval = 100000000;  /* 100ms */
    entry[TSW_SHACL_ENT5].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT5].state = tsw_shap_tas_aclist_state_open_queueu_5;

    entry[TSW_SHACL_ENT6].interval = 100000000; /* 100ms */
    entry[TSW_SHACL_ENT6].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT6].state = tsw_shap_tas_aclist_state_open_queueu_6;

    entry[TSW_SHACL_ENT7].interval = 100000000;  /* 100ms */
    entry[TSW_SHACL_ENT7].op = tsw_shap_tas_aclist_op_set_gate_states;
    entry[TSW_SHACL_ENT7].state = tsw_shap_tas_aclist_state_open_queueu_7;

    config.entry = entry;
    config.entry_count = 8;
    config.cycle_time = 1000000000; /* 1000ms */

    config.base_time_sec = 10;
    config.base_time_ns = 0;

    tsw_set_lookup_table(HPM_TSW, 0, tsw_dst_port_2, 0x0892041eb962);

    tsw_shap_disable_tas(HPM_TSW, TSW_TSNPORT_PORT2);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_0, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_1, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_2, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_3, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_4, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_5, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_6, 60);
    tsw_shap_set_tas_max_sdu_ticks(HPM_TSW, TSW_TSNPORT_PORT2, tsw_traffic_queue_7, 60);
    tsw_shap_set_tas(HPM_TSW, TSW_TSNPORT_PORT2, &config);

    tsw_shap_enable_tas(HPM_TSW, TSW_TSNPORT_PORT2);
}

static full_port_irq_cb_func s_full_port_irq_tsn0_cb;
static full_port_irq_cb_func s_full_port_irq_tsn1_cb;

void full_port_irq_tsn0_isr(void)
{
    s_full_port_irq_tsn0_cb();
}

void full_port_irq_tsn1_isr(void)
{
    s_full_port_irq_tsn1_cb();
}

SDK_DECLARE_EXT_ISR_M(FULL_PORT_IRQ_TSN0_IRQ, full_port_irq_tsn0_isr);
SDK_DECLARE_EXT_ISR_M(FULL_PORT_IRQ_TSN1_IRQ, isr_tsn_1);

void full_port_irq_tsn0_init(full_port_irq_cb_func cb)
{
    s_full_port_irq_tsn0_cb = cb;
    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_TSN0_IRQ, FULL_PORT_IRQ_TSN0_PRIORITY);
    full_port_tsn_enable_recv_dma_irq(true);
}

void full_port_irq_tsn1_init(full_port_irq_cb_func cb)
{
    s_full_port_irq_tsn1_cb = cb;
    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_TSN1_IRQ, FULL_PORT_IRQ_TSN1_PRIORITY);
}


#if MASTER
void isr_tsn_1(void)
{

    /* tsyn timer */
    if ((HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR & 0x100) && (HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR & 0x100)) {  //sync
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR |= 0x100;
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_TX_TRIG |= 0x1;
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[1] = 10;
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR |= 0x2<<16 | 0x2<<8;
        FULL_PORT_LOG_INFO("tsyn 0x%x\r\n",HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR);
    }
    if ((HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR & 0x200) && (HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR & 0x200)) {  //follow up
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR |= 0x200;   
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR &= ~(0x2<<16);
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR &= ~(0x2<<8);
        stPTPFollowUpForTX.timestamp[0] = 0;
        stPTPFollowUpForTX.timestamp[1] = 0;
        stPTPFollowUpForTX.timestamp[2] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_H >> 24);
        stPTPFollowUpForTX.timestamp[3] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_H >> 16);
        stPTPFollowUpForTX.timestamp[4] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_H >> 8);
        stPTPFollowUpForTX.timestamp[5] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_H >> 0);
        stPTPFollowUpForTX.timestamp[6] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_L >> 24);
        stPTPFollowUpForTX.timestamp[7] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_L >> 16);
        stPTPFollowUpForTX.timestamp[8] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_L >> 8);
        stPTPFollowUpForTX.timestamp[9] = (uint8_t)(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_L >> 0);
        FULL_PORT_LOG_INFO("follow up 0x%x %d.%d\r\n",HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR, HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_H, 
                                                  HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[0].TSYN_TXBUF_TX_TIMESTAMP_L);
        uint32_t *pu32 = (uint32_t*)&stPTPFollowUpForTX;
        for(int i=0; i<sizeof(stPTPFollowUpForTX)/4; i++)
        {
            HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[1].TXDATA[i] = *pu32++;
        }
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[1].TSYN_TXBUF_TQUE_AND_TX_LEN = sizeof(stPTPFollowUpForTX);
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_TX_TRIG |= 0x2;
    }

        static uint32_t s_u32RxTimerStampHForReq, s_u32RxTimerStampLForReq;
        if(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR & 0x2) { //rx irq
            if(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS & 0x80000000) {  //OV
                FULL_PORT_LOG_INFO("ptp rx ov\r\n");
                HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS |= 0x80000000;
                        return;
            }
            uint32_t u32RxState = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS;
            printf("rx sts %x\r\n",u32RxState);
            if( u32RxState & 0x40000000) {  //AV/NXT
                for(int i=0; i<30; i++)
                {
                    au32RxPtp[i] = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RXDATA[i];
                }          
                if(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[0] != 0x01 || 
                ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[1] != 0x80 ||
                ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[2] != 0xC2 ||
                ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[3] != 0x00 ||
                ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[4] != 0x00 ||
                ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[5] != 0x0E )
                {return;}
                if(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->stPtpProtocolHead.offset0_bm.messageId == 1) {
                    s_u32RxTimerStampHForReq = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_RXBUF_RX_TIME_STAMP_H;
                    s_u32RxTimerStampLForReq = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_RXBUF_RX_TIME_STAMP_L;
                    FULL_PORT_LOG_INFO("req %d.%d\r\n", s_u32RxTimerStampHForReq,s_u32RxTimerStampLForReq);
                    stPTPDelayRespForTX.timestamp[0] = 0;
                    stPTPDelayRespForTX.timestamp[1] = 0;
                    stPTPDelayRespForTX.timestamp[2] = (uint8_t)(s_u32RxTimerStampHForReq >> 24);
                    stPTPDelayRespForTX.timestamp[3] = (uint8_t)(s_u32RxTimerStampHForReq >> 16);
                    stPTPDelayRespForTX.timestamp[4] = (uint8_t)(s_u32RxTimerStampHForReq >> 8);
                    stPTPDelayRespForTX.timestamp[5] = (uint8_t)(s_u32RxTimerStampHForReq >> 0);
                    stPTPDelayRespForTX.timestamp[6] = (uint8_t)(s_u32RxTimerStampLForReq >> 24);
                    stPTPDelayRespForTX.timestamp[7] = (uint8_t)(s_u32RxTimerStampLForReq >> 16);
                    stPTPDelayRespForTX.timestamp[8] = (uint8_t)(s_u32RxTimerStampLForReq >> 8);
                    stPTPDelayRespForTX.timestamp[9] = (uint8_t)(s_u32RxTimerStampLForReq >> 0);
                    uint32_t *pu32 = (uint32_t*)&stPTPDelayRespForTX;
                    for(int i=0; i<sizeof(stPTPDelayRespForTX)/4; i++)
                    {
                        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[3].TXDATA[i] = *pu32++;
                    }
                    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[3].TSYN_TXBUF_TQUE_AND_TX_LEN = sizeof(stPTPDelayRespForTX);
                    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_TX_TRIG |= 0x8;
                    HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_NS = 0;
                    uint64_t sec = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_SEC;
                    uint64_t ns = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_NS;
                    FULL_PORT_LOG_INFO("time %llu.%llu\r\n",sec,ns);
                }
                HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS |= 0x40000000;
            }
        }

}
#endif

#if SLAVER
void isr_tsn_1(void)
{
    /* tsyn timer */
    if ((HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR & 0x400) && (HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR & 0x400)) {  //req
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR |= 0x400;
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR &= ~(0x4<<16);
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR &= ~(0x4<<8);
        HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_TX_TRIG |= 0x4;
        FULL_PORT_LOG_INFO("req 0x%x\r\n",HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR);
    }

    static uint32_t s_u32RxTimerStampHForSync, s_u32RxTimerStampLForSync;
    static uint32_t s_u32RxTimerStampHForFollowUp, s_u32RxTimerStampLForFollowUp;
    static uint32_t s_u32RxTimerStampHForReq, s_u32RxTimerStampLForReq;
    static uint32_t s_u32RxTimerStampHForResp, s_u32RxTimerStampLForResp;
    if(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_SR & 0x2) { //rx irq
        if(HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS & 0x80000000) {  //OV
            FULL_PORT_LOG_INFO("ptp rx ov\r\n");
            HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS |= 0x80000000;
                    return;
        }
        uint32_t u32RxState = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS;
        if( u32RxState & 0x40000000) {  //AV/NXT
            for(int i=0; i<30; i++)
            {
                au32RxPtp[i] = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RXDATA[i];
            }          
            if(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[0] != 0x01 || 
               ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[1] != 0x80 ||
               ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[2] != 0xC2 ||
               ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[3] != 0x00 ||
               ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[4] != 0x00 ||
               ((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->des_mac[5] != 0x0E )
               {return;}
            if(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->stPtpProtocolHead.offset0_bm.messageId == 0) {
                s_u32RxTimerStampHForSync = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_RXBUF_RX_TIME_STAMP_H;
                s_u32RxTimerStampLForSync = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_RXBUF_RX_TIME_STAMP_L;
                FULL_PORT_LOG_INFO("sync %d.%d\r\n", s_u32RxTimerStampHForSync,s_u32RxTimerStampLForSync);
            } else if(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->stPtpProtocolHead.offset0_bm.messageId == 8) {
                HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYNTMR[2] = 10;
                HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_CR |= 0x4<<16 | 0x4<<8;                
                s_u32RxTimerStampHForFollowUp = ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[2])) << 24 |
                                                ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[3])) << 16 |
                                                ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[4])) << 8  |
                                                ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[5])) << 0  ;
                s_u32RxTimerStampLForFollowUp = ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[6])) << 24 |
                                                ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[7])) << 16 |
                                                ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[8])) << 8  |
                                                ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[9])) << 0  ;
                FULL_PORT_LOG_INFO("follow up %d.%d\r\n", s_u32RxTimerStampHForFollowUp,s_u32RxTimerStampLForFollowUp);
            } else if(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->stPtpProtocolHead.offset0_bm.messageId == 9) {
                HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_NS = 0;
                uint64_t curtime_s = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_SEC;
                uint64_t curtime_ns = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_NS;
                s_u32RxTimerStampHForReq = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[2].TSYN_TXBUF_TX_TIMESTAMP_H;
                s_u32RxTimerStampLForReq = HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].BIN[2].TSYN_TXBUF_TX_TIMESTAMP_L;
                s_u32RxTimerStampHForResp = ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[2])) << 24 |
                                            ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[3])) << 16 |
                                            ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[4])) << 8  |
                                            ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[5])) << 0  ;
                s_u32RxTimerStampLForResp = ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[6])) << 24 |
                                            ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[7])) << 16 |
                                            ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[8])) << 8  |
                                            ((uint32_t)(((FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st*)au32RxPtp)->timestamp[9])) << 0  ;
                FULL_PORT_LOG_INFO("resp %d.%d\r\n", s_u32RxTimerStampHForResp,s_u32RxTimerStampLForResp);
                FULL_PORT_LOG_INFO("T2 - T1 %d.%d\r\n", (s_u32RxTimerStampHForSync - s_u32RxTimerStampHForFollowUp), (s_u32RxTimerStampLForSync - s_u32RxTimerStampLForFollowUp));
                FULL_PORT_LOG_INFO("T3 - T4 %d.%d\r\n", (s_u32RxTimerStampHForReq - s_u32RxTimerStampHForResp), (s_u32RxTimerStampLForReq - s_u32RxTimerStampLForResp));
                uint64_t t1 = (uint64_t)s_u32RxTimerStampHForFollowUp * 1000000000 + (uint64_t)s_u32RxTimerStampLForFollowUp;
                uint64_t t2 = (uint64_t)s_u32RxTimerStampHForSync * 1000000000 + (uint64_t)s_u32RxTimerStampLForSync;
                uint64_t t3 = (uint64_t)s_u32RxTimerStampHForReq * 1000000000 + (uint64_t)s_u32RxTimerStampLForReq;
                uint64_t t4 = (uint64_t)s_u32RxTimerStampHForResp * 1000000000 + (uint64_t)s_u32RxTimerStampLForResp;
                FULL_PORT_LOG_INFO("t2 - t1 %llu\r\n",t2-t1);
                FULL_PORT_LOG_INFO("t3 - t4 %llu\r\n",t3-t4);
                uint64_t delay = (t4-t3+t2-t1)/2;
                uint64_t offset = (t2-t1-t4+t3)/2;
                FULL_PORT_LOG_WARNING("delay: (t4-t3+t2-t1)/2 %llu ns\r\n", delay);
                FULL_PORT_LOG_INFO("offset: (t2-t1-t4+t3)/2 %llu ns\r\n", offset);
                FULL_PORT_LOG_INFO("before set time %llu.%llu\r\n", curtime_s, curtime_ns);
                FULL_PORT_LOG_WARNING("after set time %llu.%llu\r\n",curtime_s-(offset/1000000000), curtime_ns-(offset%1000000000)-delay);

                // FULL_PORT_LOG_INFO("base time %llu.%llu\r\n", HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSN_SHAPER_TAS_ABASETM_H, HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSN_SHAPER_TAS_ABASETM_L);
                // FULL_PORT_LOG_INFO("opbase time %llu.%llu\r\n", HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSN_SHAPER_TAS_OBASETM_H, HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSN_SHAPER_TAS_OBASETM_L);

                
            } else {}
            HPM_TSW->TSNPORT[TSW_TSNPORT_PORT1].TSYN_PTP_RX_STS |= 0x40000000;
        }
    }
}
#endif
