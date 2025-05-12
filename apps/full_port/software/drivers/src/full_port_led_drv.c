/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "full_port_config_drv.h"

void full_port_led_init(void)
{
   board_init_led();
}

void full_port_led_ctrl(FULL_PORT_LED_INDEX_ID_e eLedIndexId, FULL_PORT_LED_CTRL_e eLedCtrl)
{
    switch (eLedIndexId)
    {
        case FULL_PORT_LED_INDEX_LEDA_ID:
             if(eLedCtrl == FULL_PORT_LED_CTRL_ON) {
                board_control_led(BOARD_LED0_CTRL_INDEX, BOARD_LED_CTRL_FLAG_ON);
             } else if(eLedCtrl == FULL_PORT_LED_CTRL_OFF) {
                board_control_led(BOARD_LED0_CTRL_INDEX, BOARD_LED_CTRL_FLAG_OFF);
             } else {
                board_control_led(BOARD_LED0_CTRL_INDEX, BOARD_LED_CTRL_FLAG_TOG);
             }
        break;
        case FULL_PORT_LED_INDEX_LEDB_ID:
             if(eLedCtrl == FULL_PORT_LED_CTRL_ON) {
                board_control_led(BOARD_LED1_CTRL_INDEX, BOARD_LED_CTRL_FLAG_ON);
             } else if(eLedCtrl == FULL_PORT_LED_CTRL_OFF) {
                board_control_led(BOARD_LED1_CTRL_INDEX, BOARD_LED_CTRL_FLAG_OFF);
             } else {
                board_control_led(BOARD_LED1_CTRL_INDEX, BOARD_LED_CTRL_FLAG_TOG);
             }
        break;
        default:
        break;
    }

    
}