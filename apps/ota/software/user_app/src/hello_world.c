/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_gptmr_drv.h"
#include "hpm_platform.h"
#include "hpm_flashmap.h"
#include "hpm_ota.h"
#include "hpm_gpio_drv.h"
#include "uart_channel.h"
#include "enet_channel.h"
#include "usb_channel.h"
#if defined(CONFIG_ECAT_FOE_CHANNEL) && CONFIG_ECAT_FOE_CHANNEL
#include "ecat_foe.h"
#endif

#define LED_FLASH_PERIOD_IN_MS 300

void rgb_timer_isr(void)
{
    if (gptmr_check_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(1))) {
        gptmr_clear_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(1));
        board_led_toggle();
    }
}
SDK_DECLARE_EXT_ISR_M(IRQn_GPTMR1, rgb_timer_isr);

void rgb_timer_create(uint32_t ms)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(HPM_GPTMR1, &config);

    clock_add_to_group(clock_gptmr1, 0);
    gptmr_freq = clock_get_frequency(clock_gptmr1);

    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(HPM_GPTMR1, 1, &config, false);
    gptmr_enable_irq(HPM_GPTMR1, GPTMR_CH_RLD_IRQ_MASK(1));
    intc_m_enable_irq_with_priority(IRQn_GPTMR1, 5);

    gptmr_start_counter(HPM_GPTMR1, 1);
}

#if defined(CONFIG_USB_DEVICE_CHANNEL) && CONFIG_USB_DEVICE_CHANNEL
 /* Write file to flash */
static int writefile_addr(unsigned int addr, unsigned char* data, int len)
{
    /* First packet contain file header info */
    hpm_ota_auto_write_of_addr(addr, (void*)data, len, true);
    return 0;
}
#else
 /* Write file to flash */
static int writefile(unsigned char* data, int len)
{
    /* First packet contain file header info */
    hpm_ota_auto_write((void*)data, len, true);
    return 0;
}
#endif

int main(void)
{
    int ret;
    board_init();
    board_init_led_pins();

    rgb_timer_create(LED_FLASH_PERIOD_IN_MS);

    printf("hello world, THIS OTA%d\n", hpm_ota_get_nowrunning_appindex());

#ifndef CONFIG_USE_HPM5300
    ret = hpm_platform_flash_init();
    if (ret != 0)
    {
        printf("BAD! flash init fail:%d\r\n");
        while (1)
        {
        }
    }

#if defined(CONFIG_UART_CHANNEL) && CONFIG_UART_CHANNEL
    hpm_uart_channel_init();
    hpm_uart_block_task(writefile);
#elif defined(CONFIG_ENET_CHANNEL) && CONFIG_ENET_CHANNEL
    hpm_enet_channel_init();
    hpm_enet_block_task(writefile);
#elif defined(CONFIG_USB_DEVICE_CHANNEL) && CONFIG_USB_DEVICE_CHANNEL
    hpm_usb_device_channel_init();
    hpm_usb_device_block_task(writefile_addr);
#elif defined(CONFIG_USB_HOST_CHANNEL) && CONFIG_USB_HOST_CHANNEL
    hpm_usb_host_channel_init();
    hpm_usb_host_block_task(writefile);
#elif defined(CONFIG_ECAT_FOE_CHANNEL) && CONFIG_ECAT_FOE_CHANNEL
    hpm_ecat_foe_init();
    hpm_ecat_foe_task();
#endif
#endif

    while (1)
    {
        
    }
    return 0;
}

