/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_can.h"

static uint32_t rx_msg_cnt = 0;

static void gw_can_rx_handle(gw_mq_msg mq_msg)
{
    (void)mq_msg;
}

static void gw_can_tx_handle(gw_mq_msg msg)
{
    mcan_tx_frame_t tx_frame;
    static uint32_t id = 0;

    switch (msg.submsg_id)
    {
        case GW_MQ_SUBMSG_CAN_TX_STD_RXNUM_ID:
            tx_frame.std_id = id;
            tx_frame.data_8[0] = 0x00;
            tx_frame.data_8[1] = 0x00;
            tx_frame.data_8[2] = 0x00;
            tx_frame.data_8[3] = (rx_msg_cnt/10000)%10;
            tx_frame.data_8[4] = (rx_msg_cnt/1000)%10;
            tx_frame.data_8[5] = (rx_msg_cnt/100)%10;
            tx_frame.data_8[6] = (rx_msg_cnt/10)%10;
            tx_frame.data_8[7] = rx_msg_cnt%10;
            tx_frame.dlc = 8;
            drv_mcan_send_std(0, tx_frame.dlc, tx_frame.std_id, tx_frame.data_8);
            id++;
            if(id > 0x7FF)
            {
                id = 0;
            }
            LOG_I("gw_can_tx_handle: std_id = 0x%03X, data = 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                     tx_frame.std_id, tx_frame.data_8[0], tx_frame.data_8[1], tx_frame.data_8[2],
                     tx_frame.data_8[3], tx_frame.data_8[4], tx_frame.data_8[5], tx_frame.data_8[6], tx_frame.data_8[7]);
        break;
        case GW_MQ_SUBMSG_CAN_TX_EXT_RXNUM_ID:
            tx_frame.ext_id = id;
            tx_frame.data_8[0] = 0x00;
            tx_frame.data_8[1] = 0x00;
            tx_frame.data_8[2] = 0x00;
            tx_frame.data_8[3] = (rx_msg_cnt/10000)%10;
            tx_frame.data_8[4] = (rx_msg_cnt/1000)%10;
            tx_frame.data_8[5] = (rx_msg_cnt/100)%10;
            tx_frame.data_8[6] = (rx_msg_cnt/10)%10;
            tx_frame.data_8[7] = rx_msg_cnt%10;
            tx_frame.dlc = 8;
            drv_mcan_send_std(0, tx_frame.dlc, tx_frame.ext_id, tx_frame.data_8);
            id++;
            if(id > 0x1FFFFFFF)
            {
                id = 0;
            }
        break;
        default:
        break;
    }

}


static void gw_can_rx_cb(volatile mcan_rx_message_t *rx_msg)
{
    static mcan_rx_message_t mcan_rx_message;
    
    memcpy(&mcan_rx_message, (char*)rx_msg, sizeof(mcan_rx_message_t));
    gw_mq_msg mq_msg;
    mq_msg.msg_id = GW_MQ_MSG_MISC_LED_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_CAN_RX_ID;
    mq_msg.param = 100;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
    rx_msg_cnt++;
    if(rx_msg_cnt >= 65536)
    {
        rx_msg_cnt = 0;
    }

    mq_msg.msg_id = GW_MQ_MSG_MISC_SEGMENT_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_SEGMENT_GE_ID;
    mq_msg.param = rx_msg_cnt%10;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);

    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_SEGMENT_SHI_ID;
    mq_msg.param = (rx_msg_cnt/10)%10;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);

    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_SEGMENT_BAI_ID;
    mq_msg.param = (rx_msg_cnt/100)%10;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);

    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_SEGMENT_QIAN_ID;
    mq_msg.param = (rx_msg_cnt/1000)%10;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);

    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_SEGMENT_WAN_ID;
    mq_msg.param = (rx_msg_cnt/10000)%10;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
    
}

static void gw_can_tx_cb(void)
{
    gw_mq_msg mq_msg;
    mq_msg.msg_id = GW_MQ_MSG_MISC_LED_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_CAN_TX_ID;
    mq_msg.param = 500;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
}

void gw_can_thread(void *argument)
{
    (void)argument;
    osStatus_t ret;
    gw_mq_msg mq_msg;

    drv_mcan_init(1000000, gw_can_rx_cb, gw_can_tx_cb);

    LOG_D("gw_can_thread run\r\n");

    for(;;)
    {
        ret = gw_mq_recv(GW_MQ_CAN_ID, &mq_msg, osWaitForever);

        if(ret < 0) {
            continue;
            LOG_E("%s mq recv fail!\r\n",__func__);
        }

        switch (mq_msg.msg_id)
        {
            case GW_MQ_MSG_CAN_RX_ID:
                gw_can_rx_handle(mq_msg);
                break;
            case GW_MQ_MSG_CAN_TX_ID:
                gw_can_tx_handle(mq_msg);
                break;
            case GW_MQ_MSG_CAN_CONFIG_ID:
                drv_mcan_init(mq_msg.param, gw_can_rx_cb, gw_can_tx_cb);
                LOG_W("Baudrate %d", mq_msg.param);
                break;
            default:
                break;
        }
    }
}
