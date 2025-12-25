/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EC_LOG_H
#define EC_LOG_H

#include <stdio.h>

/* DEBUG level */
#define EC_DBG_ERROR   0
#define EC_DBG_WARNING 1
#define EC_DBG_INFO    2
#define EC_DBG_LOG     3

#ifndef EC_DBG_TAG
#define EC_DBG_TAG "EC"
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

#define ec_master_dbg_log_line(lvl, color_n, fmt, ...)          \
    do {                                                        \
        CONFIG_EC_PRINTF("\033[" #color_n "m[" lvl "/ec_master" \
                         "] ");                                 \
        CONFIG_EC_PRINTF(fmt, ##__VA_ARGS__);                   \
        CONFIG_EC_PRINTF("\033[0m");                            \
    } while (0)

#define ec_slave_dbg_log_line(lvl, color_n, fmt, ...)          \
    do {                                                       \
        CONFIG_EC_PRINTF("\033[" #color_n "m[" lvl "/ec_slave" \
                         "] ");                                \
        CONFIG_EC_PRINTF(fmt, ##__VA_ARGS__);                  \
        CONFIG_EC_PRINTF("\033[0m");                           \
    } while (0)

#if (CONFIG_EC_DBG_LEVEL >= EC_DBG_LOG)
#define EC_LOG_DBG(fmt, ...) ec_master_dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#else
#define EC_LOG_DBG(...) \
    {                   \
    }
#endif

#if (CONFIG_EC_DBG_LEVEL >= EC_DBG_INFO)
#define EC_LOG_INFO(fmt, ...) ec_master_dbg_log_line("I", 32, fmt, ##__VA_ARGS__)
#else
#define EC_LOG_INFO(...) \
    {                    \
    }
#endif

#if (CONFIG_EC_DBG_LEVEL >= EC_DBG_WARNING)
#define EC_LOG_WRN(fmt, ...) ec_master_dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#else
#define EC_LOG_WRN(...) \
    {                   \
    }
#endif

#if (CONFIG_EC_DBG_LEVEL >= EC_DBG_ERROR)
#define EC_LOG_ERR(fmt, ...) ec_master_dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define EC_LOG_ERR(...) \
    {                   \
    }
#endif

#if (CONFIG_EC_SLAVE_DBG_LEVEL >= EC_DBG_LOG)
#define EC_SLAVE_LOG_DBG(fmt, ...) ec_slave_dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#else
#define EC_SLAVE_LOG_DBG(...) \
    {                         \
    }
#endif

#if (CONFIG_EC_SLAVE_DBG_LEVEL >= EC_DBG_INFO)
#define EC_SLAVE_LOG_INFO(fmt, ...) ec_slave_dbg_log_line("I", 32, fmt, ##__VA_ARGS__)
#else
#define EC_SLAVE_LOG_INFO(...) \
    {                          \
    }
#endif

#if (CONFIG_EC_SLAVE_DBG_LEVEL >= EC_DBG_WARNING)
#define EC_SLAVE_LOG_WRN(fmt, ...) ec_slave_dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#else
#define EC_SLAVE_LOG_WRN(...) \
    {                         \
    }
#endif

#if (CONFIG_EC_SLAVE_DBG_LEVEL >= EC_DBG_ERROR)
#define EC_SLAVE_LOG_ERR(fmt, ...) ec_slave_dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define EC_SLAVE_LOG_ERR(...) \
    {                         \
    }
#endif

#define EC_LOG_RAW(...) CONFIG_EC_PRINTF(__VA_ARGS__)

#ifndef CONFIG_EC_ASSERT_DISABLE
#define EC_ASSERT(f)                                                            \
    do {                                                                        \
        if (!(f)) {                                                             \
            EC_LOG_ERR("ASSERT FAIL [%s] @ %s:%d\r\n", #f, __FILE__, __LINE__); \
            while (1) {                                                         \
            }                                                                   \
        }                                                                       \
    } while (false)

#define EC_ASSERT_MSG(f, fmt, ...)                                              \
    do {                                                                        \
        if (!(f)) {                                                             \
            EC_LOG_ERR("ASSERT FAIL [%s] @ %s:%d\r\n", #f, __FILE__, __LINE__); \
            EC_LOG_ERR(fmt "\r\n", ##__VA_ARGS__);                              \
            while (1) {                                                         \
            }                                                                   \
        }                                                                       \
    } while (false)
#else
#define EC_ASSERT(f) \
    {                \
    }
#define EC_ASSERT_MSG(f, fmt, ...) \
    {                              \
    }
#endif

#define ___is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static inline void ec_hexdump(const void *ptr, uint32_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    unsigned int i, j;

    (void)buf;

    for (i = 0; i < buflen; i += 16) {
        CONFIG_EC_PRINTF("%08x:", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen) {
                if ((j % 8) == 0) {
                    CONFIG_EC_PRINTF("  ");
                }

                CONFIG_EC_PRINTF("%02X ", buf[i + j]);
            } else
                CONFIG_EC_PRINTF("   ");
        CONFIG_EC_PRINTF(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                CONFIG_EC_PRINTF("%c", ___is_print(buf[i + j]) ? buf[i + j] : '.');
        CONFIG_EC_PRINTF("\n");
    }
}

#endif /* EC_LOG_H */
