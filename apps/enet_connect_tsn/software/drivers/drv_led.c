/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_led.h"

void drv_led_init(void)
{
   board_init_led();
}

void drv_led_ctrl(drv_led_id_e id, drv_led_ctrl_e ctrl)
{
   	switch (id)
    {
        case DRV_LED0_ID: 
        	if(ctrl == DRV_LED_CTRL_ON) {
                gpio_write_pin(BOARD_LED0_GPIO_CTRL, BOARD_LED0_GPIO_INDEX, BOARD_LED0_GPIO_PIN, false);
            } else if(ctrl == DRV_LED_CTRL_OFF) {
                gpio_write_pin(BOARD_LED0_GPIO_CTRL, BOARD_LED0_GPIO_INDEX, BOARD_LED0_GPIO_PIN, true);
            } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
                gpio_toggle_pin(BOARD_LED0_GPIO_CTRL, BOARD_LED0_GPIO_INDEX, BOARD_LED0_GPIO_PIN);
            }
            break;
        case DRV_LED1_ID:
        	if(ctrl == DRV_LED_CTRL_ON) {
                gpio_write_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN, false);
            } else if(ctrl == DRV_LED_CTRL_OFF) {
                gpio_write_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN, true);
            } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
                gpio_toggle_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN);
            }
            break;
        default:
            break;
    }
}