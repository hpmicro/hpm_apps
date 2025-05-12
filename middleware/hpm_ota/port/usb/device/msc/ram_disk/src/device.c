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
#include "hpm_ota.h"
#include "hpm_flash.h"
#include "ota_port.h"

extern void msc_ram_init(uint8_t busid, uint32_t reg_base);

static int usbdevice_writecb(unsigned char *data, uint32_t len)
{
    return hpm_ota_split_write((void *)data, len, true);
}

int hpm_ota_init(void)
{
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    int ret = hpm_flash_init();
    if (ret != 0)
    {
        printf("BAD! flash init fail:%d\r\n", ret);
        while (1)
        {
        }
    }

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);

    printf("ota port cherry usb msc_ram init.\n");

    msc_ram_init(0, (uint32_t)CONFIG_HPM_USBD_BASE);

    data_process = (DataProcess)usbdevice_writecb;

    return 0;
}

void hpm_ota_polling_handle(void)
{

}
