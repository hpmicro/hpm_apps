/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_OTA_H
#define __HPM_OTA_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "hpm_romapi.h"
#include "hpm_appheader.h"
#include "ota_kconfig.h"

/**
 * @brief HPM OTA API;ota接口
 * @addtogroup Hpm_OTA_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief BODY type mask 0~15
 *        ;内容类型掩码
 */
#define OTA_TYPE_BODY_MASK        (0x0F)
#define OTA_TYPE_BODY_SHIFT       (0)
#define OTA_TYPE_BODY_SET(x)      (((uint32_t)(x) << OTA_TYPE_BODY_SHIFT) & OTA_TYPE_BODY_MASK)
#define OTA_TYPE_BODY_GET(x)      (((uint32_t)(x) & OTA_TYPE_BODY_MASK) >> OTA_TYPE_BODY_SHIFT)

/**
 * @brief type RAM mask, 1 is ram, 0 is xip
 *        ;类型RAM掩码，1是ram类型，0是flash_xip类型
 */
#define OTA_TYPE_RAM_MASK         (0x40)
#define OTA_TYPE_RAM_SHIFT        (6)
#define OTA_TYPE_RAM_SET(x)      (((uint32_t)(x) << OTA_TYPE_RAM_SHIFT) & OTA_TYPE_RAM_MASK)
#define OTA_TYPE_RAM_GET(x)      (((uint32_t)(x) & OTA_TYPE_RAM_MASK) >> OTA_TYPE_RAM_SHIFT)

/**
 * @brief type exip mask, 1 is exip enable, 0 is no exip enable
 *        ;类型EXIP 掩码, 1是使能了EXIP启用加密， 0是为启用加密
 */
#define OTA_TYPE_EXIP_MASK        (0x80)
#define OTA_TYPE_EXIP_SHIFT       (7)
#define OTA_TYPE_EXIP_SET(x)      (((uint32_t)(x) << OTA_TYPE_EXIP_SHIFT) & OTA_TYPE_EXIP_MASK)
#define OTA_TYPE_EXIP_GET(x)      (((uint32_t)(x) & OTA_TYPE_EXIP_MASK) >> OTA_TYPE_EXIP_SHIFT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HPM OTA Type, Note: The highest decryption enable bit(0x80)
 *        ;升级内容类型，注意：最高位解密使能位 0x80
 */
typedef enum
{
     /** @brief normal firmware type    
     *         ;正常固件类型 */
    OTA_TYPE_NORMAL_FIRMWARE = 0,

    /**
     * @brief diff firmware type
     *        ;差分 固件类型 */
    OTA_TYPE_DIFF_FIRMWARE,

    /**
     * @brief lzma firmware type
     *        ;压缩 固件类型
     */
    OTA_TYPE_LZMA_FIRMWARE,

    OTA_TYPE_MAX
} HPM_OTA_BODY_TYPE;

#define OTA_RAM_INFI_MAGIC    (0xA55A5AA5)

typedef struct
{
    uint32_t magic;
    uint8_t app;
    uint32_t addr;
    uint32_t reserved;
} ota_ram_info_t;

#define OTA_APP_KEY_MAGIC    (0xFAFABFBF)
#define OTA_APP_KEY_MAXCOUNT (8)

typedef struct
{
    uint32_t magic;
    uint32_t count;
    uint32_t crc32;
    exip_region_param_t exip_region[OTA_APP_KEY_MAXCOUNT];
} ota_app_key_t;

/**
 * @brief Reset OTA
 *        ;ota重置
 */
void hpm_ota_soc_reset(void);


/**
 * @brief IMG max size
 *        ;IMG最大容量
 *
 * @param [in] ota_index ota index ;固件包index
 */
uint32_t hpm_ota_img_maxsize(uint8_t ota_index);

/**
 * @brief hpm ota get now running app index
 *        ;获取当前正在运行的固件区index
 *
 * @return app;固件区index
 */
int hpm_ota_get_nowrunning_app(void);

/**
 * @brief hpm ota get flash header information
 *        ;获取头部信息
 *
 * @param [in] ota_index ota index ;固件区index
 * @return hpm_app_header structure;头部信息结构体
 */
int hpm_ota_get_header_info_of_app(uint8_t ota_index, hpm_app_header_t *app_header);

int hpm_ota_get_header_info_of_addr(uint32_t addr, hpm_app_header_t *app_header);
/**
 * @brief verify the firmware storage area
 *        Note:if flash has only one firmware area,return HPM_APP1
 *        ;校验固件存储区域
 *        注意：flash只一个固件区，返回HPM_APP1。
 *
 * @return ota index;固件区index
 */
uint8_t hpm_ota_check_download_appindex(void);

/**
 * @brief hpm ota auto write api
 *        ;升级包写入falsh
 *
 * @param [in] src package array source ;升级包
 * @param [in] len package array length ;升级包长度
 * @param [in] reset_enable reset enable ;ota传输结束之后ota重置按钮
 * @return ota write status ;ota写入状态，0: ota写入成功, <0: ota写入错误, 1: ota写入成功并传输结束
 */
int hpm_ota_auto_write(void const* src, uint32_t len, bool reset_enable);

/**
 * @brief hpm ota split write api, device msc mode use
 *        ;升级包写入分包写入flash接口，USB device 模式时使用
 *
 * @param [in] src package array source ;升级包
 * @param [in] len package array length ;升级包长度
 * @param [in] reset_enable reset enable ;ota传输结束之后ota重置按钮
 * @return ota write status ;ota写入状态，0: ota写入成功, <0: ota写入错误, 1: ota写入成功并传输结束
 */
int hpm_ota_split_write(void const *src, uint32_t len, bool reset_enable);   

/**
 * @brief hpm ota auto write to certain address
 *        ;升级包写入flash固定地址
 *
 * @param [in] addr write flash addr ;flash地址
 * @param [in] src package array source ;升级包
 * @param [in] len package array length ;升级包长度
 * @param [in] reset_enable reset enable ;ota传输结束之后ota重置按钮
 * @return ota write status ;ota写入状态，0: ota写入成功, <0: ota写入错误, 1: ota写入成功并传输结束.
 */
int hpm_ota_auto_write_of_addr(uint32_t addr, void const *src, uint32_t len, bool reset_enable);

/**
 * @brief hpm ota package verify
 *        Note:
 *        ;存储固件校验
 *
 * @param [in] addr storage firmware address ;存储固件地址
 * @param [in] len  package array length ;存储固件长度
 * @param [in] ota_header header struct ;头部信息结构体
 * @return verify status ;存储固件包校验状态，0: check fail, 1: check success.
 */
bool hpm_ota_package_verify(uint32_t addr, uint32_t len, hpm_app_header_t* ota_header);

/**
 * @brief hpm ota package makehash
 *        Note:
 *        ;存储固件hash生成
 *
 * @param [in] addr storage firmware address ;存储固件地址
 * @param [in] len  package array length ;存储固件长度
 * @param [in,out] ota_header header struct, use hash type, output hash data ;头部信息结构体,根据结构体成员哈希类型，生成哈希值到结构体成员
 * @return make status ;存储固件hash生成结果，0: build fail, 1: build success
 */
bool hpm_ota_package_makehash(uint32_t addr, uint32_t len, hpm_app_header_t *ota_header);

/**
 * @brief hpm app jump
 *        Note:If the appindex is HPM_APP2, perform address offset and mapping first.
 *        ;app跳转
 *        注意：如果跳转固件区是HPM_APP2,需要将HPM_APP1区地址偏移映射至HPM_APP2。
 *
 * @param [in] appindex app index ;固件区index
 */
void hpm_appindex_jump(uint8_t appindex);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_OTA_H