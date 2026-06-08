/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_misc.h"

static int16_t gw_misc_led_flash_time_for_can_tx = 0;
static int16_t gw_misc_led_flash_time_for_can_rx = 0;

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

    gw_misc_led_flash_time_for_can_tx--;
    gw_misc_led_flash_time_for_can_rx--;
    if(gw_misc_led_flash_time_for_can_tx<0) gw_misc_led_flash_time_for_can_tx = -1;
    if(gw_misc_led_flash_time_for_can_rx<0) gw_misc_led_flash_time_for_can_rx = -1;
    if(gw_misc_led_flash_time_for_can_tx==0) {
        gw_misc_led_flash_time_for_can_tx = -1;
        drv_led_ctrl(DRV_LED0_ID, DRV_LED_CTRL_OFF);
    }
    if(gw_misc_led_flash_time_for_can_rx==0) {
        gw_misc_led_flash_time_for_can_rx = -1;
        drv_led_ctrl(DRV_LED0_ID, DRV_LED_CTRL_OFF);
    }


	if(timeout_1ms>=1) {
		timeout_1ms = 0;
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
	switch (msg.submsg_id)
	{
        case GW_MQ_SUBMSG_MISC_TIMER_1MS_ID:
            break;
		case GW_MQ_SUBMSG_MISC_TIMER_10MS_ID:
			break;
		case GW_MQ_SUBMSG_MISC_TIMER_100MS_ID:
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
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_CAN_TX_ID:
            gw_misc_led_flash_time_for_can_tx = msg.param;
            drv_led_ctrl(DRV_LED0_ID, DRV_LED_CTRL_ON);
            break;
        case GW_MQ_SUBMSG_MISC_LED_DEV_STATE_CAN_RX_ID:
            gw_misc_led_flash_time_for_can_rx = msg.param;
            drv_led_ctrl(DRV_LED0_ID, DRV_LED_CTRL_ON);
            break;
        default:
            break;
    }
}

static void gw_misc_segment_timer_handle(gw_mq_msg msg)
{
    static uint8_t ge = 0;
    static uint8_t shi = 0;
    static uint8_t bai = 0;
    static uint8_t qian = 0;
    static uint8_t wan = 0;

    switch (msg.submsg_id)
    {
        case GW_MQ_SUBMSG_MISC_SEGMENT_GE_ID:
            ge = msg.param;
            drv_segment_ctrl(ge,shi,bai,qian,wan);
            break;
        case GW_MQ_SUBMSG_MISC_SEGMENT_SHI_ID:
            shi = msg.param;
            drv_segment_ctrl(ge,shi,bai,qian,wan);
            break;
        case GW_MQ_SUBMSG_MISC_SEGMENT_BAI_ID:
            bai = msg.param;
            drv_segment_ctrl(ge,shi,bai,qian,wan);
            break;
        case GW_MQ_SUBMSG_MISC_SEGMENT_QIAN_ID:
            qian = msg.param;
            drv_segment_ctrl(ge,shi,bai,qian,wan);
            break;
        case GW_MQ_SUBMSG_MISC_SEGMENT_WAN_ID:
            wan = msg.param;
            drv_segment_ctrl(ge,shi,bai,qian,wan);
            break;
        default:
            break;
    }
}

void gw_misc_cb_key_down(void)
{
    gw_mq_msg msg;

    msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
    msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_STD_RXNUM_ID;
    gw_mq_send(GW_MQ_CAN_ID, &msg, 0);
}

void gw_misc_thread(void *argument)
{
    (void)argument;
    osStatus_t ret;
    gw_mq_msg mq_msg;

    LOG_D("gw_misc_thread run\r\n");

    drv_led_init();
    drv_segment_init(gw_misc_cb_key_down);
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
            case GW_MQ_MSG_MISC_SEGMENT_ID:
                gw_misc_segment_timer_handle(mq_msg);
                break;
            default:
                break;
        }
    }
}
