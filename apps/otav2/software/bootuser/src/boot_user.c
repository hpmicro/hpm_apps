/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "hpm_flash.h"
#include "hpm_ota.h"
#include "hpm_log.h"
#include "hpm_gpio_drv.h"
#include "hpm_diffmgr.h"
#include "hpm_lzma.h"
#include "ota_port.h"
#include "boot_user.h"


#ifndef BIT
#define BIT(n)                                      (1UL << (n))
#endif

#ifndef min
#define min(a, b)                                   (((a) < (b)) ? (a) : (b))
#endif

/**
 * @brief Check the PBUT KEY status;上电检查PBUT KEY按下状态
 */
bool bootuser_check_pbutn_bootmode(void)
{
    uint32_t count = 0;
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    for (uint8_t i = 0; i < 10; i++)
    {
        if (0 == gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN))
        {
            count++;
        }
        board_delay_ms(10);
    }
    if (count > 9)
        return true;
    return false;
}
/**
 * @brief check app vaild;hash校验固件升级包是否有效
 *
 * @param [in] addr fireware address;固件地址
 * @param [in] header header strcut;头文件信息
 * @return verify result;校验结果，0:check fail, 1:check success.
 */
static bool bootuser_verify_app_valid(uint32_t addr, hpm_app_header_t *header)
{
    if (header == NULL)
        return false;
    if (header->hash_enable && header->pwr_hash)
    {
        if (hpm_ota_package_verify(addr + sizeof(hpm_app_header_t) + FLASH_ADDR_BASE,
                                   header->len, header))
        {
            HPM_LOG_INFO("Addr:0x%08x, verify SUCCESS!\r\n", addr);
            return true;
        }
    }
    else
    {
        return true;
    }
    HPM_LOG_WRN("Addr:0x%08x, valify FAIL!\r\n", addr);
    return false;
}

const ota_app_key_t* hpm_ota_get_key_info(void)
{
    static const ota_app_key_t app_key = {
        .magic = OTA_APP_KEY_MAGIC,
        .count = 1,
        .crc32 = 0,
        .exip_region[0] = {
            .start = 0,
            .len = 0,
            .key = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
            .ctr = {0x8a, 0xb5, 0xf2, 0x20, 0xff, 0xaf, 0x37, 0x35}
        }
    };
    return (const ota_app_key_t*)&app_key;
}

#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
static bool is_array_zero(uint8_t arr[], uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        if (arr[i] != 0) {
            return false;
        }
    }
    return true;
}

static void app1_auto_make_hash(hpm_app_header_t *app1_header, hpm_app_header_t *app2_header)
{
    hpm_app_header_t temp_header = {0};
    memcpy(&temp_header, app1_header, sizeof(hpm_app_header_t));
    temp_header.hash_type = app2_header->diff_header.old_hash_type;
    temp_header.len = app2_header->diff_header.old_img_len;
    if(!hpm_ota_package_makehash(FLASH_USER_APP1_ADDR +  + sizeof(hpm_app_header_t), temp_header.len, &temp_header))
    {
        HPM_LOG_WRN("app1 auto makehash fail!\r\n");
        return;
    }
    if(memcmp(temp_header.hash_data, app2_header->diff_header.old_hash_data, HPM_APP_HASH_DATA_MAXSIZE) != 0)
    {  //当前固件和旧固件不相同
        HPM_LOG_INFO("app1 auto makehash: Current firmware is not the same as the diff old firmware!\r\n");
        return;
    }
    temp_header.hash_enable = 1;
    temp_header.pwr_hash = 1;
    hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));
    hpm_flash_write(FLASH_USER_APP1_ADDR, (const unsigned char *)&temp_header, sizeof(hpm_app_header_t));

    hpm_ota_get_header_info_of_app(HPM_APP1, app1_header);
    return;
}

static bool app_diffupgrade_complete(uint8_t index, hpm_app_header_t *app_header, hpm_app_header_t *diff_header)
{
    hpm_app_header_t temp_header = {0};
    memcpy(&temp_header, diff_header, sizeof(hpm_app_header_t));
    temp_header.version = diff_header->diff_header.new_version;
    temp_header.len = diff_header->diff_header.new_img_len;
    temp_header.hash_type = diff_header->diff_header.new_hash_type;
    temp_header.type = OTA_TYPE_RAM_SET(OTA_TYPE_RAM_GET(diff_header->type)) |
                       OTA_TYPE_EXIP_SET(OTA_TYPE_EXIP_GET(diff_header->type)) |
                       OTA_TYPE_BODY_SET(OTA_TYPE_NORMAL_FIRMWARE);
    memcpy(temp_header.hash_data, diff_header->diff_header.new_hash_data, HPM_APP_HASH_DATA_MAXSIZE);
    if(index == HPM_APP1)
    {
        if(hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t)) < 0)
            return false;
        if(hpm_flash_write(FLASH_USER_APP1_ADDR, (const unsigned char *)&temp_header, sizeof(hpm_app_header_t)) < 0)
            return false;

        hpm_ota_get_header_info_of_app(HPM_APP1, app_header);
        return true;
    }
    if(index == HPM_APP2)
    {
        if(hpm_flash_erase_and_rewrite(FLASH_USER_APP2_ADDR, sizeof(hpm_app_header_t)) < 0)
            return false;
        if(hpm_flash_write(FLASH_USER_APP2_ADDR, (const unsigned char *)&temp_header, sizeof(hpm_app_header_t)) < 0)
            return false;

        hpm_ota_get_header_info_of_app(HPM_APP2, app_header);
        return true;
    }
    
    return false;
}

static bool app_lzmaupgrade_complete(hpm_app_header_t *app1_header, hpm_app_header_t *app2_header)
{
    memcpy(app1_header, app2_header, sizeof(hpm_app_header_t));
    app1_header->version = app2_header->lzma_header.version;
    app1_header->len = app2_header->lzma_header.img_len;
    app1_header->hash_type = app2_header->lzma_header.hash_type;
    app1_header->type = OTA_TYPE_RAM_SET(OTA_TYPE_RAM_GET(app2_header->type)) |
                        OTA_TYPE_EXIP_SET(OTA_TYPE_EXIP_GET(app2_header->type)) |
                        OTA_TYPE_BODY_SET(OTA_TYPE_NORMAL_FIRMWARE);
    memcpy(app1_header->hash_data, app2_header->lzma_header.hash_data, HPM_APP_HASH_DATA_MAXSIZE);

    if(hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t)) < 0)
        return false;
    if(hpm_flash_write(FLASH_USER_APP1_ADDR, (const unsigned char *)app1_header, sizeof(hpm_app_header_t)) < 0)
        return false;

    hpm_ota_get_header_info_of_app(HPM_APP1, app1_header);
    return true;
}
#endif

/**
 * @brief Calculate the firmware area to be jumped;PBUT KEY为释放状态时，对各固件进行签名头部以及HASH校验通过，根据版本信息决定待执行的固件区
 *
 * @param [in] pbutn_enable PBUT KEY status;PBUT KEY状态
 */
int bootuser_get_jump_app(bool pbutn_enable)
{
    uint8_t bit = 0;
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
    static uint8_t retry_count = 0;
#endif
    hpm_app_header_t app1_header = {0};
    hpm_app_header_t app2_header = {0};
    hpm_app_header_t diff_header = {0};

    if (pbutn_enable && bootuser_check_pbutn_bootmode())
    {
        HPM_LOG_INFO("pbut press!\r\n");
        return -1;
    }

    hpm_ota_get_header_info_of_app(HPM_APP1, &app1_header);
    hpm_ota_get_header_info_of_app(HPM_APP2, &app2_header);
    hpm_ota_get_header_info_of_addr(FLASH_DIFF_APP_ADDR, &diff_header);
    if (app1_header.magic == HPM_APP_FILE_FLAG_MAGIC)
    {
        bit |= BIT(2);
        HPM_LOG_INFO("app1 type:0x%02x, ver:%d, hash_type:%d\r\n", app1_header.type, app1_header.version, app1_header.hash_type);
    }
    if (app2_header.magic == HPM_APP_FILE_FLAG_MAGIC)
    {
        bit |= BIT(1);
        HPM_LOG_INFO("app2 type:0x%02x, ver:%d, hash_type:%d\r\n", app2_header.type, app2_header.version, app2_header.hash_type);
    }
    if (diff_header.magic == HPM_APP_FILE_FLAG_MAGIC)
    {
        bit |= BIT(0);
        HPM_LOG_INFO("diff type:0x%02x, ver:%d, hash_type:%d, old img ver:%d, new img ver:%d\r\n",
               diff_header.type, diff_header.version, diff_header.hash_type,
               diff_header.diff_header.old_version,
               diff_header.diff_header.new_version);
    }

    HPM_LOG_INFO("bit:%d\r\n", bit);

    switch(bit)
    {
        case 7: //111
        {
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
            //注意：无hash头部信息，则认为是直接烧录的固件，这里重新计算hash，方便后面直接差分升级
            /**Note: If there is no hash header information, the firmware is considered to be burned directly,
             and the hash is recalculated here to facilitate the direct differential upgrade later  */
            if(is_array_zero(app1_header.hash_data, HPM_APP_HASH_DATA_MAXSIZE))
            {
                HPM_LOG_INFO("app1 auto make hash!\r\n");
                app1_auto_make_hash(&app1_header, &diff_header);
            }

            if(diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC && 
               app1_header.version >= diff_header.diff_header.new_version &&
               app2_header.version <= app1_header.version)
            {
                if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                {
                    return HPM_APP1;
                }
            }

            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_NORMAL_FIRMWARE && 
               diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC &&
               app2_header.version >= diff_header.diff_header.new_version)
            {
                if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    return HPM_APP2;
                }
            }

            if(diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC && bootuser_verify_app_valid(FLASH_DIFF_APP_ADDR, &diff_header))
            {
                if((app1_header.hash_type == diff_header.diff_header.old_hash_type) &&
                   (memcmp(app1_header.hash_data, diff_header.diff_header.old_hash_data, HPM_APP_HASH_DATA_MAXSIZE) == 0) &&
                   bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                {
                    hpm_flash_erase_and_rewrite(FLASH_USER_APP2_ADDR, sizeof(hpm_app_header_t));
                    if(hpm_diff_upgrade_restore(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app1_header.len,
                                                FLASH_DIFF_APP_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), diff_header.len,
                                                FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t)) >= 0) 
                    {
                        HPM_LOG_INFO("Differential firmware app2 restore successful!\r\n");
                        if(app_diffupgrade_complete(HPM_APP2, &app2_header, &diff_header))
                        {
                            if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                            {
                                return HPM_APP2;
                            }
                        }
                        else
                        {
                            HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! diff upgrade restore fail!\r\n");
                    }

                    if(retry_count >= HPM_OTA_RETRY_COUNT)
                    {
                        HPM_LOG_ERR("BAD! diff restore Error; retry:%d!\r\n", retry_count);
                        return -1;
                    }
                    else
                    {
                        retry_count++;
                        bootuser_get_jump_app(false);
                    }
                }
                if((app2_header.hash_type == diff_header.diff_header.old_hash_type) &&
                   (memcmp(app2_header.hash_data, diff_header.diff_header.old_hash_data, HPM_APP_HASH_DATA_MAXSIZE) == 0) &&
                   bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));   
                    if(hpm_diff_upgrade_restore(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.len,
                                                FLASH_DIFF_APP_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), diff_header.len,
                                                FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t)) >= 0) 
                    {
                        HPM_LOG_INFO("Differential firmware app1 restore successful!\r\n");
                        if(app_diffupgrade_complete(HPM_APP1, &app1_header, &diff_header))
                        {
                            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                            {
                                return HPM_APP1;
                            }
                        }
                        else
                        {
                            HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! diff upgrade restore fail!\r\n");
                    }
                    if(retry_count >= HPM_OTA_RETRY_COUNT)
                    {
                        HPM_LOG_WRN("BAD! diff restore Error; retry:%d!\r\n", retry_count);
                        return -1;
                    }
                    else
                    {
                        retry_count++;
                        bootuser_get_jump_app(false);
                    }
                }
                HPM_LOG_ERR("BAD! Differential old firmware and current firmware have inconsistent hash values or inconsistent hash types and cannot be differentially upgraded!\r\n");
            }

            //lzma update
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_LZMA_FIRMWARE && 
               app2_header.lzma_header.magic == HPM_APP_LZMA_FLAG_MAGIC &&
               app2_header.version != app1_header.version &&
               bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
            {
                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));
                //lzma update
                if(hpm_lzma_upgrade_restore(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.len, \
                                         FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.lzma_header.img_len) >= 0)
                {
                    HPM_LOG_INFO("lzma firmware app1 restore successful!\r\n");
                    if(app_lzmaupgrade_complete(&app1_header, &app2_header))
                    {
                        if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                        {
                            return HPM_APP1;
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                    }
                }
                else
                {
                    HPM_LOG_ERR("BAD! lzma upgrade restore fail!\r\n");
                }

                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));

                if(retry_count >= HPM_OTA_RETRY_COUNT)
                {
                    HPM_LOG_WRN("BAD! lzma restore Error; retry:%d!\r\n", retry_count);
                    return -1;
                }
                else
                {
                    retry_count++;
                    bootuser_get_jump_app(false);
                }
            }
#endif
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_NORMAL_FIRMWARE)
            {
                HPM_LOG_INFO("ver1:%d,ver2:%d\r\n", app1_header.version, app2_header.version);
                if (app1_header.version >= app2_header.version)
                {
                    if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                    {
                        return HPM_APP1;
                    }
                }
                #ifdef FLASH_USER_APP2_ADDR
                if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    return HPM_APP2;
                }
                #endif
            }
            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
            {
                return HPM_APP1;
            }
        }
        break;
        case 6: //110
        {
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
            //lzma update
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_LZMA_FIRMWARE && 
               app2_header.lzma_header.magic == HPM_APP_LZMA_FLAG_MAGIC &&
               app2_header.version != app1_header.version &&
               bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
            {
                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));
                //lzma update
                if(hpm_lzma_upgrade_restore(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.len, \
                                         FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.lzma_header.img_len) >= 0)
                {
                    HPM_LOG_INFO("lzma firmware app1 restore successful!\r\n");
                    if(app_lzmaupgrade_complete(&app1_header, &app2_header))
                    {
                        if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                        {
                            return HPM_APP1;
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                    }
                }
                else
                {
                    HPM_LOG_ERR("BAD! lzma upgrade restore fail!\r\n");
                }

                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));

                if(retry_count >= HPM_OTA_RETRY_COUNT)
                {
                    HPM_LOG_WRN("BAD! lzma restore Error; retry:%d!\r\n", retry_count);
                    return -1;
                }
                else
                {
                    retry_count++;
                    bootuser_get_jump_app(false);
                }
            }
#endif
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_NORMAL_FIRMWARE)
            {
                HPM_LOG_INFO("ver1:%d,ver2:%d\r\n", app1_header.version, app2_header.version);
                if (app1_header.version >= app2_header.version)
                {
                    if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                    {
                        return HPM_APP1;
                    }
                }
                #ifdef FLASH_USER_APP2_ADDR
                if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    return HPM_APP2;
                }
                #endif
            }
            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
            {
                return HPM_APP1;
            }
        }
        break;
        case 5: //101
        {
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
            //注意：无hash头部信息，则认为是直接烧录的固件，这里重新计算hash，方便后面直接差分升级
            /**Note: If there is no hash header information, the firmware is considered to be burned directly,
             and the hash is recalculated here to facilitate the direct differential upgrade later  */
            if(is_array_zero(app1_header.hash_data, HPM_APP_HASH_DATA_MAXSIZE))
            {
                app1_auto_make_hash(&app1_header, &diff_header);
            }

            if(diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC && app1_header.version >= diff_header.diff_header.new_version)
            {
                if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                {
                    return HPM_APP1;
                }
            }

            if(diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC && bootuser_verify_app_valid(FLASH_DIFF_APP_ADDR, &diff_header))
            {
                if(bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header) &&
                   (app1_header.hash_type == diff_header.diff_header.old_hash_type) &&
                   (memcmp(app1_header.hash_data, diff_header.diff_header.old_hash_data, HPM_APP_HASH_DATA_MAXSIZE) == 0))
                {
                    hpm_flash_erase_and_rewrite(FLASH_USER_APP2_ADDR, sizeof(hpm_app_header_t));
                    if(hpm_diff_upgrade_restore(FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app1_header.len,
                                                FLASH_DIFF_APP_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), diff_header.len,
                                                FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t)) >= 0) 
                    {
                        HPM_LOG_INFO("Differential firmware app2 restore successful!\r\n");
                        if(app_diffupgrade_complete(HPM_APP2, &app2_header, &diff_header))
                        {
                            if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                            {
                                return HPM_APP2;
                            }
                        }
                        else
                        {
                            HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! diff upgrade restore fail!\r\n");
                    }

                    if(retry_count >= HPM_OTA_RETRY_COUNT)
                    {
                        HPM_LOG_WRN("BAD! diff restore Error; retry:%d!\r\n", retry_count);
                        return -1;
                    }
                    else
                    {
                        retry_count++;
                        bootuser_get_jump_app(false);
                    }
                }
                HPM_LOG_ERR("BAD! Differential old firmware and current firmware have inconsistent hash values or inconsistent hash types and cannot be differentially upgraded!\r\n");
            }
#endif
            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
            {
                return HPM_APP1;
            }
        }
        break;
        case 4: //100
        {
            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
            {
                return HPM_APP1;
            }
        }
        break;
        case 3: //011
        {
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_NORMAL_FIRMWARE && diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC && \
               app2_header.version >= diff_header.diff_header.new_version)
            {
                if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    return HPM_APP2;
                }
            }

            if(diff_header.diff_header.magic == HPM_APP_DIFF_FLAG_MAGIC && bootuser_verify_app_valid(FLASH_DIFF_APP_ADDR, &diff_header))
            {
                if(bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header) &&
                   (app2_header.hash_type == diff_header.diff_header.old_hash_type) &&
                   (memcmp(app2_header.hash_data, diff_header.diff_header.old_hash_data, HPM_APP_HASH_DATA_MAXSIZE) == 0))
                {
                    hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));
                    if(hpm_diff_upgrade_restore(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.len,
                                                FLASH_DIFF_APP_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), diff_header.len,
                                                FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t)) >= 0) 
                    {
                        HPM_LOG_INFO("Differential firmware app1 restore successful!\r\n");
                        if(app_diffupgrade_complete(HPM_APP1, &app1_header, &diff_header))
                        {
                            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                            {
                                return HPM_APP1;
                            }
                        }
                        else
                        {
                            HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! diff upgrade restore fail!\r\n");
                    }
                    if(retry_count >= HPM_OTA_RETRY_COUNT)
                    {
                        HPM_LOG_WRN("BAD! diff restore Error; retry:%d!\r\n", retry_count);
                        return -1;
                    }
                    else
                    {
                        retry_count++;
                        bootuser_get_jump_app(false);
                    }
                }
                HPM_LOG_ERR("BAD! Differential old firmware and current firmware have inconsistent hash values or inconsistent hash types and cannot be differentially upgraded!\r\n");
            }

            //lzma update
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_LZMA_FIRMWARE &&
               app2_header.lzma_header.magic == HPM_APP_LZMA_FLAG_MAGIC &&
               bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
            {
                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));
                //lzma update
                if(hpm_lzma_upgrade_restore(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.len, \
                                         FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.lzma_header.img_len) >= 0)
                {
                    HPM_LOG_INFO("lzma firmware app1 restore successful!\r\n");
                    if(app_lzmaupgrade_complete(&app1_header, &app2_header))
                    {
                        if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                        {
                            return HPM_APP1;
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                    }
                }
                else
                {
                    HPM_LOG_ERR("BAD! lzma upgrade restore fail!\r\n");
                }

                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));

                if(retry_count >= HPM_OTA_RETRY_COUNT)
                {
                    HPM_LOG_WRN("BAD! lzma restore Error; retry:%d!\r\n", retry_count);
                    return -1;
                }
                else
                {
                    retry_count++;
                    bootuser_get_jump_app(false);
                }
            }
#endif
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_NORMAL_FIRMWARE)
            {
                #ifdef FLASH_USER_APP2_ADDR
                if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    return HPM_APP2;
                }
                #endif
            }
        }
        break;
        case 2: //010
        {
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
            //lzma update
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_LZMA_FIRMWARE &&
               app2_header.lzma_header.magic == HPM_APP_LZMA_FLAG_MAGIC &&
               bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
            {
                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));
                //lzma update
                if(hpm_lzma_upgrade_restore(FLASH_USER_APP2_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.len, \
                                         FLASH_USER_APP1_ADDR + FLASH_ADDR_BASE + sizeof(hpm_app_header_t), app2_header.lzma_header.img_len) >= 0)
                {
                    HPM_LOG_INFO("lzma firmware app1 restore successful!\r\n");
                    if(app_lzmaupgrade_complete(&app1_header, &app2_header))
                    {
                        if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, &app1_header))
                        {
                            return HPM_APP1;
                        }
                    }
                    else
                    {
                        HPM_LOG_ERR("BAD! flash write app header fail!\r\n");
                    }
                }
                else
                {
                    HPM_LOG_ERR("BAD! lzma upgrade restore fail!\r\n");
                }

                hpm_flash_erase_and_rewrite(FLASH_USER_APP1_ADDR, sizeof(hpm_app_header_t));

                if(retry_count >= HPM_OTA_RETRY_COUNT)
                {
                    HPM_LOG_WRN("BAD! lzma restore Error; retry:%d!\r\n", retry_count);
                    return -1;
                }
                else
                {
                    retry_count++;
                    bootuser_get_jump_app(false);
                }
            }
#endif
            if(OTA_TYPE_BODY_GET(app2_header.type) == OTA_TYPE_NORMAL_FIRMWARE)
            {
                #ifdef FLASH_USER_APP2_ADDR
                if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, &app2_header))
                {
                    return HPM_APP2;
                }
                #endif
            }
        }
        break;
        case 1: //001
        case 0: //000
        break;
        default:
        break;
    }
    return -1;
}

int main(void)
{
    int ret = 0;
    int app_index = 0;
     /* Bsp initialization */
    board_init();

     /* Gpio pin initialization */
    board_init_gpio_pins();

    HPM_LOG_INFO("boot user\r\n");

    /* Platform flash initialization */
    ret = hpm_flash_init();
    if (ret != 0)
    {
        HPM_LOG_ERR("BAD! flash init fail:%d\r\n", ret);
        while (1)
        {
        }
    }
    /* Calculate the firmware area to be jumped */
    app_index = bootuser_get_jump_app(true);

    HPM_LOG_INFO("APP index:%d\n", app_index);
    if (app_index >= 0)
    {
        /* hpm app jump */
        hpm_appindex_jump((uint8_t)app_index);
    }

    HPM_LOG_INFO("bootuser mode!\r\n");

    hpm_ota_init();

    while(1)
    {
        hpm_ota_polling_handle();
    }
    return 0;
}

