/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef DRV_LED_H_
#define DRV_LED_H_
#include "board.h"
#include "hpm_gpio_drv.h"

typedef enum
{
    DRV_LED0_R_ID= 0,
    DRV_LED0_G_ID,
    DRV_LED0_B_ID,
    DRV_LED1_R_ID,
    DRV_LED1_G_ID,
    DRV_LED1_B_ID
}drv_led_id_e;

typedef enum
{
    DRV_LED_CTRL_ON = 0,
    DRV_LED_CTRL_OFF,
    DRV_LED_CTRL_TOGGLE
}drv_led_ctrl_e;

void drv_led_init(void);
void drv_led_ctrl(drv_led_id_e id, drv_led_ctrl_e ctrl);

#endif /* DRV_LED_H_ */
