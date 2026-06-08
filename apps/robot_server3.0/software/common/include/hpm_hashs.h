/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_HASHS_H
#define __HPM_HASHS_H

#include <stdint.h>

/**
 * @brief Hpm hashs API;hpm_hashs接口
 * @addtogroup Hpm_hashs_API
 * @{
 *
 */

/**
 * @brief Maximum size of the hash array
 *        ;hash表最大长度
 */
#define HASH_DATA_MAXSIZE (32)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HPM HASH Type
 *        ;hash类型
 */
typedef enum
{
    HASH_TYPE_CHECKSUM = 0,
    HASH_TYPE_XOR,
    HASH_TYPE_CRC32,
    HASH_TYPE_SHA1,
    HASH_TYPE_SHA256,
    HASH_TYPE_SM3,
    HASH_TYPE_MAX
} HPM_HASH_TYPE;

/**
 * @brief hpm hash init
 *        ;hash初始化
 * 
 * @param [in] type hash type ;hash类型
 * @return init status;hash初始化状态，0：success,<0: fail
 */
int hpm_hash_init(uint8_t type);

/**
 * @brief hpm hash update
 *        ;hash值更新
 * 
 * @param [in] buffer firmware address ;固件地址
 * @param [in] len  firmware;固件长度
 * @return update status;hash更新状态，0：success,<0: fail
 */
int hpm_hash_update(uint8_t *buffer, uint32_t len);

/**
 * @brief hpm hash finialize
 *        ;hash结束API
 * 
 * @return hash data;hash值
 */
uint8_t *hpm_hash_finsh(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_HASHS_H