/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef ECAT_CONFIG_H
#define ECAT_CONFIG_H

#include "board.h"
#include "ota_kconfig.h"

/* Flash space allocation
* / ---------------- /
* /    APP           / 2M
* /------------------/
* / EEPROM emulation / 64K
* /------------------/
* /     FOE File     / 8K
* /------------------/
*/

#define FLASH_FILE_SIZE FLASH_USER_APP1_SIZE

/* ecat timer */
#define ECAT_TIMER_GPTMR        HPM_GPTMR0
#define ECAT_TIMER_GPTMR_CH     (0U)
#define ECAT_TIMER_GPTMR_CLK    (clock_gptmr0)
#define ECAT_TIMER_GPTRM_IRQ    IRQn_GPTMR0

/* ecat flash emulate eeprom function */
#define ECAT_EEPROM_FLASH_OFFSET      (FLASH_EEPROM_ADDR)
#define ECAT_EEPROM_FLASH_SECTOR_SIZE (FLASH_SECTOR_SIZE)
#define ECAT_EEPROM_FLASH_SECTOR_CNT  (FLASH_EEPROM_SIZE / FLASH_SECTOR_SIZE) /* suitable for 2K bytes eeprom space */

/* check Product Code and Revision Number in EEPROM, if not match, will update eeprom data by included data in eeprom.h */
#define ECAT_EEPROM_CHECK_PRODUCT_CODE_AND_REVISION      (1)

/* EEPROM size in bytes, indicate the size of the actual connected eeprom */
#define ECAT_EEPROM_SIZE_BYTE         (16 * 1024)  /* 16K bytes = 128K bits */

/* ecat reset function */
#define ECAT_RESET_ESC_PERIPHERAL  0    /* ESC reset request will reset ethercat slave controller only, not suggest */
#define ECAT_RESET_ESC_WITH_MCU    1    /* ESC reset request will reset ethercat slave controller and mcu */


#endif /* ECAT_CONFIG_H */