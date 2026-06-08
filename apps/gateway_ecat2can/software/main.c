/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include <stdio.h>
#include "board.h"
#include "gw_thread.h"


int main(void)
{
#ifndef CONFIG_RTTHREAD
    board_init();
#endif
    osKernelInitialize();              /* Initialize CMSIS-RTOS */
    gw_thread_init();
    if (osKernelGetState() == osKernelReady) {
        osKernelStart(); /* Start thread execution */
    }
}
