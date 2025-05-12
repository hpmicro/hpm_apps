/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_LOG_H
#define HPM_LOG_H

#include <stdio.h>
#include "ota_kconfig.h"

/* DEBUG level */
#define HPM_DBG_ERROR   0
#define HPM_DBG_WARNING 1
#define HPM_DBG_INFO    2
#define HPM_DBG_LOG     3

#ifndef HPM_DBG_TAG
#define HPM_DBG_TAG "OTA"
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

#ifdef  CONFIG_HPM_PRINTF_COLOR_ENABLE
#define _HPM_DBG_COLOR(n) CONFIG_HPM_PRINTF("\033[" #n "m")
#define _HPM_DBG_LOG_HDR(lvl_name, color_n) \
    CONFIG_HPM_PRINTF("\033[" #color_n "m[" lvl_name "/" HPM_DBG_TAG "] ")
#define _HPM_DBG_LOG_X_END \
    CONFIG_HPM_PRINTF("\033[0m")
#else
#define _HPM_DBG_COLOR(n)
#define _HPM_DBG_LOG_HDR(lvl_name, color_n) \
    CONFIG_HPM_PRINTF("[" lvl_name "/" HPM_DBG_TAG "] ")
#define _HPM_DBG_LOG_X_END
#endif

#define hpm_dbg_log_line(lvl, color_n, fmt, ...) \
    do {                                         \
        _HPM_DBG_LOG_HDR(lvl, color_n);          \
        CONFIG_HPM_PRINTF(fmt, ##__VA_ARGS__);              \
        _HPM_DBG_LOG_X_END;                      \
    } while (0)

#if (CONFIG_HPM_DBG_LEVEL >= HPM_DBG_LOG)
#define HPM_LOG_DBG(fmt, ...) hpm_dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#else
#define HPM_LOG_DBG(...)
#endif

#if (CONFIG_HPM_DBG_LEVEL >= HPM_DBG_INFO)
#define HPM_LOG_INFO(fmt, ...) hpm_dbg_log_line("I", 32, fmt, ##__VA_ARGS__)
#else
#define HPM_LOG_INFO(...)
#endif

#if (CONFIG_HPM_DBG_LEVEL >= HPM_DBG_WARNING)
#define HPM_LOG_WRN(fmt, ...) hpm_dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#else
#define HPM_LOG_WRN(...)
#endif

#if (CONFIG_HPM_DBG_LEVEL >= HPM_DBG_ERROR)
#define HPM_LOG_ERR(fmt, ...) hpm_dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define HPM_LOG_ERR(...)
#endif

#define HPM_LOG_RAW(...) CONFIG_HPM_PRINTF(__VA_ARGS__)

#endif /* HPM_LOG_H */
