/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_mcan.h"

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
#if defined(HPM_MCAN0)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan0_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN1)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan1_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN2)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan2_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN3)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan3_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN4)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan4_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN5)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan5_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN6)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan6_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#if defined(HPM_MCAN7)
ATTR_PLACE_AT(".ahb_sram") uint32_t mcan7_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
#endif

static drv_mcan_info s_can_info[] = {
#if defined(HPM_MCAN0)
    {
        .can_base = HPM_MCAN0,
        .irq_num = IRQn_MCAN0,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan0_msg_buf,
        .ram_size = sizeof(mcan0_msg_buf),
#endif
    },
#endif
#if defined(HPM_MCAN1)
    {
        .can_base = HPM_MCAN1,
        .irq_num = IRQn_MCAN1,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan1_msg_buf,
        .ram_size = sizeof(mcan1_msg_buf),
#endif
    },
#endif
#if defined(HPM_MCAN2)
    {
        .can_base = HPM_MCAN2,
        .irq_num = IRQn_MCAN2,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan2_msg_buf,
        .ram_size = sizeof(mcan2_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN3)
    {
        .can_base = HPM_MCAN3,
        .irq_num = IRQn_MCAN3,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan3_msg_buf,
        .ram_size = sizeof(mcan3_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN4)
    {
        .can_base = HPM_MCAN4,
        .irq_num = IRQn_MCAN4,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan4_msg_buf,
        .ram_size = sizeof(mcan4_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN5)
    {
        .can_base = HPM_MCAN5,
        .irq_num = IRQn_MCAN5,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan5_msg_buf,
        .ram_size = sizeof(mcan5_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN6)
    {
        .can_base = HPM_MCAN6,
        .irq_num = IRQn_MCAN6,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan6_msg_buf,
        .ram_size = sizeof(mcan6_msg_buf),
#endif
        },
#endif
#if defined (HPM_MCAN7)
    {
        .can_base = HPM_MCAN7,
        .irq_num = IRQn_MCAN7,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &mcan7_msg_buf,
        .ram_size = sizeof(mcan7_msg_buf),
#endif
        },
#endif
};

static volatile mcan_rx_message_t s_can_rx_buf;
static volatile mcan_tx_event_fifo_elem_t s_can_tx_evt;
static drv_mcan_rx_cb_t drv_mcan_rx_cb = NULL;
static drv_mcan_tx_cb_t drv_mcan_tx_cb = NULL;

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
static drv_mcan_info_t drv_mcan_find_info(void)
{
    MCAN_Type *base = BOARD_APP_CAN_BASE;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_can_info); i++) {
        if (s_can_info[i].can_base == base) {
            return &s_can_info[i];
        }
    }
    return NULL;
}
#endif

static void drv_mcan_isr(void)
{
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    drv_mcan_info *can_info = drv_mcan_find_info();
    assert(can_info != NULL);
#endif
    uint32_t flags = mcan_get_interrupt_flags(can_info->can_base);
    /* New message is available in RXFIFO0 */
    if ((flags & MCAN_INT_RXFIFO0_NEW_MSG) != 0) {
        mcan_read_rxfifo(can_info->can_base, 0, (mcan_rx_message_t *) &s_can_rx_buf);
        if(drv_mcan_rx_cb != NULL)
            drv_mcan_rx_cb(&s_can_rx_buf);
    }
    /* New message is available in RXFIFO1 */
    if ((flags & MCAN_INT_RXFIFO1_NEW_MSG) != 0U) {
        mcan_read_rxfifo(can_info->can_base, 1, (mcan_rx_message_t *) &s_can_rx_buf);
        if(drv_mcan_rx_cb != NULL)
            drv_mcan_rx_cb(&s_can_rx_buf);
    }
    /* New message is available in RXBUF */
    if ((flags & MCAN_INT_MSG_STORE_TO_RXBUF) != 0U) {
        /* NOTE: Below code is for demonstration purpose, the performance is not optimized
         *       Users should optimize the performance according to real use case.
         */
        for (uint32_t buf_index = 0; buf_index < MCAN_RXBUF_SIZE_CAN_DEFAULT; buf_index++) {
            if (mcan_is_rxbuf_data_available(can_info->can_base, buf_index)) {
                mcan_read_rxbuf(can_info->can_base, buf_index, (mcan_rx_message_t *) &s_can_rx_buf);
                mcan_clear_rxbuf_data_available_flag(can_info->can_base, buf_index);
                if(drv_mcan_rx_cb != NULL)
                    drv_mcan_rx_cb(&s_can_rx_buf);
            }
        }
    }
    /* New TX Event occurred */
    if ((flags & MCAN_INT_TX_EVT_FIFO_NEW_ENTRY) != 0) {
        mcan_read_tx_evt_fifo(can_info->can_base, (mcan_tx_event_fifo_elem_t *) &s_can_tx_evt);
        LOG_I("mcan tx evt\r\n");
    }
    /* Transmit completed */
    if ((flags & MCAN_EVENT_TRANSMIT) != 0U) {
        if(drv_mcan_tx_cb != NULL)
            drv_mcan_tx_cb();
    }
    /* Error happened */
    if ((flags & MCAN_EVENT_ERROR) != 0) {
        LOG_E("mcan error event occurred\r\n");
    }

    if ((flags & MCAN_INT_TIMEOUT_OCCURRED) != 0) {
        LOG_E("mcan timeout event occurred\r\n");
    }
    mcan_clear_interrupt_flags(can_info->can_base, flags);
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, drv_mcan_isr)


void drv_mcan_init(uint32_t baudrate, drv_mcan_rx_cb_t rx_cb, drv_mcan_tx_cb_t tx_cb)
{
    mcan_config_t can_config;
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    drv_mcan_info *can_info = drv_mcan_find_info();
    assert(can_info != NULL);
    mcan_msg_buf_attr_t attr = { can_info->ram_base, can_info->ram_size };
    status = mcan_set_msg_buf_attr(can_info->can_base, &attr);
    if (status != status_success) {
        LOG_E("Error was detected during setting message buffer attribute, please check the arguments\r\n");
        return;
    }
#endif
    mcan_get_default_config(can_info->can_base, &can_config);
    can_config.baudrate = baudrate; /* 500kbps */
    can_config.mode = mcan_mode_normal;
    board_init_can(can_info->can_base);
    can_info->clock_freq = board_init_can_clock(can_info->can_base);
    status = mcan_init(can_info->can_base, &can_config, can_info->clock_freq);
    LOG_I("CAN initialization success, clock freq: %d\r\n", can_info->clock_freq);
    if (status != status_success) {
        LOG_E("CAN initialization failed, error code: %d\r\n", status);
        return;
    }
    mcan_enable_interrupts(can_info->can_base, MCAN_EVENT_RECEIVE);
    mcan_enable_txbuf_transmission_interrupt(can_info->can_base, ~0U);

    drv_mcan_rx_cb = rx_cb;
    drv_mcan_tx_cb = tx_cb;

    intc_m_enable_irq_with_priority(BOARD_APP_CAN_IRQn, 9);
}

void drv_mcan_send_std(uint8_t rtr, uint8_t dlc, uint32_t id, uint8_t *data)
{
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    drv_mcan_info *can_info = drv_mcan_find_info();
    assert(can_info != NULL);
#endif

    mcan_tx_frame_t tx_buf = {0};
    tx_buf.rtr = rtr;
    tx_buf.dlc = dlc;
    tx_buf.std_id = id;
    for (uint32_t i = 0; i < tx_buf.dlc; i++) {
        tx_buf.data_8[i] = data[i];
    }
    uint32_t put_index = 0;
    status = mcan_transmit_via_txfifo_nonblocking(can_info->can_base, &tx_buf, &put_index);
    // status = mcan_transmit_blocking(can_info->can_base, &tx_buf);
    if (status != status_success) {
        LOG_E("can std sent message failed, error_code:%d\r\n", status);
        return;
    }
}

void drv_mcan_send_ext(uint8_t rtr, uint8_t dlc, uint32_t id, uint8_t *data)
{
    hpm_stat_t status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    drv_mcan_info *can_info = drv_mcan_find_info();
    assert(can_info != NULL);
#endif

    mcan_tx_frame_t tx_buf = {0};
    tx_buf.rtr = rtr;
    tx_buf.dlc = dlc;
    tx_buf.ext_id = id;
    tx_buf.use_ext_id = 1;
    for (uint32_t i = 0; i < tx_buf.dlc; i++) {
        tx_buf.data_8[i] = data[i];
    }
    uint32_t put_index = 0;
    status = mcan_transmit_via_txfifo_nonblocking(can_info->can_base, &tx_buf, &put_index);
    // status = mcan_transmit_blocking(can_info->can_base, &tx_buf);
    if (status != status_success) {
        LOG_E("can ext sent message failed, error_code:%d\r\n", status);
        return;
    }
}