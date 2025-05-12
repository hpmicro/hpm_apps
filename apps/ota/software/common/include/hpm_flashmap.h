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

#ifndef CONFIG_USE_FLASH_1M

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
// / EEPROM emulation / 64K
// /------------------/
// /      USER        / 192k
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

#define FLASH_EEPROM_ADDR        (FLASH_FILESYSTEM_ADDR + FLASH_FILESYSTEM_SIZE)
#define FLASH_EEPROM_SIZE        (0x10000)

#define FLASH_USER_COMMON_ADDR   (FLASH_EEPROM_ADDR + FLASH_EEPROM_SIZE)
#define FLASH_USER_COMMON_SIZE   (0x40000)
#else
//1M
// / ---------------- /
// /    BOOTHEADER    / 12K
// /------------------/
// /    BOOTUSER      / 256K
// /------------------/
// /    APP1 OTA1     / 256M
// /------------------/
// /    APP2 OTA2     / 256MM
// /------------------/
// / EEPROM emulation / 64K
// /------------------/
// /      USER        / 64k
// /------------------/

#define FLASH_ADDR_BASE          (0x80000000)

#define FLASH_START_ADDR         (0)

#define FLASH_MAX_SIZE           (0x100000)

#define FLASH_EXIP_INFO_ADDR     (FLASH_START_ADDR)

#define FLASH_EXIP_INFO_SIZE     (0x400)

#define FLASH_NORCFG_OPTION_ADDR (FLASH_EXIP_INFO_ADDR + FLASH_EXIP_INFO_SIZE)

#define FLASH_NORCFG_OPTION_SIZE (0xC00)

#define FLASH_BOOT_HEADER_ADDR   (FLASH_NORCFG_OPTION_ADDR + FLASH_NORCFG_OPTION_SIZE)
#define FLASH_BOOT_HEADER_SIZE   (0x2000)

#define FLASH_BOOT_USER_ADDR     (FLASH_BOOT_HEADER_ADDR + FLASH_BOOT_HEADER_SIZE) 
#define FLASH_BOOT_USER_SIZE     (0x40000)

#define FLASH_USER_APP1_ADDR     (FLASH_BOOT_USER_ADDR + FLASH_BOOT_USER_SIZE)
#define FLASH_USER_APP1_SIZE     (0x40000)

#define FLASH_USER_APP2_ADDR     (FLASH_USER_APP1_ADDR + FLASH_USER_APP1_SIZE)
#define FLASH_USER_APP2_SIZE     (0x40000)

#define FLASH_PROFILE_ADDR       (FLASH_USER_APP2_ADDR + FLASH_USER_APP2_SIZE)
#define FLASH_PROFILE_SIZE       (0)

#define FLASH_KEYVALUE1_ADDR     (FLASH_PROFILE_ADDR + FLASH_PROFILE_SIZE)
#define FLASH_KEYVALUE1_SIZE     (0)

#define FLASH_KEYVALUE2_ADDR     (FLASH_KEYVALUE1_ADDR + FLASH_KEYVALUE1_SIZE)
#define FLASH_KEYVALUE2_SIZE     (0)

#define FLASH_FILESYSTEM_ADDR    (FLASH_KEYVALUE2_ADDR + FLASH_KEYVALUE2_SIZE)
#define FLASH_FILESYSTEM_SIZE    (0)

#define FLASH_EEPROM_ADDR        (FLASH_FILESYSTEM_ADDR + FLASH_FILESYSTEM_SIZE)
#define FLASH_EEPROM_SIZE        (0x10000)

#define FLASH_USER_COMMON_ADDR   (FLASH_EEPROM_ADDR + FLASH_EEPROM_SIZE)
#define FLASH_USER_COMMON_SIZE   (0x10000)
#endif

#if ((FLASH_USER_COMMON_ADDR + FLASH_USER_COMMON_SIZE) > FLASH_MAX_SIZE)
#error ("flash map error!\r\n")
#endif

/** @} */

#endif //__FLASH_MAP_H