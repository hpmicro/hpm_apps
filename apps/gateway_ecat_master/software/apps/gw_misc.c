/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_misc.h"

gw_misc_led_dev_status_e led_dev_status = GW_MISC_LED_DEV_STATUS_NULL;
static uint8_t key_dev_status_a = false;
static uint8_t key_dev_status_b = false;
static uint8_t key_dev_status_c = false;
static uint8_t key_dev_status_d = false;

void gw_misc_timer_cb(void)
{
    gw_mq_msg mq_msg;

    static uint16_t timeout_1ms = 0;
    static uint16_t timeout_10ms = 0;
    static uint16_t timeout_100ms = 0;
    static uint16_t timeout_1000ms = 0;

    static uint16_t time_out_key_a = 0;
    static uint16_t time_out_key_b = 0;
    static uint16_t time_out_key_c = 0;
    static uint16_t time_out_key_d = 0;	

    timeout_1ms++;
	timeout_10ms++;
	timeout_100ms++;
	timeout_1000ms++;


	if(timeout_1ms>=1) {
		timeout_1ms = 0;
        // mq_msg.msg_id = GW_MQ_MSG_MISC_TIMER_ID;
        // mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_TIMER_1MS_ID;
        // gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
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

	if(key_dev_status_a) {
		time_out_key_a++;
		if(time_out_key_a > 50) {
			if(drv_key_get_status(DRV_KEY_KEYA_ID) && time_out_key_a < 2000) {
                time_out_key_a = 0;
				key_dev_status_a = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_A_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYA_ID);
			} else if(time_out_key_a >= 2000) {
				time_out_key_a = 0;
                key_dev_status_a = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_A_LONG_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYA_ID);
			}
		} else if(time_out_key_a == 50) {
            if(drv_key_get_status(DRV_KEY_KEYA_ID)) {
                time_out_key_a = 0;
                key_dev_status_a = false;
                drv_irq_key_enable(DRV_KEY_KEYA_ID);
            }
        }
	}

	if(key_dev_status_b) {
		time_out_key_b++;
		if(time_out_key_b > 50) {
			if(drv_key_get_status(DRV_KEY_KEYB_ID) && time_out_key_b < 2000) {
                time_out_key_b = 0;
				key_dev_status_b = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_B_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYB_ID);
			} else if(time_out_key_b >= 2000) {
				time_out_key_b = 0;
                key_dev_status_b = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_B_LONG_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYB_ID);
			}
		} else if(time_out_key_b == 50) {
            if(drv_key_get_status(DRV_KEY_KEYB_ID)) {
                time_out_key_b = 0;
                key_dev_status_b = false;
                drv_irq_key_enable(DRV_KEY_KEYB_ID);
            }
        }
	}

	if(key_dev_status_c) {
		time_out_key_c++;
        if(time_out_key_c > 50) {
			if(drv_key_get_status(DRV_KEY_KEYC_ID) && time_out_key_c < 2000) {
                time_out_key_c = 0;
				key_dev_status_c = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_C_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYC_ID);
			} else if(time_out_key_c >= 2000) {
				time_out_key_c = 0;
                key_dev_status_c = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_C_LONG_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYC_ID);
			}
        } else if(time_out_key_c == 50) {
            if(drv_key_get_status(DRV_KEY_KEYC_ID)) {
                time_out_key_c = 0;
                key_dev_status_c = false;
                drv_irq_key_enable(DRV_KEY_KEYC_ID);
            }
        }
	}

	if(key_dev_status_d) {
		time_out_key_d++;
        if(time_out_key_d > 50) {
			if(drv_key_get_status(DRV_KEY_KEYD_ID) && time_out_key_d < 2000) {
                time_out_key_d = 0;
				key_dev_status_d = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_D_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYD_ID);
			} else if(time_out_key_d >= 2000) {
				time_out_key_d = 0;
                key_dev_status_d = false;
                mq_msg.msg_id = GW_MQ_MSG_MISC_KEY_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_KEY_D_LONG_PRESS_ID;
                gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 0);
                drv_irq_key_enable(DRV_KEY_KEYD_ID);
			}
        } else if(time_out_key_d == 50) {
            if(drv_key_get_status(DRV_KEY_KEYD_ID)) {
                time_out_key_d = 0;
                key_dev_status_d = false;
                drv_irq_key_enable(DRV_KEY_KEYD_ID);
            }
        }
	}
}

void gw_misc_timer_handle(gw_mq_msg msg)
{
    gw_mq_msg mq_msg;

	switch (msg.submsg_id)
	{
        case GW_MQ_SUBMSG_MISC_TIMER_1MS_ID:
            break;
		case GW_MQ_SUBMSG_MISC_TIMER_10MS_ID:
			break;
		case GW_MQ_SUBMSG_MISC_TIMER_100MS_ID:
            mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_UPDATE_ID;
            gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 0);
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_UPDATE_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
			break;
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
        default:
            break;
    }
    LOG_I("led submsg_id = %d\r\n", msg.submsg_id);
}

static void gw_irq_keya_handler_cb(void)
{
	key_dev_status_a = true;
    drv_irq_key_disable(DRV_KEY_KEYA_ID);
}

static void gw_irq_keyb_handler_cb(void)
{
	key_dev_status_b = true;
    drv_irq_key_disable(DRV_KEY_KEYB_ID);
}

static void gw_irq_keyc_handler_cb(void)
{
	key_dev_status_c = true;
    drv_irq_key_disable(DRV_KEY_KEYC_ID);
}

static void gw_irq_keyd_handler_cb(void)
{
	key_dev_status_d = true;
    drv_irq_key_disable(DRV_KEY_KEYD_ID);
}

static void gw_misc_key_handle(gw_mq_msg msg)
{
    gw_mq_msg mq_msg;

    switch (msg.submsg_id)
    {
        case GW_MQ_SUBMSG_MISC_KEY_A_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYA_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_A_LONG_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYA_LONG_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_B_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYB_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_B_LONG_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYB_LONG_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_C_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYC_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_C_LONG_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYC_LONG_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_D_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYD_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        case GW_MQ_SUBMSG_MISC_KEY_D_LONG_PRESS_ID:
            mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_KEYD_LONG_CLICKED_ID;
            gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            break;
        default:
            break;
    }
    LOG_I("key submsg_id = %d\r\n", msg.submsg_id);
}

void gw_misc_thread(void *argument)
{
    (void)argument;
    osStatus_t ret;
    gw_mq_msg mq_msg;

    LOG_D("gw_misc_thread run\r\n");

    drv_led_init();
    board_timer_create(1, gw_misc_timer_cb);
    drv_key_init();
    drv_irq_keya_init(gw_irq_keya_handler_cb);
	drv_irq_keyb_init(gw_irq_keyb_handler_cb);
	drv_irq_keyc_init(gw_irq_keyc_handler_cb);
	drv_irq_keyd_init(gw_irq_keyd_handler_cb);

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
            case GW_MQ_MSG_MISC_KEY_ID:
                gw_misc_key_handle(mq_msg);
                break;
            default:
                break;
        }
    }
}
