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
#include "hpm_gpio_drv.h"
#include "hpm_ota.h"
#include "ota_port.h"

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


int main(void)
{
    board_init();
    board_init_led_pins();

    rgb_timer_create(LED_FLASH_PERIOD_IN_MS);

    printf("__DATE__:%s, __TIME__:%s\r\n", __DATE__, __TIME__);

    printf("hello world, THIS OTA%d\n", hpm_ota_get_nowrunning_app());

    hpm_ota_init();
    while(1)
    {
        hpm_ota_polling_handle();
    }
    return 0;
}

