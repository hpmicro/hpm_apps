/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_l1c_drv.h"
#include "hpm_hashs.h"
#include "hpm_platform.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "hpm_ota.h"

typedef struct
{
    uint32_t current_addr;
    uint32_t current_erase;
    uint32_t pack_offset;
    hpm_app_header_t current_header;
} hpm_ota_handle_t;

static hpm_ota_handle_t ota_handle = {0};

#define SOC_ROM_API_ARG_REG (*(volatile uint32_t *)(0xF40D4000UL + 0U))
#define SOC_XPI_PERSIST_REG (*(volatile uint32_t *)(0xF40D4000UL + 4U))
#define SOC_ROM_FLAG_REG (*(volatile uint32_t *)(0xF40D4000UL + 8U))

static int hpm_ota_complete_end(bool reset_enable)
{
    if (ota_handle.current_addr != FLASH_USER_APP1_ADDR
#ifdef FLASH_USER_APP2_ADDR
        && ota_handle.current_addr != FLASH_USER_APP2_ADDR
#endif
    )
    {
        printf("BAD! addr error!\r\n");
        return -1;
    }
    if (ota_handle.current_header.hash_enable &&
        !hpm_ota_package_verify(ota_handle.current_addr + sizeof(hpm_app_header_t) + FLASH_ADDR_BASE,
                                ota_handle.current_header.len, &ota_handle.current_header))
    {
        printf("BAD! verify failed!\r\n");
        return -2;
    }
#ifndef CONFIG_UART_CHANNEL
    printf("ota success!\r\n");
#endif
    if (reset_enable)
    {
        memset(&ota_handle, 0, sizeof(hpm_app_header_t));
#ifndef CONFIG_UART_CHANNEL
        hpm_ota_soc_reset();
#endif
    }
    return 0;
}

static int hpm_ota_flash_write(uint32_t addr, void const *src, uint32_t len)
{
    if (ota_handle.current_erase <= (addr + len))
    {
        if (ota_handle.current_erase == 0)
        {
            ota_handle.current_erase = addr;
        }
        if (hpm_flash_erase_sector(ota_handle.current_erase) < 0)
        {
            printf("erase error!\r\n");
            return -2;
        }
        ota_handle.current_erase += FLASH_SECTOR_SIZE;
    }

    return hpm_flash_write(addr, (unsigned char *)src, len);
}

void hpm_ota_soc_reset(void)
{
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    SOC_ROM_FLAG_REG = 0;                            // warning: this is rom reset flag,
    ppor_sw_reset(HPM_PPOR, 1000);
    while (1)
    {
    }
}

uint32_t hpm_ota_img_maxsize(uint8_t ota_index)
{
#ifdef FLASH_USER_APP2_SIZE
    return ota_index == HPM_APP1 ? FLASH_USER_APP1_SIZE : FLASH_USER_APP2_SIZE;
#else
    return ota_index == HPM_APP1 ? FLASH_USER_APP1_SIZE : 0;
#endif
}

uint8_t hpm_ota_get_nowrunning_appindex(void)
{
#ifdef FLASH_USER_APP2_ADDR
    if (rom_xpi_nor_is_remap_enabled(HPM_XPI0))
        return HPM_APP2;
#endif
    return HPM_APP1;
}

hpm_app_header_t *hpm_ota_get_flash_header_info(uint8_t ota_index)
{
#if defined(BOOTUSER_ENABLE) && BOOTUSER_ENABLE
    uint32_t aligned_start;
    uint32_t aligned_end;
    uint32_t aligned_size;
#ifdef FLASH_USER_APP2_ADDR
    if(ota_index == HPM_APP1)
    {
        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
        return (hpm_app_header_t *)(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE);
    }
    else
    {
        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
        return (hpm_app_header_t *)(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE);
    }
#else
    if(ota_index == HPM_APP1)
    {
        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
        return (hpm_app_header_t *)(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE);
    }
    return NULL;
#endif
#else
    static hpm_app_header_t header_info1, header_info2;
    if(ota_index == HPM_APP1)
    {
        hpm_flash_read(FLASH_USER_APP1_ADDR, (unsigned char*)&header_info1, sizeof(hpm_app_header_t));
        return &header_info1;
    }
    else
    {
#ifdef FLASH_USER_APP2_ADDR
        hpm_flash_read(FLASH_USER_APP2_ADDR, (unsigned char*)&header_info2, sizeof(hpm_app_header_t));
        return &header_info2;
#else
        return NULL;
#endif
    }
#endif
}

uint8_t hpm_ota_check_download_appindex(void)
{
#if defined(BOOTUSER_ENABLE) && BOOTUSER_ENABLE
    hpm_app_header_t *app1_header = hpm_ota_get_flash_header_info(HPM_APP1);
    hpm_app_header_t *app2_header = hpm_ota_get_flash_header_info(HPM_APP2);
#ifndef FLASH_USER_APP2_ADDR
    return HPM_APP1;
#endif
    if (app1_header == NULL || app1_header->magic != HPM_APP_FILE_FLAG_MAGIC ||
        app1_header->version == 0 || app1_header->version == 0xFFFF)
        return HPM_APP1;
    if (app2_header == NULL || app2_header->magic != HPM_APP_FILE_FLAG_MAGIC ||
        app2_header->version == 0 || app2_header->version == 0xFFFF)
        return HPM_APP2;
    if (app1_header->version > app2_header->version)
        return HPM_APP2;
    else
        return HPM_APP1;
#else
    if(hpm_ota_get_nowrunning_appindex() == HPM_APP1)
        return HPM_APP2;
    return HPM_APP1;
#endif
    
}

int hpm_ota_auto_write(void const *src, uint32_t len, bool reset_enable)
{
    int ret = -1;
    hpm_app_header_t *current_header = (hpm_app_header_t *)src;
    if (current_header->magic == HPM_APP_FILE_FLAG_MAGIC &&
        current_header->touchid == HPM_APP_FILE_TOUCH_ID)
    {
        memset(&ota_handle, 0, sizeof(hpm_app_header_t));
        memcpy(&ota_handle.current_header, current_header, sizeof(hpm_app_header_t));
        if (current_header->type == OTA_TYPE_FIRMWARE)
        {
            if (hpm_ota_check_download_appindex() == HPM_APP1)
            {
#ifndef CONFIG_UART_CHANNEL
                printf("ota0, device:0x%08X, length:%d, version:%d, hash_type:0x%08x\n",
                       current_header->device, current_header->len, current_header->version, current_header->hash_type);
#endif
#if !defined(BOOTUSER_ENABLE) && defined(CONFIG_USB_HOST_CHANNEL) && CONFIG_USB_HOST_CHANNEL //this prevent loop ota!
                hpm_app_header_t *temp_header = hpm_ota_get_flash_header_info(HPM_APP1);
                if(temp_header != NULL && temp_header->version == current_header->version)
                {
                    printf("Same version number, ignore!\r\n");
                    return 0;
                }
#endif
                ota_handle.current_addr = FLASH_USER_APP1_ADDR;
#ifndef CONFIG_UART_CHANNEL
                printf("ota0 data download...\n");
#endif
            }
            else
            {
#ifdef FLASH_USER_APP2_ADDR
#ifndef CONFIG_UART_CHANNEL
                printf("ota1, device:0x%08X, length:%d, version:%d, hash_type:0x%08x\n",
                       current_header->device, current_header->len, current_header->version, current_header->hash_type);
#endif
#if !defined(BOOTUSER_ENABLE) && defined(CONFIG_USB_HOST_CHANNEL) && CONFIG_USB_HOST_CHANNEL //this prevent loop ota!
                hpm_app_header_t *temp_header = hpm_ota_get_flash_header_info(HPM_APP2);
                if(temp_header != NULL && temp_header->version == current_header->version)
                {
                    printf("Same version number, ignore!\r\n");
                    return 0;
                }
#endif
                ota_handle.current_addr = FLASH_USER_APP2_ADDR;
#ifndef CONFIG_UART_CHANNEL
                printf("ota1 data download...\n");
#endif
#endif
            }
        }
        else if (current_header->type == OTA_TYPE_PROFILE)
        {
            ota_handle.current_addr = FLASH_PROFILE_ADDR;
#ifndef CONFIG_UART_CHANNEL
            printf("profile, length:%d,hash_type:0x%08x\n",
                   current_header->len, current_header->hash_type);
            printf("profile data download...\n");
#endif
        }
    }

    if (ota_handle.current_addr != 0)
    {
        ret = hpm_ota_flash_write(ota_handle.current_addr + ota_handle.pack_offset, src, len);
        if (ret < 0)
        {
            printf("Error: write flash failed!, addr: 0x%x, length: 0x%x\n", ota_handle.current_addr + ota_handle.pack_offset, len);
            return -1;
        }

        ota_handle.pack_offset += len;
        if (ota_handle.pack_offset >= ota_handle.current_header.len + sizeof(hpm_app_header_t))
        {
            // complete checksum and reset
#ifndef CONFIG_UART_CHANNEL
            printf("complete checksum and reset!\r\n");
#endif
            ret = hpm_ota_complete_end(reset_enable);
            if (ret == 0)
            {
                ret = 1;
            }
            memset(&ota_handle, 0, sizeof(hpm_app_header_t));
        }
    }
    return ret;
}

int hpm_ota_auto_write_of_addr(uint32_t addr, void const *src, uint32_t len, bool reset_enable)
{
    int ret = -1;
    hpm_app_header_t *current_header = (hpm_app_header_t *)src;
    if(ota_handle.current_addr == 0) //first pack
    {
        ota_handle.pack_offset = 0;
        if (hpm_ota_check_download_appindex() == HPM_APP1)
        {
            ota_handle.current_addr = FLASH_USER_APP1_ADDR;
            printf("ota0 data download...\n");
            ret = hpm_flash_erase(FLASH_USER_APP1_ADDR, FLASH_USER_APP1_SIZE);
            if (ret < 0)
            {
                printf("Error: erase flash failed!, addr: 0x%x, length: 0x%x\n", FLASH_USER_APP1_ADDR, FLASH_USER_APP1_SIZE);
                return -1;
            }
        }
        else
        {
#ifdef FLASH_USER_APP2_ADDR
            ota_handle.current_addr = FLASH_USER_APP2_ADDR;
            printf("ota1 data download...\n");
            ret = hpm_flash_erase(FLASH_USER_APP2_ADDR, FLASH_USER_APP2_SIZE);
            if (ret < 0)
            {
                printf("Error: erase flash failed!, addr: 0x%x, length: 0x%x\n", FLASH_USER_APP2_ADDR, FLASH_USER_APP2_SIZE);
                return -1;
            }
#else
            ota_handle.current_addr = 0;
            printf("BAD! not support update!\r\n");
#endif
        }
    }

    if (current_header->magic == HPM_APP_FILE_FLAG_MAGIC &&
        current_header->touchid == HPM_APP_FILE_TOUCH_ID)
    {
        memcpy(&ota_handle.current_header, current_header, sizeof(hpm_app_header_t));
        if (current_header->type == OTA_TYPE_PROFILE)
        {
            printf("BAD! in this must update firmware!\r\n");
            return -1;
        }
    }

    if (ota_handle.current_addr != 0)
    {
        ret = hpm_flash_write(ota_handle.current_addr + addr, src, len);
        if (ret < 0)
        {
            printf("Error: write flash failed!, addr: 0x%x, length: 0x%x\n", ota_handle.current_addr + addr, len);
            return -1;
        }

        ota_handle.pack_offset += len;
        if (ota_handle.pack_offset >= ota_handle.current_header.len + sizeof(hpm_app_header_t))
        {
            // complete checksum and reset
            printf("complete checksum and reset!\r\n");
            ret = hpm_ota_complete_end(reset_enable);
            if (ret == 0)
            {
                ret = 1;
            }
            memset(&ota_handle, 0, sizeof(hpm_app_header_t));
        }
    }
    return ret;
}

bool hpm_ota_package_verify(uint32_t addr, uint32_t len, hpm_app_header_t *ota_header)
{
    uint8_t *xip_point = (uint8_t *)addr;
    uint8_t *result_verify = NULL;
    uint8_t hash_len = 0;
    uint8_t buffer[1024];
    uint32_t offset = 0, curlen;

    if (ota_header == NULL)
        return false;
    if (hpm_hash_init(ota_header->hash_type) != 0)
        return false;

#ifdef FLASH_USER_APP2_ADDR
    if(rom_xpi_nor_is_remap_enabled(HPM_XPI0))
    {
        printf("remap enable!\r\n");
        do
        {
            /*NOTE:If define FLASH_USER_APP2_ADDR, you need to perform address offset and map, then flash reads the firmware content.*/
            curlen = ((len - offset) > sizeof(buffer)) ? sizeof(buffer) : (len - offset);
            hpm_flash_read(addr - FLASH_ADDR_BASE + offset, buffer, curlen);
            offset += curlen;
            if (hpm_hash_update(buffer, curlen) != 0)
            {
                printf("hash update fail\r\n");
                return false;
            }
        }while((int)(len-offset) > 0);
    }
    else
#endif
    {
        /*NOTE:xip points to the firmware address, reads the firmware content, firmware length*/
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(xip_point);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(xip_point + len);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
        if (hpm_hash_update(xip_point, len) != 0)
        {
            printf("hash update fail\r\n");
            return false;
        }
    }

    result_verify = hpm_hash_finsh();
    if (result_verify == NULL)
        return false;

    if (ota_header->hash_type == HASH_TYPE_CHECKSUM ||
        ota_header->hash_type == HASH_TYPE_XOR ||
        ota_header->hash_type == HASH_TYPE_CRC32)
    {
        hash_len = 4;
    }
    else if (ota_header->hash_type == HASH_TYPE_SHA1)  
    {
        hash_len = 20;
    }
    else
    {
        hash_len = 32;
    }

    if (memcmp(ota_header->hash_data, result_verify, hash_len) != 0)   
    {
        return false;
    }

    return true;
}

void hpm_appindex_jump(uint8_t appindex)
{
#ifdef FLASH_USER_APP2_ADDR
    rom_xpi_nor_remap_disable(HPM_XPI0);
    if (appindex == HPM_APP2)
    {
        rom_xpi_nor_remap_config(HPM_XPI0, FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE, FLASH_USER_APP1_SIZE, FLASH_USER_APP1_SIZE);
        rom_xpi_nor_is_remap_enabled(HPM_XPI0);
    }
#endif
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    //disable_global_irq(CSR_MSTATUS_SIE_MASK);
    disable_global_irq(CSR_MSTATUS_UIE_MASK);
    l1c_dc_invalidate_all();
    l1c_dc_disable();
    l1c_ic_disable();
    fencei();
    __asm("la a0, %0" ::"i"(FLASH_USER_APP1_ADDR + sizeof(hpm_app_header_t) + FLASH_ADDR_BASE));
    __asm("jr a0");
    // WFI();
    while (1)
    {
    }
}