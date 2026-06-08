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

#define DRV_IRQ_KEYA_IRQ                  IRQn_GPIO0_E
#define DRV_IRQ_KEYA_PRIORITY             5
#define DRV_IRQ_KEYB_IRQ                  IRQn_GPIO0_E
#define DRV_IRQ_KEYB_PRIORITY             5
#define DRV_IRQ_KEYC_IRQ                  IRQn_GPIO0_E
#define DRV_IRQ_KEYC_PRIORITY             5
#define DRV_IRQ_KEYD_IRQ                  IRQn_GPIO0_E
#define DRV_IRQ_KEYD_PRIORITY             5
#define DRV_IRQ_KEY_IRQ0                  IRQn_GPIO0_E  

/* irq config */
typedef void (*drv_irq_cb_func)(void);

typedef enum
{
    DRV_KEY_KEYA_ID = 0,
    DRV_KEY_KEYB_ID,
    DRV_KEY_KEYC_ID,
    DRV_KEY_KEYD_ID,
    DRV_KEY_KEYMAX_ID,
}drv_key_id_e;

void drv_key_init(void);
int8_t drv_key_get_status(drv_key_id_e id);
void drv_irq_keya_init(drv_irq_cb_func cb);
void drv_irq_keyb_init(drv_irq_cb_func cb);
void drv_irq_keyc_init(drv_irq_cb_func cb);
void drv_irq_keyd_init(drv_irq_cb_func cb);
void drv_irq_key_enable(drv_key_id_e id);
void drv_irq_key_disable(drv_key_id_e id);

#endif /* DRV_KEY_H_ */
