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
#include "hpm_uart_drv.h"
#include "uart_channel.h"

void hpm_uart_channel_init(void)
{
    printf("default uart, board console init done!\r\n");
}

void hpm_uart_channel_handle(uart_cb_t uart_cb)
{
    static uint8_t recv_buffer[2048];
    static uint32_t recv_len = 0;
    bool new_data = false;
    while(1)
    {
        if(recv_len >= sizeof(recv_buffer))
            recv_len = 0;
        if(uart_receive_byte(BOARD_CONSOLE_UART_BASE, &recv_buffer[recv_len]) != status_success)
            break;
        recv_len++;
        new_data = true;
    }
    if(new_data && uart_cb != NULL && uart_cb(recv_buffer, recv_len) >= 0)
    {
        recv_len = 0;
    }
}

void hpm_uart_block_task(void* proc)
{
    (void)proc;
}

int hpm_uart_channel_output(uint8_t *data, uint16_t length)
{
    if(data == NULL || length <= 0)
        return -1;
    return (int)uart_send_data(BOARD_CONSOLE_UART_BASE, data, length);
}

