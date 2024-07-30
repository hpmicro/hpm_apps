/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "ota_api.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "hpm_l1c_drv.h"

#define SECTOR_SIZE (4 * 1024)
#define BLOCK_SIZE  (64 * 1024)

typedef struct
{
    uint32_t magic;
    uint32_t device;
    uint32_t len;
    uint32_t checksum;
} user_fota_header_t;

static uint32_t current_addr = 0;
static uint8_t sector_buffer[SECTOR_SIZE];
static xpi_nor_config_t s_xpi_nor_config;
static user_fota_header_t current_fota_header;

ATTR_RAMFUNC
void ota_board_flash_init(void)
{
    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
}

#define SOC_ROM_API_ARG_REG (*(volatile uint32_t *)(0xF40D4000UL + 0U))
#define SOC_XPI_PERSIST_REG (*(volatile uint32_t *)(0xF40D4000UL + 4U))
#define SOC_ROM_FLAG_REG    (*(volatile uint32_t *)(0xF40D4000UL + 8U))

static void soc_reset(void)
{
    SOC_ROM_FLAG_REG = 0;                            //warning: this is rom reset flag,
    HPM_PPOR->RESET_ENABLE |= HPM_BITSMASK(1UL, 31); // Enable Software reset   HPM_PPOR->RESET_HOT &= ~HPM_BITSMASK(1UL, 31);
    HPM_PPOR->RESET_COLD |= HPM_BITSMASK(1UL, 31);
    HPM_PPOR->RESET_HOT = 0;
    HPM_PPOR->SOFTWARE_RESET = 1000U;
    while (1) {
    }
}

uint32_t ota_board_flash_size(uint8_t ota_index)
{
    return ota_index == 0 ? FLASH_APP1_ALL_IMG_SIZE : FLASH_APP2_ALL_IMG_SIZE;
}

void ota_board_flash_erase(uint8_t ota_index)
{
    hpm_stat_t status;
    uint32_t addr, user_size, start_index, erase_count = 0;
    if (ota_index == 0) {
        addr = FLASH_APP1_ALL_IMG_ADDR - FLASH_ADDR_BASE;
        user_size = FLASH_APP1_ALL_IMG_SIZE;
    } else if (ota_index == 1) {
        addr = FLASH_APP2_ALL_IMG_ADDR - FLASH_ADDR_BASE;
        user_size = FLASH_APP2_ALL_IMG_SIZE;
    } else {
        return;
    }

    if (!(addr % BLOCK_SIZE) && !(user_size % BLOCK_SIZE)) {
        printf("erase block mode!\n");
        erase_count = addr + user_size;
        disable_global_irq(CSR_MSTATUS_MIE_MASK);
        for (start_index = addr; start_index < erase_count; start_index += BLOCK_SIZE) {
            status = rom_xpi_nor_erase_block(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, start_index);
            if (status != status_success) {
                printf("ERROR:erase block failed:%d\n", start_index);
                break;
            }
        }
        enable_global_irq(CSR_MSTATUS_MIE_MASK);
        printf("erase block success!\n");
    } else if (!(addr % SECTOR_SIZE) && !(user_size % SECTOR_SIZE)) {
        printf("erase sector mode!\n");
        erase_count = addr + user_size;
        printf("start_index:%d, erase_count:%d\n", start_index, erase_count);
        disable_global_irq(CSR_MSTATUS_MIE_MASK);
        for (start_index = addr; start_index < erase_count; start_index += SECTOR_SIZE) {
            status = rom_xpi_nor_erase_sector(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, start_index);
            if (status != status_success) {
                printf("ERROR:erase sector failed:%d\n", start_index);
                break;
            }
        }
        enable_global_irq(CSR_MSTATUS_MIE_MASK);
        printf("erase sector success!\n");
    } else {
        printf("ERROR: flase erase not algin!\n");
        return;
    }
}

static void ota_board_flash_write(uint32_t addr, void const *src, uint32_t len)
{
    hpm_stat_t status;
    uint32_t write_addr = addr - FLASH_ADDR_BASE;
    static uint32_t erase_addr = 0;
    if (len > SECTOR_SIZE) {
        printf("ERROR: write size overflow!\n");
        return;
    }
    memcpy(sector_buffer, src, len);
    if (erase_addr <= (write_addr + len)) {
        if (erase_addr == 0 && !(write_addr % SECTOR_SIZE)) {
            erase_addr = write_addr;
        }
        // printf("write addr:0x%08x, erase_addr:0x%08x\n", write_addr, erase_addr);
        disable_global_irq(CSR_MSTATUS_MIE_MASK);
        status = rom_xpi_nor_erase_sector(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, erase_addr);
        enable_global_irq(CSR_MSTATUS_MIE_MASK);
        if (status != status_success) {
            printf("ERROR:erase sector failed, addr:0x%x\n", erase_addr);
            return;
        }
        erase_addr += SECTOR_SIZE;
    }

    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
                                 (uint32_t *)sector_buffer, write_addr, len);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (status != status_success) {
        printf("ERROR:program failed: status = %ld!\r\n", status);
        return;
    }
    // printf("write success!\n");
}

static void ota_board_header_write(uint8_t ota_index, user_fota_header_t *fota_header)
{
    hpm_stat_t status;
    uint32_t write_addr;

    if (fota_header == NULL || sizeof(user_fota_header_t) > SECTOR_SIZE)
        return;
    if (ota_index == 0)
        write_addr = FLASH_APP1_IMGINFO_ADDR - FLASH_ADDR_BASE;
    else if (ota_index == 1)
        write_addr = FLASH_APP2_IMGINFO_ADDR - FLASH_ADDR_BASE;
    else
        return;

    if (write_addr % SECTOR_SIZE)
        return;

    printf("header write addr:0x%08x\n", write_addr);
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    status = rom_xpi_nor_erase_sector(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, write_addr);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (status != status_success) {
        printf("ERROR:erase sector failed, addr:0x%x\n", write_addr);
        return;
    }

    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
                                 (uint32_t *)fota_header, write_addr, sizeof(user_fota_header_t));
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (status != status_success) {
        printf("ERROR:program failed: status = %ld!\r\n", status);
        return;
    }
    printf("write success!\n");
}

static int ota_fota_flash_checksum(uint32_t addr, uint32_t len, uint32_t *checksum)
{
    hpm_stat_t status;
    int i;
    int allsize = 0;
    int read_len;
    unsigned int tmp;
    unsigned char buf[512];

    *checksum = 0;
    while (1) {
        if (len - allsize > sizeof(buf))
            read_len = sizeof(buf);
        else
            read_len = len - allsize;

        status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)buf,
                                  addr + allsize - FLASH_ADDR_BASE, read_len);
        if (status != status_success) {
            printf("flash read fail\r\n");
            return -1;
        }
        for (i = 0; i < read_len; i++) {
            tmp = buf[i];
            *checksum += tmp;
        }
        allsize += read_len;

        if (allsize >= len)
            return 0;
    }
    return -1;
}

bool ota_board_auto_checksum(void)
{
    int ret;
    uint32_t checksum;
    //checksum
    ret = ota_fota_flash_checksum(current_addr, current_fota_header.len, &checksum);
    printf("current.checksum:0x%08x, checksum:0x%08x\n", current_fota_header.checksum, checksum);
    if (ret != 0 || current_fota_header.checksum != checksum) {
        printf("checksum failed, ota fail!\n");
        current_addr = 0;
        return false;
    }
    return true;
}

bool ota_board_auto_write(void const *src, uint32_t len)
{
    static uint32_t offset = 0;
    uint8_t ota_index;

    user_fota_header_t *current_header = (user_fota_header_t *)src;
    if (current_header->magic == USER_UPGREAD_FLAG_MAGIC &&
        current_header->device == BOARD_DEVICE_ID) {
        ota_index = ota_check_current_otaindex();
        if (ota_index == 1) {
            memcpy(&current_fota_header, current_header, sizeof(user_fota_header_t));
            // printf("ota1, length:%d,checksem:0x%08x\n", current_header->len, current_header->checksum);
            ota_board_header_write(0, &current_fota_header);
            current_addr = FLASH_APP1_ALL_IMG_ADDR;
            offset = 0;
            src = src + sizeof(user_fota_header_t);
            len -= sizeof(user_fota_header_t);
            //printf("ota data download...\n");
        } else if (ota_index == 0) {
            memcpy(&current_fota_header, current_header, sizeof(user_fota_header_t));
            ota_board_header_write(1, &current_fota_header);
            current_addr = FLASH_APP2_ALL_IMG_ADDR;
            offset = 0;
            src = src + sizeof(user_fota_header_t);
            len -= sizeof(user_fota_header_t);
            //printf("ota data download...\n");
        }
    }

    if (current_addr != 0) {
        ota_board_flash_write(current_addr + offset, src, len);
        offset += len;
        if (offset >= current_fota_header.len) {
            //complete checksum and reset
            //ota_board_complete_reset();
            offset = 0;
            return true;
        }
    }
    return false;
}

void ota_board_complete_reset(void)
{
    if (current_addr != FLASH_APP1_ALL_IMG_ADDR && current_addr != FLASH_APP2_ALL_IMG_ADDR)
        return;

    if (!ota_board_auto_checksum())
        return;

    current_addr = 0;
    printf("ota success!\n");
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    soc_reset();
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
}

uint8_t ota_check_current_otaindex(void)
{
    hpm_stat_t status;
    uint32_t version = 0;

    uint16_t sw_version = *((uint16_t *)(FLASH_APP1_BOOT_HEAD_ADDR + FW_HEADER_SW_VERSION_OFFSET));
    uint16_t fw_version1, fw_version2;

    printf("sw_version:%d\n", sw_version);

    status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, &version,
                              FLASH_APP1_BOOT_HEAD_ADDR + FW_HEADER_SW_VERSION_OFFSET - FLASH_ADDR_BASE, 2);
    if (status != status_success) {
        printf("flash read fail\r\n");
        return -1;
    }
    fw_version1 = (uint16_t)version;

    status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, &version,
                              FLASH_APP2_BOOT_HEAD_ADDR + FW_HEADER_SW_VERSION_OFFSET - FLASH_ADDR_BASE, 2);
    if (status != status_success) {
        printf("flash read fail\r\n");
        return -1;
    }
    fw_version2 = (uint16_t)version;

    printf("ota1 version:%d, ota2 version:%d\n", fw_version1, fw_version2);

    if (rom_xpi_nor_is_remap_enabled(BOARD_APP_XPI_NOR_XPI_BASE)) {
        return 1;
    }
    return 0;
}