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
#include "board.h"
#include "hpm_flashmap.h"
#include "hpm_hashs.h"
#include "hpm_appheader.h"

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HPM OTA Type
 *        ;升级内容类型
 */
typedef enum
{
     /** @brief firmware type    
     *         ;固件类型 */
    OTA_TYPE_FIRMWARE = 0,
     /** @brief profile type    
     *         ;配置文件 */
    OTA_TYPE_PROFILE,
    OTA_TYPE_MAX
} HPM_OTA_TYPE;

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
 *        Note: this api must app1 or app2 running use, bootuser use return HPM_APP1
 *        ;获取当前正在运行的固件区index
 *        注意：该函数必须在app1或者app2运行时使用，使用bootuser下载升级时返回HPM_APP1
 *
 * @return appindex;固件区index
 */
uint8_t hpm_ota_get_nowrunning_appindex(void);

/**
 * @brief hpm ota get flash header information
 *        ;获取头部信息
 *
 * @param [in] ota_index ota index ;固件区index
 * @return hpm_app_header structure;头部信息结构体
 */
hpm_app_header_t* hpm_ota_get_flash_header_info(uint8_t ota_index);

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