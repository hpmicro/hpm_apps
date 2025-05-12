/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_KEY_DRV_H_
#define __FULL_PORT_KEY_DRV_H_

#define FULL_PORT_IRQ_KEYA_IRQ                  IRQn_GPIO0_E
#define FULL_PORT_IRQ_KEYA_PRIORITY             5
#define FULL_PORT_IRQ_KEYB_IRQ                  IRQn_GPIO0_E
#define FULL_PORT_IRQ_KEYB_PRIORITY             5
#define FULL_PORT_IRQ_KEYC_IRQ                  IRQn_GPIO0_E
#define FULL_PORT_IRQ_KEYC_PRIORITY             5
#define FULL_PORT_IRQ_KEYD_IRQ                  IRQn_GPIO0_E
#define FULL_PORT_IRQ_KEYD_PRIORITY             5
#define FULL_PORT_IRQ_KEY_IRQ0                  IRQn_GPIO0_E        //use for isr func declaration

typedef enum __FULL_PORT_KEY_INDEX_ID__
{
    FULL_PORT_KEY_INDEX_BASE_ID  = 0,  
    FULL_PORT_KEY_INDEX_KEYA_ID,
    FULL_PORT_KEY_INDEX_KEYB_ID,
    FULL_PORT_KEY_INDEX_KEYC_ID,
    FULL_PORT_KEY_INDEX_KEYD_ID,
    FULL_PORT_KEY_INDEX_KEYMAX_ID,
}FULL_PORT_KEY_INDEX_ID_e;

/**
 * @brief key init
 */
void full_port_key_init(void);

/**
 * @brief get key status; 获取按键状态
 * @param[in] eKeyIndexId key param; 按键参数 FULL_PORT_KEY_INDEX_ID_e
 * @return 1, 0, -1
 */
int8_t full_port_key_get_status(FULL_PORT_KEY_INDEX_ID_e eKeyIndexId);

void full_port_irq_keya_init(full_port_irq_cb_func cb);
void full_port_irq_keyb_init(full_port_irq_cb_func cb);
void full_port_irq_keyc_init(full_port_irq_cb_func cb);
void full_port_irq_keyd_init(full_port_irq_cb_func cb);
#endif // __FULL_PORT_KEY_H_
