/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_GROUPSPLIT_H
#define __HPM_GROUPSPLIT_H

#include <stdint.h>

/**
 * @brief HPM group split API;切片处理对应接口
 * @addtogroup Hpm_groupsplit_API
 * @{
 *
 */

/**
 * @brief sector size
 *        ;切片后每片大小
 */
#define GROUPSPLIT_BLOCKSIZE           (512)

/**
 * @brief sector magic0 info
 *        ;切片magic0信息
 */
#define GROUPSPLIT_MAGIC_0    0x48504D2DUL 

/**
 * @brief sector magic1 info
 *        ;切片magic1信息
 */
#define GROUPSPLIT_MAGIC_1    0x9E5D5157UL 

/**
 * @brief sector end info
 *        ;切片end信息
 */
#define GROUPSPLIT_END       0x48454E44UL 

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sector package structure 
 *        ;切片包信息结构体
 */
typedef struct
{
     /** @brief magic0 information    
     *         ;magic0信息 */
    uint32_t magic0;
     /** @brief magic1 information    
     *         ;magic1信息 */
    uint32_t magic1;
    /**
     * @brief file type
     *        ;文件类型
     */
    uint8_t type;
    /**
     * @brief reserved
     *        ;预留
     */
    uint8_t reserved[3];

    /**
     * @brief file version
     *        ;文件版本
     */
    uint32_t file_version;
    /**
     * @brief file total len
     *        ;文件总大小
     */
    uint32_t total_len;
     /** @brief sector index    
     *         ;切片index */
    uint32_t index;
     /** @brief data size    
     *         ;data大小 */
    uint32_t datasize;
     /** @brief data   
     *         ;data内容 */
    uint8_t data[480];
     /** @brief end info    
     *         ;end信息 */
    uint32_t magicend;
} __attribute__((packed)) hpm_groupsplit_pack_t;

/**
 * @brief secotr package judge
 *        ;判断是否是切片包
 *
 * @param [in] bl sector package struct;切片包结构体
 * @return groupsplit block;是否为切片包，0: 固件不是切片包格式,  1: 固件是切片包格式.
 */
static inline bool is_hpm_groupsplit_block (hpm_groupsplit_pack_t const *bl)
{
    return ((bl->magic0 == GROUPSPLIT_MAGIC_0) &&
        (bl->magic1 == GROUPSPLIT_MAGIC_1) &&
        (bl->magicend == GROUPSPLIT_END));
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_GROUPSPLIT_H