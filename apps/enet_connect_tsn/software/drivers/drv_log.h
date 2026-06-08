/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __DRV_LOG_H__
#define __DRV_LOG_H__
#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_log.h"
#include "hpm_uart_drv.h"

#define LOG_D(fmt, ...) HPM_LOG_PRINTF("\033[0;37;40m"); \
                        HPM_LOG_DEBUG(fmt, ##__VA_ARGS__); \
                        HPM_LOG_PRINTF("\033[0m");
                        
#define LOG_I(fmt, ...) HPM_LOG_PRINTF("\033[0;32;40m"); \
                        HPM_LOG_INFO(fmt, ##__VA_ARGS__); \
                        HPM_LOG_PRINTF("\033[0m");

#define LOG_W(fmt, ...) HPM_LOG_PRINTF("\033[0;33;40m"); \
                        HPM_LOG_WARN(fmt, ##__VA_ARGS__); \
                        HPM_LOG_PRINTF("\033[0m");

#define LOG_E(fmt, ...) HPM_LOG_PRINTF("\033[0;31;40m"); \
                        HPM_LOG_ERR(fmt, ##__VA_ARGS__); \
                        HPM_LOG_PRINTF("\033[0m");

void drv_log_init(void);

#endif
