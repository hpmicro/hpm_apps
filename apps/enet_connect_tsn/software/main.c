/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include <stdio.h>
#include "board.h"
#include "app_thread.h"

int main(void)
{
#ifndef CONFIG_RTTHREAD
    board_init();
#endif
    app_thread_init();
}

