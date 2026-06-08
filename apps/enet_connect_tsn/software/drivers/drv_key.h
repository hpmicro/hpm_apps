/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef DRV_KEY_H_
#define DRV_KEY_H_
#include "board.h"
#include "hpm_gpio_drv.h"

#define DRV_KEY_KEYA_IRQ                  IRQn_GPIO0_C
#define DRV_KEY_KEYA_PRIORITY             5
#define DRV_KEY_KEYB_IRQ                  IRQn_GPIO0_C
#define DRV_KEY_KEYB_PRIORITY             5
#define DRV_KEY_KEYC_IRQ                  IRQn_GPIO0_C
#define DRV_KEY_KEYC_PRIORITY             5
#define DRV_KEY_KEYD_IRQ                  IRQn_GPIO0_C
#define DRV_KEY_KEYD_PRIORITY             5
#define DRV_KEY_KEY_IRQ                   IRQn_GPIO0_C        //use for isr func declaration

typedef void (*drv_key_irq_cb_func)(void);

typedef enum
{
    DRV_KEY_KEYA_ID = 0,
    DRV_KEY_KEYB_ID,
    DRV_KEY_KEYC_ID,
    DRV_KEY_KEYD_ID,
    DRV_KEY_KEYMAX_ID,
}drv_key_id_e;

void drv_key_init(void);
void drv_key_keya_init(drv_key_irq_cb_func cb);
void drv_key_keyb_init(drv_key_irq_cb_func cb);
void drv_key_keyc_init(drv_key_irq_cb_func cb);
void drv_key_keyd_init(drv_key_irq_cb_func cb);
void drv_key_irq_ctrl(drv_key_id_e id, bool enable);
int8_t drv_key_get_status(drv_key_id_e id);

#endif /* DRV_KEY_H_ */
