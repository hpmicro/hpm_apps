/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "ecat_foe_support.h"

#include "hpm_ota.h"
#include "ota_kconfig.h"

#define DISABLE_GLOBAL_INTERRUPT() disable_global_irq(CSR_MSTATUS_MIE_MASK)
#define ENABLE_GLOBAL_INTERRUPT() enable_global_irq(CSR_MSTATUS_MIE_MASK)

bool foe_reset_request;

extern uint32_t (*foe_get_file_addr)(uint16_t MBXMEM * name, uint16_t name_size);
extern uint32_t (*foe_get_file_size)(uint16_t MBXMEM * name, uint16_t name_size);
extern int8_t (*foe_prepare_data_space)(uint16_t MBXMEM * name, uint16_t name_size);
extern int8_t (*foe_store_data)(uint32_t offset, uint16_t MBXMEM * data, uint16_t size, uint8_t data_following);

uint32_t foe_support_get_file_addr(uint16_t MBXMEM * name, uint16_t name_size)
{
    (void) name;
    (void) name_size;

    uint32_t addr = 0;
    uint8_t app_index = hpm_ota_get_nowrunning_app();
    if (app_index == HPM_APP2) {
        if (rom_xpi_nor_is_remap_enabled(HPM_XPI0)) {
            addr = (FLASH_ADDR_BASE + FLASH_USER_APP1_ADDR);
        } else {
            addr = (FLASH_ADDR_BASE + FLASH_USER_APP2_ADDR);
        }
    } else {
        addr = (FLASH_ADDR_BASE + FLASH_USER_APP1_ADDR);
    }

    return addr;
}

uint32_t foe_support_get_file_size(uint16_t MBXMEM * name, uint16_t name_size)
{
    (void) name;
    (void) name_size;

    hpm_app_header_t app_header = {0};
    uint8_t app_index = hpm_ota_get_nowrunning_app();
    hpm_ota_get_header_info_of_app(app_index, &app_header);
    uint32_t size = app_header.len + sizeof(hpm_app_header_t); /* get file size from app header in flash */

    if (size > FLASH_FILE_SIZE) {
        return 0;
    } else {
        return size;
    }

    return 0;
}

int8_t foe_support_prepare_data_space(uint16_t MBXMEM * name, uint16_t name_size)
{
    (void) name;
    (void) name_size;

    /* erase flash */
    /* do flash erase in hpm_ota_auto_write_of_addr() */

    return 0;
}

int8_t foe_support_store_data(uint32_t offset, uint16_t MBXMEM * data, uint16_t size, uint8_t data_following)
{
    (void) data_following;
    (void) offset;

    if (size == 0) {
        return 0;
    } 

    return hpm_ota_auto_write((void*)data, size, false);
}

void foe_support_soc_reset(void)
{
    hpm_ota_soc_reset();
}

int8_t foe_support_init(void)
{
    int ret = hpm_flash_init(); /* flash driver init */
    if (ret != 0) {
        printf("Flash init failed!!\n");
        return -1;
    }

    foe_get_file_addr = foe_support_get_file_addr;
    foe_get_file_size = foe_support_get_file_size;
    foe_prepare_data_space = foe_support_prepare_data_space;
    foe_store_data = foe_support_store_data;

    return 0;
}

