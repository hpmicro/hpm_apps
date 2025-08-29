/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_DFLASHMAP_H
#define __HPM_DFLASHMAP_H

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

/**
 * @brief // Considering that the built-in flash is 4M, it is common to use a 4M flash partition, 
 *           except for the BOOTHEADER 12K, which cannot be adjusted, the other partition sizes can be adjusted at will.
 *           ;考虑到内置的flash均为4M， 通用以4M Flash分区, 除BOOTHEADER 12K 不可调整，其它分区大小可随意调节
 */
// / ---------------- /
// /    BOOTHEADER    / 12K 0x3000
// /------------------/
// /    BOOTUSER      / 256K
// /------------------/
// /    USERKEY       / 4K
// /------------------/
// /    APP1IMG       / 1M
// /------------------/
// /    APP2IMG/LZMA  / 1M
// /------------------/
// /    DIFFFILE      / 512k
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

#define FLASH_USER_KEY_ADDR      (FLASH_BOOT_USER_ADDR + FLASH_BOOT_USER_SIZE)
#define FLASH_USER_KEY_SIZE      (0x1000)

#define FLASH_USER_APP1_ADDR     (FLASH_USER_KEY_ADDR + FLASH_USER_KEY_SIZE)
#define FLASH_USER_APP1_SIZE     (0x100000)

#define FLASH_USER_APP2_ADDR     (FLASH_USER_APP1_ADDR + FLASH_USER_APP1_SIZE)
#define FLASH_USER_APP2_SIZE     (0x100000)

#define FLASH_DIFF_APP_ADDR     (FLASH_USER_APP2_ADDR + FLASH_USER_APP2_SIZE)
#define FLASH_DIFF_APP_SIZE      (0x080000)   //0.5M

#define FLASH_EEPROM_ADDR        (FLASH_DIFF_APP_ADDR + FLASH_DIFF_APP_SIZE)
#define FLASH_EEPROM_SIZE        (0x10000)

#define FLASH_USER_COMMON_ADDR   (FLASH_DIFF_APP_ADDR + FLASH_DIFF_APP_SIZE)
#define FLASH_USER_COMMON_SIZE   (0x30000)

#else

/**
 * @brief // it is common to use a 1M flash partition, 
 *           except for the BOOTHEADER 12K, which cannot be adjusted, the other partition sizes can be adjusted at will.
 *           ;以1M Flash分区, 除BOOTHEADER 12K 不可调整，其它分区大小可随意调节
 */
// / ---------------- /
// /    BOOTHEADER    / 12K
// /------------------/
// /    BOOTUSER      / 256K
// /------------------/
// /    USERKEY       / 4K
// /------------------/
// /    APP1IMG       / 256K
// /------------------/
// /    APP2IMG/LZMA  / 256k
// /------------------/
// /    DIFFFILE      / 100k
// /------------------/
// /      USER        / 8k
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

#define FLASH_USER_KEY_ADDR      (FLASH_BOOT_USER_ADDR + FLASH_BOOT_USER_SIZE)
#define FLASH_USER_KEY_SIZE      (0x1000)

#define FLASH_USER_APP1_ADDR     (FLASH_USER_KEY_ADDR + FLASH_USER_KEY_SIZE)
#define FLASH_USER_APP1_SIZE     (0x40000)

#define FLASH_USER_APP2_ADDR     (FLASH_USER_APP1_ADDR + FLASH_USER_APP1_SIZE)
#define FLASH_USER_APP2_SIZE     (0x40000)

#define FLASH_DIFF_APP_ADDR     (FLASH_USER_APP2_ADDR + FLASH_USER_APP2_SIZE)
#define FLASH_DIFF_APP_SIZE      (0x19000)

#define FLASH_USER_COMMON_ADDR   (FLASH_DIFF_APP_ADDR + FLASH_DIFF_APP_SIZE)
#define FLASH_USER_COMMON_SIZE   (0x1000)

#endif

#if ((FLASH_USER_COMMON_ADDR + FLASH_USER_COMMON_SIZE) > FLASH_MAX_SIZE)
#error ("flash map error!\r\n")
#endif

/** @} */

#endif //__FLASH_MAP_H