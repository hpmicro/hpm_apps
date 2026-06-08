/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_misc.h"
#include "ecatslv.h"

gw_misc_led_dev_status_e led_dev_status = GW_MISC_LED_DEV_STATUS_NULL;

volatile uint32_t can_std_cyc = 0;
volatile uint32_t can_ext_cyc = 0;
volatile uint32_t can_std_cyc_cnt = 0;
volatile uint32_t can_ext_cyc_cnt = 0;

void gw_misc_timer_cb(void)
{
    gw_mq_msg mq_msg;

    static uint16_t timeout_1ms = 0;
    static uint16_t timeout_10ms = 0;
    static uint16_t timeout_100ms = 0;
    static uint16_t timeout_1000ms = 0;

    timeout_1ms++;
	timeout_10ms++;
	timeout_100ms++;
	timeout_1000ms++;


	if(timeout_1ms>=1) {
		timeout_1ms = 0;
        mq_msg.msg_id = GW_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_TIMER_1MS_ID;
        gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
	}
	if(timeout_10ms>=10) {
		timeout_10ms = 0;
        mq_msg.msg_id = GW_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_TIMER_10MS_ID;
        gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
	}
	if(timeout_100ms>=100) {
		timeout_100ms = 0;
        mq_msg.msg_id = GW_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_TIMER_100MS_ID;
        gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
	}
	if(timeout_1000ms>=1000) {
		timeout_1000ms = 0;
        mq_msg.msg_id = GW_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_TIMER_1000MS_ID;
        gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
	}
}

void gw_misc_timer_handle(gw_mq_msg msg)
{
    gw_mq_msg mq_msg;

	switch (msg.submsg_id)
	{
        case GW_MQ_SUBMSG_MISC_TIMER_1MS_ID:
            if(nAlStatus == STATE_OP) {
                can_std_cyc_cnt++;
                can_ext_cyc_cnt++; 
                if(can_std_cyc_cnt >= can_std_cyc && can_std_cyc != 0) {
                    can_std_cyc_cnt = 0;
                    uint8_t num = 0;
                    uint8_t *user_data = (uint8_t*)gateway_apps_get_rxpdo_std(&num);
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
                if(can_ext_cyc_cnt >= can_ext_cyc && can_ext_cyc != 0) {
                    can_ext_cyc_cnt = 0;
                    uint8_t num = 0;
                    uint8_t *user_data = (uint8_t*)gateway_apps_get_rxpdo_ext(&num);
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
            }
            break;
		case GW_MQ_SUBMSG_MISC_TIMER_10MS_ID:
			break;
		case GW_MQ_SUBMSG_MISC_TIMER_100MS_ID:
            if(led_dev_status == GW_MISC_LED_DEV_STATUS_R_BLINKY) {
                drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_TOGGLE);
                drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_OFF);
                drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_OFF);
            } else if(led_dev_status == GW_MISC_LED_DEV_STATUS_G_BLINKY) {
                drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_OFF);
                drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_TOGGLE);
                drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_OFF);
            } else if(led_dev_status == GW_MISC_LED_DEV_STATUS_B_BLINKY) {
                drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_OFF);
                drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_OFF);
                drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_TOGGLE);
            }
			break;
		case GW_MQ_SUBMSG_MISC_TIMER_1000MS_ID:
			break;
		default:
			break;
	}
}

static void gw_misc_led_handle(gw_mq_msg msg)
{
    switch (msg.submsg_id)
    {
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_OP_ID:
            led_dev_status = GW_MISC_LED_DEV_STATUS_NULL;
            drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_ON);
            drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_OFF);
            break;
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_FAIL_ID:
            led_dev_status = GW_MISC_LED_DEV_STATUS_NULL;
            drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_ON);
            drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_OFF);
            break;
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_INIT_ID:
            led_dev_status = GW_MISC_LED_DEV_STATUS_R_BLINKY;
            break;
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_BOOTSTRAP_ID:
            led_dev_status = GW_MISC_LED_DEV_STATUS_NULL;
            drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_ON);
            break;
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_PRE_OP_ID:
            led_dev_status = GW_MISC_LED_DEV_STATUS_B_BLINKY;
            break;
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_SAFE_OP_ID:
            led_dev_status = GW_MISC_LED_DEV_STATUS_G_BLINKY;
            break;
        case GW_MQ_SUBMSG_MISC_LED_CAN_STATE_RX_OK_ID:
            drv_led_ctrl(DRV_LED1_R_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED1_G_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED1_B_ID, DRV_LED_CTRL_ON);
            break;
        case GW_MQ_SUBMSG_MISC_LED_CAN_STATE_TX_OK_ID:
            drv_led_ctrl(DRV_LED1_R_ID, DRV_LED_CTRL_OFF);
            drv_led_ctrl(DRV_LED1_G_ID, DRV_LED_CTRL_ON);
            drv_led_ctrl(DRV_LED1_B_ID, DRV_LED_CTRL_OFF);
            break;
        default:
            break;
    }
    LOG_I("led submsg_id = %d\r\n", msg.submsg_id);
}

void gw_misc_thread(void *argument)
{
    (void)argument;
    osStatus_t ret;
    gw_mq_msg mq_msg;

    LOG_D("gw_misc_thread run\r\n");

    drv_led_init();
    board_timer_create(1, gw_misc_timer_cb);

    for(;;)
    {
        ret = gw_mq_recv(GW_MQ_MISC_ID, &mq_msg, osWaitForever);

        if(ret < 0) {
            continue;
            LOG_E("%s mq recv fail!\r\n",__func__);
        }

        switch (mq_msg.msg_id)
        {
            case GW_MQ_MSG_MISC_LED_ID:
                gw_misc_led_handle(mq_msg);
                break;
            case GW_MQ_MSG_MISC_TIMER_ID:
                gw_misc_timer_handle(mq_msg);
                break;
            default:
                break;
        }
    }
}
