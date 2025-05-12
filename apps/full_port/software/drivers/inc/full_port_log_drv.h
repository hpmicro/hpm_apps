/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_LOG_DRV_H_
#define __FULL_PORT_LOG_DRV_H_

#define FULL_PORT_LOG_LEVEL_DEBUG   0
#define FULL_PORT_LOG_LEVEL_INFO    1
#define FULL_PORT_LOG_LEVEL_WARNING 2
#define FULL_PORT_LOG_LEVEL_ERROR   3

/*******************Log level define part start*******************/
#define CFG_FULL_PORT_LOG_LEVEL FULL_PORT_LOG_LEVEL_DEBUG

#if (CFG_FULL_PORT_LOG_LEVEL <= FULL_PORT_LOG_LEVEL_ERROR)
#define FULL_PORT_LOG_ERROR(format, ...) \
    full_port_log_stash_info(FULL_PORT_LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define FULL_PORT_LOG_ERROR_ARRAY(array, arraylenth, format, ...) \
    full_port_log_stash_array(FULL_PORT_LOG_LEVEL_ERROR, array, arraylenth, format, ##__VA_ARGS__)
#else
#define FULL_PORT_LOG_ERROR(format, ...)
#define FULL_PORT_LOG_ERROR_ARRAY(array, arraylenth, format, ...) 
#endif

#if (CFG_FULL_PORT_LOG_LEVEL <= FULL_PORT_LOG_LEVEL_WARNING)
#define FULL_PORT_LOG_WARNING(format, ...) \
    full_port_log_stash_info(FULL_PORT_LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define FULL_PORT_LOG_WARNING_ARRAY(array, arraylenth, format, ...) \
    full_port_log_stash_array(FULL_PORT_LOG_LEVEL_WARNING, array, arraylenth, format, ##__VA_ARGS__)
#else
#define FULL_PORT_LOG_WARNING(format, ...) 
#define FULL_PORT_LOG_WARNING_ARRAY(array, arraylenth, format, ...)
#endif

#if (CFG_FULL_PORT_LOG_LEVEL <= FULL_PORT_LOG_LEVEL_INFO)
#define FULL_PORT_LOG_INFO(format, ...) \
    full_port_log_stash_info(FULL_PORT_LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define FULL_PORT_LOG_INFO_ARRAY(array, arraylenth, format, ...) \
    full_port_log_stash_array(FULL_PORT_LOG_LEVEL_INFO, array, arraylenth, format, ##__VA_ARGS__)
#else
#define FULL_PORT_LOG_INFO(format, ...)
#define FULL_PORT_LOG_INFO_ARRAY(array, arraylenth, format, ...) 
#endif

#if (CFG_FULL_PORT_LOG_LEVEL <= FULL_PORT_LOG_LEVEL_DEBUG)
#define FULL_PORT_LOG_DEBUG(format, ...) \
    full_port_log_stash_info(FULL_PORT_LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define FULL_PORT_LOG_DEBUG_ARRAY(array, arraylenth, format, ...)\
    full_port_log_stash_array(FULL_PORT_LOG_LEVEL_DEBUG, array, arraylenth, format, ##__VA_ARGS__)
#else
#define FULL_PORT_LOG_DEBUG(format, ...)
#define FULL_PORT_LOG_DEBUG_ARRAY(array, arraylenth, format, ...)
#endif

/**
 * @brief 
 * @param[in] s16Level log level; 日志等级
 * @param[in] p_s8Info log content point; 日志内容指针
 */
void full_port_log_stash_info(int16_t s16Level, char* p_s8Info, ...);

/**
 * @brief 
 * @param[in] s16Level log level; 日志等级
 * @param[in] p_u8Array log array; 日志数组
 * @param[in] u16ArrayLenth log array len; 日志数组长度
 * @param[in] p_s8Info log content point; 日志内容指针
 */
void full_port_log_stash_array(int16_t s16Level, uint8_t* p_u8Array, uint16_t u16ArrayLenth, char* p_s8Info, ...);


#endif /* __FULL_PORT_LOG_H */

/******************************* END OF FILE **********************************/
