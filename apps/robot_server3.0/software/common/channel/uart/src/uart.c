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
#include "uart_channel.h"

void hpm_uart_channel_init(void)
{

}

void hpm_uart_block_task(void* proc)
{
    xmodemReceive(NULL, 0, proc);
}


