/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "pmsm_init.h"
#include "monitor.h"
#include "command_process.h"


int main(void)
 {
    board_init();
    parm_global_init();
    timer_init();
    monitor_init();
    printf("motor control and monitor test.\n");
    pmsm_motor_init();
    pmsm_foc_angle_align();

    while (1)
    {
      monitor_handle();
      process_cmd();
    }


  
}

