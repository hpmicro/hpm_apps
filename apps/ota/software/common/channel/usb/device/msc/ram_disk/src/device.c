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
#include "msc_ram.h"
#include "usb_channel.h"

extern void msc_ram_init(uint8_t busid, uint32_t reg_base);

void hpm_usb_device_channel_init(void)
{
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    printf("cherry usb msc_ram sample.\n");
}

void hpm_usb_device_block_task(void* proc)
{
    data_process = (DataProcess)proc;
    msc_ram_init(0, CONFIG_HPM_USBD_BASE);
    while (1) {
    }
}
