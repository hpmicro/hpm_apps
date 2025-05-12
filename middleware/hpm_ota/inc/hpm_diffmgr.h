/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_DIFFMGR_H
#define __HPM_DIFFMGR_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief HPM DIFF MGR API; 差分升级API
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
 * @brief 用户使用差分升级时唯一需要关心的接口
 * 
 * @param oldimgaddr 设备中执行区代码所在的地址，用户可指定flash执行区的地址，方便算法读出来当前
 *            运行中的代码
 * @param oldimglen 设备中执行区代码的长度，用户可在差分包bin头获取
 * @param diffimgaddr 设备中已经下载的差分包所在的flash地址，或者ram地址，只要能让算法读出来即可
 *              注意，下载的差分包自带image_header_t格式的文件头，真正的差分包需要偏
 *              移sizeof(image_header_t)的长度
 * @param diffimglen 设备中已经下载的差分包的长度，用户可在差分包bin头获取
 * @param newimglen 新文件的大小，用户需填入新版本bin的长度，用户亦可以差分包bin头获取
 * @return int <0 失败，>= 0 成功
 */
int hpm_diff_upgrade_restore(const uint32_t oldimgaddr, uint32_t oldimglen, const uint32_t diffimgaddr, uint32_t diffimglen, uint32_t newimglen);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_OTA_H