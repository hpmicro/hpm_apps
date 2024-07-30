/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_APPHEADER_H
#define __HPM_APPHEADER_H

#include "hpm_common.h"

/**
 * @brief HPM APP HEADER  information;apphead_info接口
 * @addtogroup Hpm_App_Header_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief Maximum size of the hash data
 *        ;hash值最大长度
 */
#define HPM_APP_HASH_DATA_MAXSIZE   (64)

/**
 * @brief Magic information
 *        ;magic信息
 */
#define HPM_APP_FILE_FLAG_MAGIC   (0xbeaf5aa5)

/**
 * @brief ID information
 *        ;id信息
 */
#define HPM_APP_FILE_TOUCH_ID     (0xa5a55a5a)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HPM APP Index
 *        ;固件区index
 */
typedef enum
{
    HPM_APP1   = 0,
    HPM_APP2,
    HPM_APPMAX
} HPM_APP_INDEX;

/**
 * @brief HPM app header structure 
 *        ;头文件数据结构
 */
typedef struct
{
     /** @brief magic information    
     *         ;magic信息 */
    uint32_t magic;
     /** @brief id information    
     *         ;id信息 */
    uint32_t touchid;
     /** @brief device information    
     *         ;设备信息 */
    uint32_t device;
     /** @brief firmware length    
     *         ;固件长度 */
    uint32_t len;
     /** @brief version information    
     *         ;版本信息 */
    uint32_t version;
     /** @brief type of the upgrade content    
     *         ;升级内容类型 */
    uint8_t type;
     /** @brief hash enable switch    
     *         ;hash使能开关 */
    uint8_t hash_enable;
     /** @brief hash start verify switch    
     *         ;启动检验hash开关 */
    uint8_t pwr_hash;
     /** @brief hash type    
     *         ;hash类型 */
    uint8_t hash_type;
     /** @brief reserve    
     *         ;保留 */
    uint32_t reserved;
     /** @brief hash data    
     *         ;hash值 */
    uint8_t hash_data[HPM_APP_HASH_DATA_MAXSIZE];
} hpm_app_header_t;

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_APPHEADER_H