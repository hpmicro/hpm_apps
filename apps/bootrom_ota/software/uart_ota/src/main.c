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
#include "uart_xmodem.h"
#include "flash_map.h"
#include "ota_api.h"

 /* Write file to flash */
static int writefile(unsigned char* data, int len)
{
    /* First packet contain file header info */
    ota_board_auto_write((void*)data, len);
    return 0;
}

int main(void)
{
    int ret = 0;
    uint8_t ota_index;
    board_init();

    printf("hello world!\n");
    ota_board_flash_init();
    ota_index = ota_check_current_otaindex();
    printf("now runing OTA%d...\n", ota_index+1);


    ret = xmodemReceive(writefile);
    if (ret > 0)
       ota_board_complete_reset();
    while (1)
    {

    }
    return 0;
}


