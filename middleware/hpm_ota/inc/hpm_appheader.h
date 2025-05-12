/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_APPHEADER_H
#define __HPM_APPHEADER_H

#include <stdint.h>

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
 * @brief app header size
 *        ; app 头部长度
*/
#define HPM_APP_HEADER_SIZE       (0x1000)

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

/**
 * @brief Diff Magic information
 *        ; 差分magic 信息
 */
#define HPM_APP_DIFF_FLAG_MAGIC   (0xbeafa5a5)

/**
 * @brief lzma magic
 *
 */
#define HPM_APP_LZMA_FLAG_MAGIC   (0xbefaa55a)

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
 * @brief diff image header information
 *        ;差分固件头部信息
 */
#pragma pack(1)
typedef struct
{
    
    /**
     * @brief diff magic informatcion
     *        ;差分mgic信息
     */
    uint32_t magic;
    /**
     * @brief copression type
     *        ;压缩类型
     */
    uint8_t comp_type;

    /** @brief reserve    
     *         ;保留 */
    uint8_t reserved;
    /**
     * @brief old image version information
     *        ;旧固件的版本信息
     */
    uint32_t old_version;
    /**
     * @brief old image length
     *        ;旧固件的长度
     */
    uint32_t old_img_len;
    /** @brief old image hash type    
    *         ;旧固件hash类型 */
    uint8_t old_hash_type;
    /** @brief old image hash data    
    *         ;旧固件hash值 */
    uint8_t old_hash_data[HPM_APP_HASH_DATA_MAXSIZE];

    /**
     * @brief new image version information
     *        ;新固件的版本信息
     */
    uint32_t new_version;
    /**
     * @brief new iamge length
     *        ;新固件的长度
     */
    uint32_t new_img_len;
    /**
     * @brief new image hash type
     *        ;新固件hash类型
     */
    uint8_t new_hash_type;
    /**
     * @brief new image hash data
     *        ;新固件hash值
     */
    uint8_t new_hash_data[HPM_APP_HASH_DATA_MAXSIZE];
} diff_image_header_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    
    /**
     * @brief diff magic informatcion
     *        ;差分mgic信息
     */
    uint32_t magic;
    /**
     * @brief copression type
     *        ;压缩类型
     */
    uint8_t comp_type;

    /** @brief reserve    
     *         ;保留 */
    uint8_t reserved;
    /**
     * @brief image version information
     *        ;旧固件的版本信息
     */
    uint32_t version;
    /**
     * @brief image length
     *        ;旧固件的长度
     */
    uint32_t img_len;
    /** @brief old image hash type    
    *         ;旧固件hash类型 */
    uint8_t hash_type;
    /** @brief old image hash data    
    *         ;旧固件hash值 */
    uint8_t hash_data[HPM_APP_HASH_DATA_MAXSIZE];
} lzma_image_header_t;
#pragma pack()

/**
 * @brief HPM app header structure 
 *        ;头文件数据结构
 */
#pragma pack(1)
typedef union
{
    struct {
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
        /** @brief run start addr, ram build use
         *         ;运行的起始地址，当RAM构建类型使用 */
        uint32_t start_addr;
        /** @brief hash data    
         *         ;hash值 */
        uint8_t hash_data[HPM_APP_HASH_DATA_MAXSIZE];

        union{
            /**
             * @brief diff image header information
             *        ;差分固件header信息
             */
            diff_image_header_t diff_header;


            /**
             * @brief lzma image header information
             *        ;lzma压缩固件header信息
             */
            lzma_image_header_t lzma_header;
        };
    };
    uint8_t buffer[HPM_APP_HEADER_SIZE];
} hpm_app_header_t;
#pragma pack()

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_APPHEADER_H