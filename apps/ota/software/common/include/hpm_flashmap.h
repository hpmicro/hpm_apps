/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_FLASHMAP_H
#define __HPM_FLASHMAP_H

/**
 * @brief HPM FLASH MAP;flash分区
 * @addtogroup Hpm_FLASH_MAP
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief flash sector size
 *        ;切片大小
 */
#define FLASH_SECTOR_SIZE        (0x1000)  

#ifndef CONFIG_USE_HPM5300

// 考虑到内置的flash均为4M， 通用以4M Flash分区
// / ---------------- /
// /    BOOTHEADER    / 12K
// /------------------/
// /    BOOTUSER      / 256K
// /------------------/
// /    APP1 OTA1     / 1M
// /------------------/
// /    APP2 OTA2     / 1M
// /------------------/
// /    PROFILE(XIP)  / 64K
// /------------------/
// /  PSM1 / EASY(KV) / 64K
// /------------------/
// /  PSM2 / EASY(KV) / 64K
// /------------------/
// /     FAFTS        / 1M
// /------------------/
// /      USER        / 256k
// /------------------/

#define FLASH_ADDR_BASE          (0x80000000)
#define FLASH_START_ADDR         (0)

#define FLASH_MAX_SIZE           (0x400000)

#define FLASH_EXIP_INFO_ADDR     (FLASH_START_ADDR)
#define FLASH_EXIP_INFO_SIZE     (0x400)

#define FLASH_NORCFG_OPTION_ADDR (FLASH_EXIP_INFO_ADDR + FLASH_EXIP_INFO_SIZE)
#define FLASH_NORCFG_OPTION_SIZE (0xC00)

#define FLASH_BOOT_HEADER_ADDR   (FLASH_NORCFG_OPTION_ADDR + FLASH_NORCFG_OPTION_SIZE)
#define FLASH_BOOT_HEADER_SIZE   (0x2000)

#define FLASH_BOOT_USER_ADDR     (FLASH_BOOT_HEADER_ADDR + FLASH_BOOT_HEADER_SIZE) 
#define FLASH_BOOT_USER_SIZE     (0x40000)

#define FLASH_USER_APP1_ADDR     (FLASH_BOOT_USER_ADDR + FLASH_BOOT_USER_SIZE)
#define FLASH_USER_APP1_SIZE     (0x100000)

#define FLASH_USER_APP2_ADDR     (FLASH_USER_APP1_ADDR + FLASH_USER_APP1_SIZE)
#define FLASH_USER_APP2_SIZE     (0x100000)

#define FLASH_PROFILE_ADDR       (FLASH_USER_APP2_ADDR + FLASH_USER_APP2_SIZE)
#define FLASH_PROFILE_SIZE       (0x10000)

#define FLASH_KEYVALUE1_ADDR     (FLASH_PROFILE_ADDR + FLASH_PROFILE_SIZE)
#define FLASH_KEYVALUE1_SIZE     (0x10000)

#define FLASH_KEYVALUE2_ADDR     (FLASH_KEYVALUE1_ADDR + FLASH_KEYVALUE1_SIZE)
#define FLASH_KEYVALUE2_SIZE     (0x10000)

#define FLASH_FILESYSTEM_ADDR    (FLASH_KEYVALUE2_ADDR + FLASH_KEYVALUE2_SIZE)
#define FLASH_FILESYSTEM_SIZE    (0x100000)

#define FLASH_USER_COMMON_ADDR   (FLASH_FILESYSTEM_ADDR + FLASH_FILESYSTEM_SIZE)
#define FLASH_USER_COMMON_SIZE   (0x40000)
#else
//1M
// / ---------------- /
// /    BOOTHEADER    / 12K
// /------------------/
// /    BOOTUSER      / 256K
// /------------------/
// /    APP1 OTA1     / 360K
// /------------------/
// /     FAFTS        / 360K
// /------------------/
// /      USER        / 8K
// /------------------/

/**
 * @brief flash  address base
 *        ;flash基地址
 */
#define FLASH_ADDR_BASE          (0x80000000)

/**
 * @brief flash divide start address
 *        ;flash分区起始地址
 */
#define FLASH_START_ADDR         (0)

/**
 * @brief flash max size
 *        ;flash最大size
 */
#define FLASH_MAX_SIZE           (0x100000)

/**
 * @brief exip information address
 *        ;exip信息配置地址
 */
#define FLASH_EXIP_INFO_ADDR     (FLASH_START_ADDR)

/**
 * @brief exip information size
 *        ;exip信息size
 */
#define FLASH_EXIP_INFO_SIZE     (0x400)

/**
 * @brief nor falsh cfg address
 *        ;nor flash配置信息地址
 */
#define FLASH_NORCFG_OPTION_ADDR (FLASH_EXIP_INFO_ADDR + FLASH_EXIP_INFO_SIZE)

/**
 * @brief nor falsh  information size
 *        ;nor flash信息size
 */
#define FLASH_NORCFG_OPTION_SIZE (0xC00)

/**
 * @brief header information cfg address
 *        ;头文件信息配置地址
 */
#define FLASH_BOOT_HEADER_ADDR   (FLASH_NORCFG_OPTION_ADDR + FLASH_NORCFG_OPTION_SIZE)

/**
 * @brief header information size
 *        ;头文件信息size
 */
#define FLASH_BOOT_HEADER_SIZE   (0x2000)

/**
 * @brief boot user information address
 *        ;user区地址
 */
#define FLASH_BOOT_USER_ADDR     (FLASH_BOOT_HEADER_ADDR + FLASH_BOOT_HEADER_SIZE) 

/**
 * @brief boot user information size
 *        ;user区size
 */
#define FLASH_BOOT_USER_SIZE     (0x40000)

/**
 * @brief user app1 address
 *        ;app1区address
 */
#define FLASH_USER_APP1_ADDR     (FLASH_BOOT_USER_ADDR + FLASH_BOOT_USER_SIZE)

/**
 * @brief user app1 size
 *        ;app1区size
 */
#define FLASH_USER_APP1_SIZE     (0x5A000)

/**
 * @brief  profile address
 *        ;配置文件地址
 */
#define FLASH_PROFILE_ADDR       (FLASH_USER_APP1_ADDR + FLASH_USER_APP1_SIZE)

/**
 * @brief  profile size
 *        ;配置文件size
 */
#define FLASH_PROFILE_SIZE       (0)

/**
 * @brief  PSM1 address
 *        ;PSM1配置地址
 */
#define FLASH_KEYVALUE1_ADDR     (FLASH_PROFILE_ADDR + FLASH_PROFILE_SIZE)

/**
 * @brief  PSM1 size
 *        ;PSM1配置内存大小
 */
#define FLASH_KEYVALUE1_SIZE     (0)

/**
 * @brief  PSM2 address
 *        ;PSM2配置地址
 */
#define FLASH_KEYVALUE2_ADDR     (FLASH_KEYVALUE1_ADDR + FLASH_KEYVALUE1_SIZE)

/**
 * @brief  PSM2 size
 *        ;PSM2配置内存大小
 */
#define FLASH_KEYVALUE2_SIZE     (0)

/**
 * @brief  system file cfg address
 *        ;系统文件配置地址
 */
#define FLASH_FILESYSTEM_ADDR    (FLASH_KEYVALUE2_ADDR + FLASH_KEYVALUE2_SIZE)

/**
 * @brief  system file cfg size
 *        ;系统文件配置size
 */
#define FLASH_FILESYSTEM_SIZE    (0x5A000)

/**
 * @brief  user common cfg address
 *        ;user区配置地址
 */
#define FLASH_USER_COMMON_ADDR   (FLASH_FILESYSTEM_ADDR + FLASH_FILESYSTEM_SIZE)

/**
 * @brief  user common size
 *        ;user区配置size
 */
#define FLASH_USER_COMMON_SIZE   (0x2000)
#endif

#if ((FLASH_USER_COMMON_ADDR + FLASH_USER_COMMON_SIZE) > FLASH_MAX_SIZE)
#error ("flash map error!\r\n")
#endif

/** @} */

#endif //__FLASH_MAP_H