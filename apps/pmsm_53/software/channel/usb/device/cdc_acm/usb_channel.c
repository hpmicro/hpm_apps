/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "usb_config.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern void cdc_acm_init(uint8_t busid, uint32_t reg_base);
extern void USBD_IRQHandler(uint8_t busid);

void hpm_usb_device_channel_init(void)
{
    board_init_usb_pins();
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    printf("cherry usb cdc_acm device sample.\n");
    cdc_acm_init(0, CONFIG_HPM_USBD_BASE);
#ifdef USB_POLLING_PROCESS
    intc_m_disable_irq(CONFIG_HPM_USBD_IRQn);
#endif
}

void hpm_usb_device_channel_handle(void)
{
#ifdef USB_POLLING_PROCESS
    USBD_IRQHandler(0);
#endif
}

