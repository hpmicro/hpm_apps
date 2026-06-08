/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "app_misc.h"


static uint8_t key_dev_status_a = false;
static uint8_t key_dev_status_b = false;
static uint8_t key_dev_status_c = false;
static uint8_t key_dev_status_d = false;

void app_misc_timer_cb(void)
{
    app_mq_msg mq_msg;

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
	}
	if(timeout_10ms>=10) {
		timeout_10ms = 0;
        mq_msg.msg_id = APP_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_TIMER_10MS_ID;
        app_mq_send_isr(APP_MQ_MISC_ID, &mq_msg);
	}
	if(timeout_100ms>=100) {
		timeout_100ms = 0;
        mq_msg.msg_id = APP_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_TIMER_100MS_ID;
        app_mq_send_isr(APP_MQ_MISC_ID, &mq_msg);
	}
	if(timeout_1000ms>=1000) {
		timeout_1000ms = 0;
        mq_msg.msg_id = APP_MQ_MSG_MISC_TIMER_ID;
        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_TIMER_1000MS_ID;
        app_mq_send_isr(APP_MQ_MISC_ID, &mq_msg);
	}
}

void app_misc_timer_handle(app_mq_msg msg)
{
    static uint16_t timeout_key_a = 0;
    static uint16_t timeout_key_b = 0;
    static uint16_t timeout_key_c = 0;
    static uint16_t timeout_key_d = 0;

    app_mq_msg mq_msg;

	switch (msg.submsg_id)
	{
        case APP_MQ_SUBMSG_MISC_TIMER_1MS_ID:
            break;
		case APP_MQ_SUBMSG_MISC_TIMER_10MS_ID:
            if(key_dev_status_a) {
                timeout_key_a++;
                if(timeout_key_a > 5) {
                    if(drv_key_get_status(DRV_KEY_KEYA_ID) && timeout_key_a < 200) {
                        timeout_key_a = 0;
                        key_dev_status_a = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_A_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYA_ID, true);
                    } else if(timeout_key_a >= 200) {
                        timeout_key_a = 0;
                        key_dev_status_a = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_A_LONG_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYA_ID, true);
                    }
                } else if(timeout_key_a == 5) {
                    if(drv_key_get_status(DRV_KEY_KEYA_ID)) {
                        timeout_key_a = 0;
                        key_dev_status_a = false;
                        drv_key_irq_ctrl(DRV_KEY_KEYA_ID, true);
                    }
                }
            }

            if(key_dev_status_b) {
                timeout_key_b++;
                if(timeout_key_b > 5) {
                    if(drv_key_get_status(DRV_KEY_KEYB_ID) && timeout_key_b < 200) {
                        timeout_key_b = 0;
                        key_dev_status_b = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_B_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYB_ID, true);
                    } else if(timeout_key_b >= 200) {
                        timeout_key_b = 0;
                        key_dev_status_b = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_B_LONG_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYB_ID, true);
                    }
                } else if(timeout_key_b == 5) {
                    if(drv_key_get_status(DRV_KEY_KEYB_ID)) {
                        timeout_key_b = 0;
                        key_dev_status_b = false;
                        drv_key_irq_ctrl(DRV_KEY_KEYB_ID, true);
                    }
                }
            }

            if(key_dev_status_c) {
                timeout_key_c++;
                if(timeout_key_c > 5) {
                    if(drv_key_get_status(DRV_KEY_KEYC_ID) && timeout_key_c < 200) {
                        timeout_key_c = 0;
                        key_dev_status_c = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_C_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYC_ID, true);
                    } else if(timeout_key_c >= 200) {
                        timeout_key_c = 0;
                        key_dev_status_c = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_C_LONG_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYC_ID, true);
                    }
                } else if(timeout_key_c == 5) {
                    if(drv_key_get_status(DRV_KEY_KEYC_ID)) {
                        timeout_key_c = 0;
                        key_dev_status_c = false;
                        drv_key_irq_ctrl(DRV_KEY_KEYC_ID, true);
                    }
                }
            }

            if(key_dev_status_d) {
                timeout_key_d++;
                if(timeout_key_d > 5) {
                    if(drv_key_get_status(DRV_KEY_KEYD_ID) && timeout_key_d < 200) {
                        timeout_key_d = 0;
                        key_dev_status_d = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_D_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYD_ID, true);
                    } else if(timeout_key_d >= 200) {
                        timeout_key_d = 0;
                        key_dev_status_d = false;
                        mq_msg.msg_id = APP_MQ_MSG_MISC_KEY_ID;
                        mq_msg.submsg_id = APP_MQ_SUBMSG_MISC_KEY_D_LONG_PRESS_ID;
                        app_mq_send(APP_MQ_MISC_ID, &mq_msg, 0);
                        drv_key_irq_ctrl(DRV_KEY_KEYD_ID, true);
                    }
                } else if(timeout_key_d == 5) {
                    if(drv_key_get_status(DRV_KEY_KEYD_ID)) {
                        timeout_key_d = 0;
                        key_dev_status_d = false;
                        drv_key_irq_ctrl(DRV_KEY_KEYD_ID, true);
                    }
                }
            }
			break;
		case APP_MQ_SUBMSG_MISC_TIMER_100MS_ID:
            drv_led_ctrl(DRV_LED0_ID, DRV_LED_CTRL_TOGGLE);
			break;
		case APP_MQ_SUBMSG_MISC_TIMER_1000MS_ID:
            drv_led_ctrl(DRV_LED1_ID, DRV_LED_CTRL_TOGGLE);
			break;
		default:
			break;
	}
}

static void app_misc_led_handle(app_mq_msg msg)
{
    switch (msg.submsg_id)
    {
        case 0:
            break;
        default:
            break;
    }
}

static void app_misc_keya_handler_cb(void)
{
	key_dev_status_a = true;
    drv_key_irq_ctrl(DRV_KEY_KEYA_ID, false);
}

static void app_misc_keyb_handler_cb(void)
{
	key_dev_status_b = true;
    drv_key_irq_ctrl(DRV_KEY_KEYB_ID, false);
}

static void app_misc_keyc_handler_cb(void)
{
    key_dev_status_c = true;
    drv_key_irq_ctrl(DRV_KEY_KEYC_ID, false);
}

static void app_misc_keyd_handler_cb(void)
{   
    key_dev_status_d = true;
    drv_key_irq_ctrl(DRV_KEY_KEYD_ID, false);
}

static void app_misc_key_handle(app_mq_msg msg)
{
    //app_mq_msg mq_msg;

    switch (msg.submsg_id)
    {
        case APP_MQ_SUBMSG_MISC_KEY_A_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_A_LONG_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_B_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_B_LONG_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_C_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_C_LONG_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_D_PRESS_ID:
            break;
        case APP_MQ_SUBMSG_MISC_KEY_D_LONG_PRESS_ID:
            break;
        default:
            break;
    }
    LOG_I("key submsg_id = %d\r\n", msg.submsg_id);
}
void app_misc_thread(void *argument)
{
    (void)argument;
    BaseType_t ret;
    app_mq_msg mq_msg;

    LOG_I("app_misc_thread run\r\n");

    drv_led_init();
    drv_key_init();
    drv_key_keya_init(app_misc_keya_handler_cb);
    drv_key_keyb_init(app_misc_keyb_handler_cb);
    drv_key_keyc_init(app_misc_keyc_handler_cb);
    drv_key_keyd_init(app_misc_keyd_handler_cb);
    board_timer_create(1, app_misc_timer_cb);
    drv_oled_init();

    for(;;)
    {
        ret = app_mq_recv(APP_MQ_MISC_ID, &mq_msg, portMAX_DELAY);

        if(ret < 0) {
            continue;
            LOG_E("%s mq recv fail!\r\n",__func__);
        }

        switch (mq_msg.msg_id)
        {
            case APP_MQ_MSG_MISC_TIMER_ID:
                app_misc_timer_handle(mq_msg);
                break;
            case APP_MQ_MSG_MISC_LED_ID:
                app_misc_led_handle(mq_msg);
                break;
            case APP_MQ_MSG_MISC_KEY_ID:
                app_misc_key_handle(mq_msg);
                break;
            default:
                break;
        }
    }
}
