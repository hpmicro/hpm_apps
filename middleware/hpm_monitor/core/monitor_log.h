/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MONITOR_LOG_H
#define MONITOR_LOG_H

#include <stdio.h>
#include "monitor_kconfig.h"

/* DEBUG level */
#define MONITOR_DBG_ERROR   0
#define MONITOR_DBG_WARNING 1
#define MONITOR_DBG_INFO    2
#define MONITOR_DBG_LOG     3

#ifndef MONITOR_DBG_TAG
#define MONITOR_DBG_TAG "MONITOR"
#endif
/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */

#ifdef  CONFIG_MONITOR_PRINTF_COLOR_ENABLE
#define _MONITOR_DBG_COLOR(n) CONFIG_MONITOR_PRINTF("\033[" #n "m")
#define _MONITOR_DBG_LOG_HDR(lvl_name, color_n) \
    CONFIG_MONITOR_PRINTF("\033[" #color_n "m[" lvl_name "/" MONITOR_DBG_TAG "] ")
#define _MONITOR_DBG_LOG_X_END \
    CONFIG_MONITOR_PRINTF("\033[0m")
#else
#define _MONITOR_DBG_COLOR(n)
#define _MONITOR_DBG_LOG_HDR(lvl_name, color_n) \
    CONFIG_MONITOR_PRINTF("[" lvl_name "/" MONITOR_DBG_TAG "] ")
#define _MONITOR_DBG_LOG_X_END
#endif

#define monitor_dbg_log_line(lvl, color_n, fmt, ...) \
    do {                                         \
        _MONITOR_DBG_LOG_HDR(lvl, color_n);          \
        CONFIG_MONITOR_PRINTF(fmt, ##__VA_ARGS__);              \
        _MONITOR_DBG_LOG_X_END;                      \
    } while (0)

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
#define MONITOR_LOG_DBG(fmt, ...) monitor_dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#else
#define MONITOR_LOG_DBG(...)
#endif

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_INFO)
#define MONITOR_LOG_INFO(fmt, ...) monitor_dbg_log_line("I", 32, fmt, ##__VA_ARGS__)
#else
#define MONITOR_LOG_INFO(...)
#endif

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_WARNING)
#define MONITOR_LOG_WRN(fmt, ...) monitor_dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#else
#define MONITOR_LOG_WRN(...)
#endif

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_ERROR)
#define MONITOR_LOG_ERR(fmt, ...) monitor_dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define MONITOR_LOG_ERR(...)
#endif

#define MONITOR_LOG_RAW(...) CONFIG_MONITOR_PRINTF(__VA_ARGS__)

#endif /* MONITOR_LOG_H */
