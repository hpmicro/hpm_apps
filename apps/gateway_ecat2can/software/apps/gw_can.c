/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_can.h"
#include "ecatslv.h"

static void gw_can_rx_handle(gw_mq_msg mq_msg)
{
    switch (mq_msg.submsg_id)
    {
        case GW_MQ_SUBMSG_CAN_RX_STD_ID:
        break;
        default:
        break;
    }
}

static void gw_can_tx_handle(gw_mq_msg msg)
{
    gw_mq_msg mq_msg;

    uint8_t num = 0;
    uint8_t *user_data = NULL;

    switch (msg.submsg_id)
    {
        case GW_MQ_SUBMSG_CAN_TX_STD_TRIG_ID:
            can_std_cyc = 0;
            can_ext_cyc = 0;
            if(nAlStatus == STATE_OP) {
                user_data = (uint8_t*)gateway_apps_get_rxpdo_std(&num);   
                for(int i=0; i<num; i++) 
                {
                    gw_can_tx_std can_std;
                    memcpy(&can_std, user_data + 8 + i*sizeof(gw_can_tx_std), sizeof(gw_can_tx_std));
                    if(can_std.dlc <= 0) {
                        continue;
                    }
                    LOG_D("std tx rtr dlc id:%d, %d, %d\r\n", can_std.rtr, can_std.dlc, can_std.std_id);
                    drv_mcan_send_std(can_std.rtr, can_std.dlc, can_std.std_id, can_std.data);           
                    uint8_t __num = 0;
                    uint8_t *__user_data = (uint8_t*)gateway_apps_get_txpdo_std(&__num);
                    if(__user_data != NULL && __num != 0) {
                        *((uint16_t*)__user_data+1) += 1;
                        *((uint16_t*)__user_data+4) = can_std.tx_num;
                    }                     
                }
            }
        break;
        case GW_MQ_SUBMSG_CAN_TX_STD_CYC_ID:
            can_ext_cyc = 0;
            can_std_cyc = msg.param;
            can_std_cyc_cnt = 0;
            LOG_D("std tx cyc id:%d, %d, %d\r\n", can_std_cyc, can_std_cyc_cnt, can_std_cyc);
        break;
        case GW_MQ_SUBMSG_CAN_TX_EXT_TRIG_ID:
            can_std_cyc = 0;
            can_ext_cyc = 0;
            if(nAlStatus == STATE_OP) {
                user_data = (uint8_t*)gateway_apps_get_rxpdo_ext(&num);
                for(int i=0; i<num; i++) 
                {
                    gw_can_tx_ext can_ext;
                    memcpy(&can_ext, user_data + 8 + i*sizeof(gw_can_tx_ext), sizeof(gw_can_tx_ext));
                    if(can_ext.dlc <= 0) {
                        continue;
                    }
                    LOG_D("ext tx rtr dlc id:%d, %d, %d\r\n", can_ext.rtr, can_ext.dlc, can_ext.ext_id);
                    drv_mcan_send_ext(can_ext.rtr, can_ext.dlc, can_ext.ext_id, can_ext.data);  
                    uint8_t __num = 0;
                    uint8_t *__user_data = (uint8_t*)gateway_apps_get_txpdo_ext(&__num);
                    if(__user_data != NULL && __num != 0) {
                        *((uint16_t*)__user_data+1) += 1;
                        *((uint16_t*)__user_data+4) = can_ext.tx_num;
                    }
                }
            }
        break;
        case GW_MQ_SUBMSG_CAN_TX_EXT_CYC_ID:
            can_std_cyc = 0;
            can_ext_cyc = msg.param;
            can_ext_cyc_cnt = 0;    
        break;
        case GW_MQ_SUBMSG_CAN_TX_STOP_CYC_ID:
            can_std_cyc = 0;
            can_ext_cyc = 0;
            can_ext_cyc_cnt = 0; 
            can_std_cyc_cnt = 0; 
        break;
        default:
        break;
    }
}


static void gw_can_rx_cb(volatile mcan_rx_message_t *rx_msg)
{
    static mcan_rx_message_t mcan_rx_message;
    
    memcpy(&mcan_rx_message, rx_msg, sizeof(mcan_rx_message_t));

    if(mcan_rx_message.use_ext_id) {
        static uint8_t index_ext = 0;
        uint8_t num = 0;
        gw_can_rx_ext can_ext = {0};
        can_ext.dlc = mcan_rx_message.dlc;
        can_ext.rtr = mcan_rx_message.rtr;
        can_ext.ext_id = mcan_rx_message.ext_id;
        can_ext.std_ext_bit = mcan_rx_message.use_ext_id;
        can_ext.res = 0;
        for(int i=0; i<can_ext.dlc; i++)
        {
          can_ext.data[i] = mcan_rx_message.data_8[i];
        }
        LOG_D("ext rx rtr dlc id std_ext_bit:%d, %d, %d\r\n", can_ext.rtr, can_ext.dlc, can_ext.ext_id, can_ext.std_ext_bit);
        uint8_t *user_data = (uint8_t*)gateway_apps_get_txpdo_ext(&num);
        if(user_data == NULL || num == 0) {
           return;
        }
        memcpy(user_data + 10 + sizeof(gw_can_rx_ext)*(index_ext++), &can_ext, sizeof(gw_can_rx_ext));
        *((uint16_t*)user_data+2) += 1;
        *((uint16_t*)user_data+3) += 1;
        if(index_ext >= num) index_ext = 0;

    } else {
        static uint8_t index_std = 0;
        uint8_t num = 0;
        gw_can_rx_std can_std = {0};
        can_std.dlc = mcan_rx_message.dlc;
        can_std.rtr = mcan_rx_message.rtr;
        can_std.std_id = mcan_rx_message.std_id;
        for(int i=0; i<can_std.dlc; i++)
        {
          can_std.data[i] = mcan_rx_message.data_8[i];
        }
        LOG_D("std rx rtr dlc id:%d, %d, %d\r\n", can_std.rtr, can_std.dlc, can_std.std_id);
        uint8_t *user_data = (uint8_t*)gateway_apps_get_txpdo_std(&num);
        if(user_data == NULL || num == 0) {
           return;
        }
        memcpy(user_data + 10 + sizeof(gw_can_rx_std)*(index_std++), &can_std, sizeof(gw_can_rx_std));
        *((uint16_t*)user_data+2) += 1;
        *((uint16_t*)user_data+3) += 1;
        if(index_std >= num) index_std = 0;        

    }

    gw_mq_msg mq_msg;
    mq_msg.msg_id = GW_MQ_MSG_MISC_LED_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_CAN_STATE_RX_OK_ID;
    gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
}

static void gw_can_tx_cb(void)
{
    gw_mq_msg mq_msg;
    mq_msg.msg_id = GW_MQ_MSG_MISC_LED_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_CAN_STATE_TX_OK_ID;
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
