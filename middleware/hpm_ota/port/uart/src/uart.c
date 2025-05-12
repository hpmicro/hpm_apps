/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "uart_xmodem.h"
#include "hpm_ota.h"
#include "hpm_flash.h"
#include "ota_port.h"

static int uart_writecb(unsigned char *data, int len)
{
    return hpm_ota_auto_write((void *)data, len, true);
}


int hpm_ota_init(void)
{
    int ret = hpm_flash_init();
    if (ret != 0)
    {
        printf("BAD! flash init fail:%d\r\n", ret);
        while (1)
        {
        }
    }

    return 0;
}

void hpm_ota_polling_handle(void)
{
    xmodemReceive(NULL, 0, uart_writecb);
}
