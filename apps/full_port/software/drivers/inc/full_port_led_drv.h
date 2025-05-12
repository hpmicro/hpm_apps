/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_LED_DRV_H_
#define __FULL_PORT_LED_DRV_H_

typedef enum __FULL_PORT_LED_CTRL__
{ 
    FULL_PORT_LED_CTRL_ON = 0,
    FULL_PORT_LED_CTRL_OFF,
    FULL_PORT_LED_CTRL_TOGGLE,
}FULL_PORT_LED_CTRL_e;

typedef enum __FULL_PORT_LED_INDEX_ID__
{
    FULL_PORT_LED_INDEX_BASE_ID  = 0,  
    FULL_PORT_LED_INDEX_LEDA_ID,
    FULL_PORT_LED_INDEX_LEDB_ID,
    FULL_PORT_LED_INDEX_LEDMAX_ID,
}FULL_PORT_LED_INDEX_ID_e;

/**
 * @brief led init
 */
void full_port_led_init(void);

/**
 * @brief led control; led 控制
 * @param[in] eLedIndexId index from FULL_PORT_LED_INDEX_ID_e
 * @param[in] eLedCtrl ctrl from FULL_PORT_LED_CTRL_e: FULL_PORT_LED_CTRL_ON FULL_PORT_LED_CTRL_OFF FULL_PORT_LED_CTRL_TOGGLE
 */
void full_port_led_ctrl(FULL_PORT_LED_INDEX_ID_e eLedIndexId, FULL_PORT_LED_CTRL_e eLedCtrl);

#endif // __FULL_PORT_LED_H_