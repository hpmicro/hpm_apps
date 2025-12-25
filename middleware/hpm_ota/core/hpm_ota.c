/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_l1c_drv.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_hashs.h"
#include "hpm_groupsplit.h"
#include "hpm_flash.h"
#include "hpm_log.h"
#include "hpm_ota.h"

#define EXIP_PADING_LEN_SIZE   (0x400)

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
        && ota_handle.current_addr != FLASH_USER_APP2_ADDR
        && ota_handle.current_addr != FLASH_DIFF_APP_ADDR
    )
    {
        HPM_LOG_ERR("BAD! addr error!\r\n");
        return -1;
    }
    if (ota_handle.current_header.hash_enable &&
        !hpm_ota_package_verify(ota_handle.current_addr + sizeof(hpm_app_header_t) + FLASH_ADDR_BASE,
                                ota_handle.current_header.len, &ota_handle.current_header))
    {
        HPM_LOG_ERR("BAD! verify failed!\r\n");
        return -2;
    }
    HPM_LOG_INFO("ota success!\r\n");
    if (reset_enable)
    {
        memset(&ota_handle, 0, sizeof(hpm_app_header_t));
        hpm_ota_soc_reset();
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
    return ota_index == HPM_APP1 ? FLASH_USER_APP1_SIZE : FLASH_USER_APP2_SIZE;
}

uint32_t hpm_ota_partition_maxsize(uint32_t addr)
{
    addr = addr > FLASH_ADDR_BASE ? addr - FLASH_ADDR_BASE : addr;
    if(addr == FLASH_USER_KEY_ADDR)
        return FLASH_USER_KEY_SIZE;
    if(addr == FLASH_USER_APP1_ADDR)
        return FLASH_USER_APP1_SIZE;
    if(addr == FLASH_USER_APP2_ADDR)
        return FLASH_USER_APP2_SIZE;
    if(addr == FLASH_DIFF_APP_ADDR)
        return FLASH_DIFF_APP_SIZE;
    return 0;
}

int hpm_ota_set_raminfo(ota_ram_info_t *ram_info)
{
    if(ram_info == NULL)
        return -1;
    ram_info->magic = OTA_RAM_INFI_MAGIC;
    memcpy((void*)HPM_OTA_INFO_RAM_ADDR, (void*)ram_info, sizeof(ota_ram_info_t));
    return 0;
}

void hpm_ota_clear_raminfo(void)
{
    memset((void*)HPM_OTA_INFO_RAM_ADDR, 0, sizeof(ota_ram_info_t));
}

ota_ram_info_t *hpm_ota_get_raminfo(void)
{
    return (ota_ram_info_t*)HPM_OTA_INFO_RAM_ADDR;
}

int hpm_ota_get_nowrunning_app(void)
{
#if defined(BOOTUSER_ENABLE) && BOOTUSER_ENABLE
    return -1;
#endif
    if (rom_xpi_nor_is_remap_enabled(HPM_XPI0))
        return HPM_APP2;

    if(hpm_ota_get_raminfo()->magic == OTA_RAM_INFI_MAGIC)
        return hpm_ota_get_raminfo()->app;
    
    return HPM_APP1;
}

int hpm_ota_get_header_info_of_app(uint8_t ota_index, hpm_app_header_t *app_header)
{
    int ret = -1;
    if(app_header == NULL)
        return -1;
    if (rom_xpi_nor_is_remap_enabled(HPM_XPI0))
    {
        if(ota_index == HPM_APP1)
        {
            ret = hpm_flash_read(FLASH_USER_APP1_ADDR, (unsigned char*)app_header, sizeof(hpm_app_header_t));
        }
        else if(hpm_ota_img_maxsize(HPM_APP2) != 0)
        {
            ret = hpm_flash_read(FLASH_USER_APP2_ADDR, (unsigned char*)app_header, sizeof(hpm_app_header_t));
        }
    }
    else
    {
        if(ota_index == HPM_APP1)
        {
            ret = hpm_flash_localmem_read(FLASH_USER_APP1_ADDR, (unsigned char*)app_header, sizeof(hpm_app_header_t));
        }
        else if(hpm_ota_img_maxsize(HPM_APP2) != 0)
        {
            ret = hpm_flash_localmem_read(FLASH_USER_APP2_ADDR, (unsigned char*)app_header, sizeof(hpm_app_header_t));
        }
    }
    return ret;
}

int hpm_ota_get_header_info_of_addr(uint32_t addr, hpm_app_header_t* app_header)
{
    if (rom_xpi_nor_is_remap_enabled(HPM_XPI0))
    {
        return -1;
    }
    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;

    switch(addr)
    {
        case FLASH_USER_APP1_ADDR:
        {
            if(hpm_ota_partition_maxsize(FLASH_USER_APP1_ADDR) == 0)
                return -1;
        }
        break;
#if FLASH_USER_APP2_SIZE
        case FLASH_USER_APP2_ADDR:
        {
            if(hpm_ota_partition_maxsize(FLASH_USER_APP2_ADDR) == 0)
                return -1;
        }
        break;
#endif
#if FLASH_DIFF_APP_SIZE
        case FLASH_DIFF_APP_ADDR:
        {
            if(hpm_ota_partition_maxsize(FLASH_DIFF_APP_ADDR) == 0)
                return -1;
        }
        break;
#endif
        default:
            return -1;
    }

    return hpm_flash_localmem_read(addr, (unsigned char*)app_header, sizeof(hpm_app_header_t));
}

uint8_t hpm_ota_get_download_app(void)
{
#if defined(BOOTUSER_ENABLE) && BOOTUSER_ENABLE
    hpm_app_header_t app1_header = {0};
    hpm_app_header_t app2_header = {0};
    hpm_ota_get_header_info_of_app(HPM_APP1, &app1_header);
    hpm_ota_get_header_info_of_app(HPM_APP2, &app2_header);
    if(hpm_ota_img_maxsize(HPM_APP2) == 0)
    {
        return HPM_APP1;
    }
    if (app1_header.magic != HPM_APP_FILE_FLAG_MAGIC ||
        app1_header.version == 0 || app1_header.version == 0xFFFF)
        return HPM_APP1;
    if (app2_header.magic != HPM_APP_FILE_FLAG_MAGIC ||
        app2_header.version == 0 || app2_header.version == 0xFFFF)
        return HPM_APP2;
    if (app1_header.version > app2_header.version)
        return HPM_APP2;
    else
        return HPM_APP1;
#else
    if(hpm_ota_get_nowrunning_app() == HPM_APP1)
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
        hpm_app_header_t temp_header = {0};
        memset(&ota_handle, 0, sizeof(hpm_app_header_t));
        memcpy(&ota_handle.current_header, current_header, sizeof(hpm_app_header_t));
        HPM_LOG_INFO("type:%d, len:%d, version:%d, hash_type:%d\r\n", \
                      current_header->type, current_header->len, current_header->version, current_header->hash_type);
        switch(OTA_TYPE_BODY_GET(current_header->type))
        {
            case OTA_TYPE_NORMAL_FIRMWARE:
            {
                if (hpm_ota_get_download_app() == HPM_APP1)
                {
                    hpm_ota_get_header_info_of_app(HPM_APP1, &temp_header);
                    if(temp_header.version == current_header->version)
                    {
                        HPM_LOG_WRN("Same version number, ignore!\r\n");
                        ota_handle.current_addr = 0;
                    }
                    else
                    {
                        HPM_LOG_INFO("ota0 data download...\n");
                        ota_handle.current_addr = FLASH_USER_APP1_ADDR;
                    }
                }
                else
                {
                    hpm_ota_get_header_info_of_app(HPM_APP2, &temp_header);
                    if(temp_header.version == current_header->version)
                    {
                        HPM_LOG_WRN("Same version number, ignore!\r\n");
                        ota_handle.current_addr = 0;
                    }
                    else
                    {
                        HPM_LOG_INFO("ota1 data download...\n");
                        ota_handle.current_addr = FLASH_USER_APP2_ADDR;
                    }
                }
            }
            break;
            case OTA_TYPE_DIFF_FIRMWARE:
            {
                ota_handle.current_addr = FLASH_DIFF_APP_ADDR;
                HPM_LOG_INFO("diff image, img data download...\n");
            }
            break;
            case OTA_TYPE_LZMA_FIRMWARE:
            {
                ota_handle.current_addr = FLASH_USER_APP2_ADDR;
                HPM_LOG_INFO("lzma image, img data download...\n");
            }
            break;
            default:
            break;
        }

        if(hpm_ota_partition_maxsize(ota_handle.current_addr) - sizeof(hpm_app_header_t) < current_header->len)
        {
            HPM_LOG_ERR("BAD! Firmware size exceeds the maximum!\r\n");
            ota_handle.current_addr = 0;
        }
    }

    if (ota_handle.current_addr != 0)
    {
        ret = 0;
        //HPM_LOG_INFO("addr:0x%08x, len:%d\r\n",ota_handle.current_addr + ota_handle.pack_offset, len);
        hpm_ota_flash_write(ota_handle.current_addr + ota_handle.pack_offset, src, len);

        ota_handle.pack_offset += len;
        if (ota_handle.pack_offset >= ota_handle.current_header.len + sizeof(hpm_app_header_t))
        {
            // complete checksum and reset
            HPM_LOG_INFO("complete checksum and reset!\r\n");
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

//this api only usb device msc channel, fatfs：File transfers are not continuous, so data is written in full erase and then in packets.
static int hpm_ota_splitpack_write(hpm_groupsplit_pack_t *groupsplit, bool reset_enable)
{
    int ret = -1;
    hpm_app_header_t temp_header = {0};
    if(groupsplit == NULL)
        return -1;
    if(ota_handle.current_addr == 0) //first pack
    {
        ota_handle.pack_offset = 0;
        ota_handle.current_header.len = groupsplit->total_len;
        hpm_flash_empty_erase_mask();
        switch(OTA_TYPE_BODY_GET(groupsplit->type))
        {
            case OTA_TYPE_NORMAL_FIRMWARE:
            {
                if (hpm_ota_get_download_app() == HPM_APP1)
                {
                    hpm_ota_get_header_info_of_app(HPM_APP1, &temp_header);
                    if(temp_header.version == groupsplit->file_version)
                    {
                        HPM_LOG_WRN("Same version number, ignore!\r\n");
                        ota_handle.current_addr = 0;
                    }
                    else
                    {
                        HPM_LOG_INFO("ota0 data download...\n");
                        ota_handle.current_addr = FLASH_USER_APP1_ADDR;
                    }
                }
                else
                {
                    hpm_ota_get_header_info_of_app(HPM_APP2, &temp_header);
                    if(temp_header.version == groupsplit->file_version)
                    {
                        HPM_LOG_WRN("Same version number, ignore!\r\n");
                        ota_handle.current_addr = 0;
                    }
                    else
                    {
                        HPM_LOG_INFO("ota1 data download...\n");
                        ota_handle.current_addr = FLASH_USER_APP2_ADDR;
                    }
                }
            }
            break;
            case OTA_TYPE_DIFF_FIRMWARE:
            {
                ota_handle.current_addr = FLASH_DIFF_APP_ADDR;
                HPM_LOG_INFO("diff image, img data download...\n");
            }
            break;
            case OTA_TYPE_LZMA_FIRMWARE:
            {
                ota_handle.current_addr = FLASH_USER_APP2_ADDR;
                HPM_LOG_INFO("lzma image, img data download...\n");
            }
            break;
            default:
            break;
        }

        if(hpm_ota_partition_maxsize(ota_handle.current_addr) - sizeof(hpm_app_header_t) < ota_handle.current_header.len)
        {
            HPM_LOG_ERR("BAD! Firmware size exceeds the maximum!\r\n");
            ota_handle.current_addr = 0;
        }

        //Here, the current address is temporarily saved with currnet_erase, which will be used later to get the header information.
        ota_handle.current_erase = ota_handle.current_addr;
    }

    if (ota_handle.current_addr != 0)
    {
        ret = 0;
        //HPM_LOG_INFO("index:%d, addr:0x%08x, size:%d\r\n",groupsplit->index, ota_handle.current_addr + (groupsplit->index * sizeof(groupsplit->data)), groupsplit->datasize);
        hpm_flash_erase_write_of_mask(ota_handle.current_addr + (groupsplit->index * sizeof(groupsplit->data)), 
                                      groupsplit->data, groupsplit->datasize);
        ota_handle.pack_offset += groupsplit->datasize;
        //HPM_LOG_INFO("pack_offset:%d, total len:%d\r\n", ota_handle.pack_offset,  ota_handle.current_header.len);
        if (ota_handle.pack_offset >= ota_handle.current_header.len)
        {
            // complete checksum and reset
            HPM_LOG_INFO("complete checksum and reset!\r\n");
            hpm_ota_get_header_info_of_addr(ota_handle.current_erase, &temp_header);
            memcpy((void*)&ota_handle.current_header, (void*)&temp_header, sizeof(hpm_app_header_t));
            if(hpm_ota_complete_end(reset_enable) == 0)
            {
                ret = 1;
            }
            
            memset(&ota_handle, 0, sizeof(hpm_app_header_t));
        }
    }
    return ret;
}

int hpm_ota_split_write(void const *src, uint32_t len, bool reset_enable)
{
    uint32_t i;
    int ret;
    hpm_groupsplit_pack_t *groupsplit_t = NULL;
    if (len < sizeof(hpm_groupsplit_pack_t) || (len % sizeof(hpm_groupsplit_pack_t)) != 0)
    {
        HPM_LOG_ERR("BAD!, sizeof(hpm_groupsplit_pack_t):%d, input len:%d\r\n", sizeof(hpm_groupsplit_pack_t), len);
        return -1;
    }
    for (i = 0; i < len / sizeof(hpm_groupsplit_pack_t); i++)
    {
        groupsplit_t = (hpm_groupsplit_pack_t *)src + (i * sizeof(hpm_groupsplit_pack_t));
        if (!is_hpm_groupsplit_block(groupsplit_t))
        {
            continue;
        }

        ret = hpm_ota_splitpack_write(groupsplit_t, reset_enable);
        if(ret < 0)
            return ret;
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

    if(rom_xpi_nor_is_remap_enabled(HPM_XPI0))
    {
        HPM_LOG_INFO("remap enable!\r\n");
        do
        {
            /*NOTE:If define FLASH_USER_APP2_ADDR, you need to perform address offset and map, then flash reads the firmware content.*/
            curlen = ((len - offset) > sizeof(buffer)) ? sizeof(buffer) : (len - offset);
            hpm_flash_read(addr - FLASH_ADDR_BASE + offset, buffer, curlen);
            offset += curlen;
            if (hpm_hash_update(buffer, curlen) != 0)
            {
                HPM_LOG_ERR("hash update fail\r\n");
                return false;
            }
        }while((int)(len-offset) > 0);
    }
    else
    {
        /*NOTE:xip points to the firmware address, reads the firmware content, firmware length*/
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(xip_point);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(xip_point + len);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);
        if (hpm_hash_update(xip_point, len) != 0)
        {
            HPM_LOG_ERR("hash update fail\r\n");
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

bool hpm_ota_package_makehash(uint32_t addr, uint32_t len, hpm_app_header_t *ota_header)
{
    addr = addr < FLASH_ADDR_BASE ? addr + FLASH_ADDR_BASE : addr;
    uint8_t *xip_point = (uint8_t *)addr;
    uint8_t *result_verify = NULL;
    uint8_t hash_len = 0;
    uint8_t buffer[1024];
    uint32_t offset = 0, curlen;

    if (ota_header == NULL)
        return false;
    if (hpm_hash_init(ota_header->hash_type) != 0)
        return false;

    if(rom_xpi_nor_is_remap_enabled(HPM_XPI0))
    {
        HPM_LOG_INFO("remap enable!\r\n");
        do
        {
            /*NOTE:If define FLASH_USER_APP2_ADDR, you need to perform address offset and map, then flash reads the firmware content.*/
            curlen = ((len - offset) > sizeof(buffer)) ? sizeof(buffer) : (len - offset);
            hpm_flash_read(addr - FLASH_ADDR_BASE + offset, buffer, curlen);
            offset += curlen;
            if (hpm_hash_update(buffer, curlen) != 0)
            {
                HPM_LOG_ERR("hash update fail\r\n");
                return false;
            }
        }while((int)(len-offset) > 0);
    }
    else
    {
        /*NOTE:xip points to the firmware address, reads the firmware content, firmware length*/
        if (hpm_hash_update(xip_point, len) != 0)
        {
            HPM_LOG_ERR("hash update fail\r\n");
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

    memcpy(ota_header->hash_data, result_verify, hash_len);

    return true;
}

ATTR_WEAK const ota_app_key_t* hpm_ota_get_key_info(void)
{
    static ota_app_key_t app_key = {0};
    uint8_t *result_verify = NULL;
    if(hpm_ota_partition_maxsize(FLASH_USER_KEY_ADDR) == 0)
        return NULL;

    hpm_flash_localmem_read(FLASH_USER_KEY_ADDR, (unsigned char*)&app_key, sizeof(ota_app_key_t));

    if(app_key.magic != OTA_APP_KEY_MAGIC || app_key.count == 0)
        return NULL;

    if (hpm_hash_init(HASH_TYPE_CRC32) != 0)
        return NULL;

    /*NOTE:xip points to the firmware address, reads the firmware content, firmware length*/
    if (hpm_hash_update((uint8_t*)&app_key.exip_region[0], sizeof(exip_region_param_t) * app_key.count) != 0)
    {
        HPM_LOG_ERR("hash update fail\r\n");
        return NULL;
    }

    result_verify = hpm_hash_finsh();
    if (result_verify == NULL)
        return NULL;
    
    if(*(uint32_t*)result_verify != app_key.crc32)
    {
        HPM_LOG_ERR("crc32 verify fail!\r\n");
        return NULL;
    }

    return (ota_app_key_t*)&app_key;
}

int hpm_ota_app_exip_config(hpm_app_header_t *app_header, uint8_t appindex)
{
    (void) appindex;
    exip_region_param_t exip_region = {0};
    HPM_LOG_INFO("exip enable!\r\n");
    if(app_header == NULL)
        return -1;
    const ota_app_key_t *appkey = hpm_ota_get_key_info();
    if(appkey == NULL)
        return -1;
    memcpy(&exip_region.key, appkey->exip_region[0].key, 16);
    memcpy(&exip_region.ctr[0], &appkey->exip_region[0].ctr[4], 4);
    memcpy(&exip_region.ctr[4], &appkey->exip_region[0].ctr[0], 4);

    //Since APP2 will enable remap, it only needs to be configured for the APP1 address
    exip_region.start = FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t);
    exip_region.len = app_header->len + ((EXIP_PADING_LEN_SIZE - (app_header->len % EXIP_PADING_LEN_SIZE)) % EXIP_PADING_LEN_SIZE);
    HPM_LOG_INFO("exip config start:0x%08x, len:0x%08x\r\n", exip_region.start, exip_region.len);

    rom_xpi_nor_exip_disable(HPM_XPI0);
    if(!rom_xpi_nor_exip_region_config(HPM_XPI0, HPM_OTA_USER_EXIP_INDEX, &exip_region))
        return -2;

    rom_xpi_nor_exip_enable(HPM_XPI0);
    return 0;
}

#ifdef HPMSOC_HAS_HPMSDK_SDP
ATTR_PLACE_AT_NONCACHEABLE static sdp_dma_ctx_t s_dma_ctx;
#endif

static void memcpy_userapp(uint32_t img_flash_addr, uint32_t img_run_addr, uint32_t img_size)
{
#ifdef HPMSOC_HAS_HPMSDK_SDP
    sdp_dma_ctx_t *p_sdp_ctx = (sdp_dma_ctx_t *)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&s_dma_ctx);
    uint32_t sec_core_app_sys_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)img_flash_addr);
    uint32_t sec_core_img_sys_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)img_run_addr);
#endif
    uint32_t aligned_start;
    uint32_t aligned_end;
    uint32_t aligned_size;

#ifdef HPMSOC_HAS_HPMSDK_SDP
    clock_add_to_group(clock_sdp, 0);
    HPM_LOG_INFO("Copying image memory: 0x%08x -> 0x%08x\r\n", sec_core_app_sys_addr, sec_core_img_sys_addr);
    rom_sdp_memcpy(p_sdp_ctx, (void *)sec_core_img_sys_addr, (void *)sec_core_app_sys_addr, img_size);
#else
    HPM_LOG_INFO("Copying image memory: 0x%08x -> 0x%08x\r\n", img_flash_addr, img_run_addr);
    memcpy((void *)img_run_addr, (void *)img_flash_addr, img_size);
#endif
    if (l1c_ic_is_enabled() || l1c_dc_is_enabled())
    {
        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(img_run_addr);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(img_run_addr + img_size);
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

void hpm_appindex_jump(uint8_t appindex)
{
    hpm_app_header_t header_info = {0};
    uint32_t jump_addr;
    ota_ram_info_t ram_info = {
        .app = appindex,
    };
    rom_xpi_nor_remap_disable(HPM_XPI0);
    hpm_ota_get_header_info_of_app(appindex, &header_info);
    if(header_info.magic != HPM_APP_FILE_FLAG_MAGIC)
    {
        HPM_LOG_ERR("BAD! header info error!\r\n");
        return;
    }

    if(OTA_TYPE_EXIP_GET(header_info.type) && hpm_ota_app_exip_config(&header_info, appindex) < 0)
    {
        HPM_LOG_ERR("BAD!  exip config error!\r\n");
        return;
    }

    if(appindex == HPM_APP2)
    {
        rom_xpi_nor_remap_config(HPM_XPI0, FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE, FLASH_USER_APP1_SIZE, FLASH_USER_APP1_SIZE);
        rom_xpi_nor_is_remap_enabled(HPM_XPI0);
    }

    if(OTA_TYPE_RAM_GET(header_info.type))
    {
        //ram type
        HPM_LOG_INFO("ram type enable! \r\n");
#if defined(CONFIG_USE_RAM_TYPE) && CONFIG_USE_RAM_TYPE
        HPM_LOG_ERR("Error, not supporting that both boot and app are executed in ram.");
        (void)memcpy_userapp;
        return;
#else
        memcpy_userapp(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), header_info.start_addr, header_info.len);
        jump_addr = header_info.start_addr;
#endif
    }
    else
    {
        jump_addr = (FLASH_USER_APP1_ADDR + sizeof(hpm_app_header_t) + FLASH_ADDR_BASE);
    }

    ram_info.addr = jump_addr;
    hpm_ota_set_raminfo(&ram_info);

    printf("jump addr:0x%08x\r\n", jump_addr);
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    l1c_dc_writeback_all();
    l1c_dc_disable();
    l1c_ic_disable();
    fencei();
    __asm("mv a0, %0" : : "r"(jump_addr));
    __asm("jr a0");
    WFI();
    while (1)
    {
    }
}