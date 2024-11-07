/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "hpm_platform.h"
#include "hpm_flashmap.h"
#include "hpm_ota.h"
#include "hpm_gpio_drv.h"
#include "boot_user.h"
#include "uart_channel.h"
#include "enet_channel.h"
#include "usb_channel.h"
#if defined(CONFIG_ECAT_FOE_CHANNEL) && CONFIG_ECAT_FOE_CHANNEL
#include "ecat_foe.h"
#endif



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
            printf("APP%d, verify SUCCESS!\r\n", (addr == FLASH_USER_APP1_ADDR) ? 0 : 1);
            return true;
        }
    }
    else
    {
        return true;
    }
    printf("APP%d, valify FAIL!\r\n", (addr == FLASH_USER_APP1_ADDR) ? 0 : 1);
    return false;
}

/**
 * @brief Calculate the firmware area to be jumped;PBUT KEY为释放状态时，对各固件进行签名头部以及HASH校验通过，根本版本信息决定待执行的固件区
 *
 * @param [in] pbutn_enable PBUT KEY status;PBUT KEY状态
 */
int bootuser_check_jump_app(bool pbutn_enable)
{
    uint8_t bit = 0;
    hpm_app_header_t *app1_header = NULL;
    hpm_app_header_t *app2_header = NULL;

    if (pbutn_enable && bootuser_check_pbutn_bootmode())
    {
        printf("pbut press!\r\n");
        return -1;
    }
    app1_header = hpm_ota_get_flash_header_info(HPM_APP1);
    app2_header = hpm_ota_get_flash_header_info(HPM_APP2);
    if (app1_header != NULL && app1_header->magic == HPM_APP_FILE_FLAG_MAGIC)
    {
        bit |= BIT(0);
    }
    if (app2_header != NULL && app2_header->magic == HPM_APP_FILE_FLAG_MAGIC)
    {
        bit |= BIT(1);
    }
    if (bit == 3)//
    {
        printf("ver1:%d,ver2:%d\r\n", app1_header->version, app2_header->version);
        if (app1_header->version >= app2_header->version)
        {
            if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, app1_header))
            {
                return HPM_APP1;
            }
        }
#ifdef FLASH_USER_APP2_ADDR
        if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, app2_header))
        {
            return HPM_APP2;
        }
#endif
        if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, app1_header))
        {
            return HPM_APP1;
        }
    }
    else if (bit == 2)
    {
#ifdef FLASH_USER_APP2_ADDR
        if (bootuser_verify_app_valid(FLASH_USER_APP2_ADDR, app2_header))
        {
            return HPM_APP2;
        }
#endif
    }
    else if (bit == 1)
    {
        if (bootuser_verify_app_valid(FLASH_USER_APP1_ADDR, app1_header))
        {
            return HPM_APP1;
        }
    }
    return -1;
}

#if defined(CONFIG_USB_DEVICE_CHANNEL) && CONFIG_USB_DEVICE_CHANNEL

 /* Write file to flash */
static int boot_writefile_addr(unsigned int addr, unsigned char* data, int len)
{
    /* First packet contain file header info */
    hpm_ota_auto_write_of_addr(addr, (void*)data, len, true);
    return 0;
}

#else

static uint8_t reboot = 0;
static int boot_writefile(unsigned char *data, int len)
{
    /* First packet contain file header info */
    if(hpm_ota_auto_write((void *)data, len, true) == 1)
    {
        reboot = 1;
    }
    return 0;
}
#endif

int main(void)
{
    int ret = 0;
    int boot_index = 0;
     /* Bsp initialization */
    board_init();

     /* Gpio pin initialization */
    board_init_gpio_pins();

    printf("boot user\r\n");

    /* Platform flash initialization */
    ret = hpm_platform_flash_init();
    if (ret != 0)
    {
        printf("BAD! flash init fail:%d\r\n");
        while (1)
        {
        }
    }
    /* Calculate the firmware area to be jumped */
    boot_index = bootuser_check_jump_app(true);

    printf("APP index:%d\n", boot_index);
    if (boot_index >= 0)
    {
        /* hpm app jump */
        hpm_appindex_jump((uint8_t)boot_index);
    }

    printf("bootuser mode!\r\n");
#if defined(CONFIG_UART_CHANNEL) && CONFIG_UART_CHANNEL
    /* uart channel init */
    hpm_uart_channel_init();
    /* ota write to flash*/
    hpm_uart_block_task(boot_writefile);
    if(reboot)
    {
        hpm_ota_soc_reset();
    }
#elif defined(CONFIG_ENET_CHANNEL) && CONFIG_ENET_CHANNEL
    hpm_enet_channel_init();
    hpm_enet_block_task(boot_writefile);
#elif defined(CONFIG_USB_DEVICE_CHANNEL) && CONFIG_USB_DEVICE_CHANNEL
    hpm_usb_device_channel_init();
    hpm_usb_device_block_task(boot_writefile_addr);
#elif defined(CONFIG_USB_HOST_CHANNEL) && CONFIG_USB_HOST_CHANNEL
    hpm_usb_host_channel_init();
    hpm_usb_host_block_task(boot_writefile);
#elif defined(CONFIG_ECAT_FOE_CHANNEL) && CONFIG_ECAT_FOE_CHANNEL
    hpm_ecat_foe_init();
    hpm_ecat_foe_task();
#endif
    while (1)
    {
    }
    return 0;
}

