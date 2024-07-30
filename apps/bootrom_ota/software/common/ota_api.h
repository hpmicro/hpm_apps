#ifndef __OTA_API_H
#define __OTA_API_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "board.h"
#include "flash_map.h"

/**
 *
 * @brief bootrom ota APIs; bootrom ota 接口
 * @defgroup bootrom ota APIs
 * @{
 */

#define USER_UPGREAD_FLAG_MAGIC   (0xbeaf5aa5)
#define BOARD_DEVICE_ID           (0x6750)

/**
 * @brief Automatically configure XPI NOR based on cfg_option; 根据 cfg_option 自动配置 XIP NOR
 *
 */
void ota_board_flash_init(void);

/**
 * @brief flash auto check image and download; flash 自动校验镜像和下载镜像
 *
 * @param [in] src buffer pointer; 数据存放的地址
 * @param [in] len buffer size; 数据长度
 */
bool ota_board_auto_write(void const* src, uint32_t len);

/**
 * @brief check firmware checksum and then reset system;校验下载固件的checksum 并复位系统
 *
 */
void ota_board_complete_reset(void);

/**
 * @brief get current ota flash index;获取当前 ota 固件 index
 *
 * @return 0 for flash image0 and 1 for flash image1;如果是0 表示启动镜像0，1表示启动镜像1
 */
uint8_t ota_check_current_otaindex(void);

/**
 * @}
 */
#endif //__OTA_API_H