/*
 * Copyright (c) 2023 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_uart_drv.h"
#include "hpm_sdk_version.h"
#include "hpm_gptmr_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_usb_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_pllctlv2_drv.h"

static board_timer_cb timer_cb;

#if defined(FLASH_XIP) && FLASH_XIP
__attribute__ ((section(".nor_cfg_option"))) const uint32_t option[4] = {0xfcf90002, 0x00000006, 0x1000, 0x0};
#endif

#if defined(FLASH_UF2) && FLASH_UF2
ATTR_PLACE_AT(".uf2_signature") const uint32_t uf2_signature = BOARD_UF2_SIGNATURE;
#endif

void board_init_console(void)
{
#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#if BOARD_CONSOLE_TYPE == CONSOLE_TYPE_UART
    console_config_t cfg;

    /* uart needs to configure pin function before enabling clock, otherwise the level change of
     * uart rx pin when configuring pin function will cause a wrong data to be received.
     * And a uart rx dma request will be generated by default uart fifo dma trigger level.
     */
    init_uart_pins((UART_Type *) BOARD_CONSOLE_BASE);

    /* Configure the UART clock to 24MHz */
    clock_set_source_divider(BOARD_CONSOLE_CLK_NAME, clk_src_osc24m, 1U);
    clock_add_to_group(BOARD_CONSOLE_CLK_NAME, 0);

    cfg.type = BOARD_CONSOLE_TYPE;
    cfg.base = (uint32_t)BOARD_CONSOLE_BASE;
    cfg.src_freq_in_hz = clock_get_frequency(BOARD_CONSOLE_CLK_NAME);
    cfg.baudrate = BOARD_CONSOLE_BAUDRATE;

    if (status_success != console_init(&cfg)) {
        /* failed to  initialize debug console */
        while (1) {
        }
    }
#else
    while (1)
        ;
#endif
#endif
}

void board_print_banner(void)
{
    const uint8_t banner[] = "\n"
"----------------------------------------------------------------------\n"
"$$\\   $$\\ $$$$$$$\\  $$\\      $$\\ $$\\\n"
"$$ |  $$ |$$  __$$\\ $$$\\    $$$ |\\__|\n"
"$$ |  $$ |$$ |  $$ |$$$$\\  $$$$ |$$\\  $$$$$$$\\  $$$$$$\\   $$$$$$\\\n"
"$$$$$$$$ |$$$$$$$  |$$\\$$\\$$ $$ |$$ |$$  _____|$$  __$$\\ $$  __$$\\\n"
"$$  __$$ |$$  ____/ $$ \\$$$  $$ |$$ |$$ /      $$ |  \\__|$$ /  $$ |\n"
"$$ |  $$ |$$ |      $$ |\\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |\n"
"$$ |  $$ |$$ |      $$ | \\_/ $$ |$$ |\\$$$$$$$\\ $$ |      \\$$$$$$  |\n"
"\\__|  \\__|\\__|      \\__|     \\__|\\__| \\_______|\\__|       \\______/\n"
"----------------------------------------------------------------------\n";
#ifdef SDK_VERSION_STRING
    printf("hpm_sdk: %s\n", SDK_VERSION_STRING);
#endif
    printf("%s", banner);
}

void board_print_clock_freq(void)
{
    printf("==============================\n");
    printf(" %s clock summary\n", BOARD_NAME);
    printf("==============================\n");
    printf("cpu0:\t\t %dHz\n", clock_get_frequency(clock_cpu0));
    printf("ahb:\t\t %dHz\n", clock_get_frequency(clock_ahb));
    printf("mchtmr0:\t %dHz\n", clock_get_frequency(clock_mchtmr0));
    printf("xpi0:\t\t %dHz\n", clock_get_frequency(clock_xpi0));
    printf("==============================\n");
}

void board_init(void)
{
    board_init_clock();
    board_init_console();
    board_init_pmp();
#if BOARD_SHOW_CLOCK
    board_print_clock_freq();
#endif
#if BOARD_SHOW_BANNER
    board_print_banner();
#endif
}

void board_init_clock(void)
{
    uint32_t cpu0_freq = clock_get_frequency(clock_cpu0);
    if (cpu0_freq == PLLCTL_SOC_PLL_REFCLK_FREQ) {
        /* Configure the External OSC ramp-up time: ~9ms */
        pllctlv2_xtal_set_rampup_time(HPM_PLLCTLV2, 32UL * 1000UL * 9U);

        /* Select clock setting preset1 */
        sysctl_clock_set_preset(HPM_SYSCTL, 2);
    }

    /* group0[0] */
    clock_add_to_group(clock_cpu0, 0);
    clock_add_to_group(clock_ahb, 0);
    clock_add_to_group(clock_lmm0, 0);
    clock_add_to_group(clock_mchtmr0, 0);
    clock_add_to_group(clock_rom, 0);
    clock_add_to_group(clock_can0, 0);
    clock_add_to_group(clock_can1, 0);
    clock_add_to_group(clock_can2, 0);
    clock_add_to_group(clock_can3, 0);
    clock_add_to_group(clock_ptpc, 0);
    clock_add_to_group(clock_gptmr0, 0);
    clock_add_to_group(clock_gptmr1, 0);
    clock_add_to_group(clock_gptmr2, 0);
    clock_add_to_group(clock_gptmr3, 0);
    clock_add_to_group(clock_i2c0, 0);
    clock_add_to_group(clock_i2c1, 0);
    clock_add_to_group(clock_i2c2, 0);
    clock_add_to_group(clock_i2c3, 0);
    clock_add_to_group(clock_spi0, 0);
    clock_add_to_group(clock_spi1, 0);
    clock_add_to_group(clock_spi2, 0);
    clock_add_to_group(clock_spi3, 0);
    clock_add_to_group(clock_uart0, 0);
    clock_add_to_group(clock_uart1, 0);
    clock_add_to_group(clock_uart2, 0);
    clock_add_to_group(clock_uart3, 0);
    clock_add_to_group(clock_uart4, 0);
    clock_add_to_group(clock_uart5, 0);
    clock_add_to_group(clock_uart6, 0);
    /* group0[1] */
    clock_add_to_group(clock_uart7, 0);
    clock_add_to_group(clock_watchdog0, 0);
    clock_add_to_group(clock_watchdog1, 0);
    clock_add_to_group(clock_mbx0, 0);
    clock_add_to_group(clock_tsns, 0);
    clock_add_to_group(clock_crc0, 0);
    clock_add_to_group(clock_adc0, 0);
    clock_add_to_group(clock_adc1, 0);
    clock_add_to_group(clock_dac0, 0);
    clock_add_to_group(clock_dac1, 0);
    clock_add_to_group(clock_acmp, 0);
    clock_add_to_group(clock_opa0, 0);
    clock_add_to_group(clock_opa1, 0);
    clock_add_to_group(clock_mot0, 0);
    clock_add_to_group(clock_rng, 0);
    clock_add_to_group(clock_sdp, 0);
    clock_add_to_group(clock_kman, 0);
    clock_add_to_group(clock_gpio, 0);
    clock_add_to_group(clock_hdma, 0);
    clock_add_to_group(clock_xpi0, 0);
    clock_add_to_group(clock_usb0, 0);

    /* Connect Group0 to CPU0 */
    clock_connect_group_to_cpu(0, 0);

    clock_update_core_clock();

    /* Configure mchtmr to 24MHz */
    clock_set_source_divider(clock_mchtmr0, clk_src_osc24m, 1);
}

void board_delay_us(uint32_t us)
{
    clock_cpu_delay_us(us);
}

void board_delay_ms(uint32_t ms)
{
    clock_cpu_delay_ms(ms);
}

void board_timer_isr(void)
{
    if (gptmr_check_status(BOARD_CALLBACK_TIMER, GPTMR_CH_RLD_STAT_MASK(BOARD_CALLBACK_TIMER_CH))) {
        gptmr_clear_status(BOARD_CALLBACK_TIMER, GPTMR_CH_RLD_STAT_MASK(BOARD_CALLBACK_TIMER_CH));
        timer_cb();
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_CALLBACK_TIMER_IRQ, board_timer_isr);

void board_timer_create(uint32_t ms, board_timer_cb cb)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    timer_cb = cb;
    gptmr_channel_get_default_config(BOARD_CALLBACK_TIMER, &config);

    clock_add_to_group(BOARD_CALLBACK_TIMER_CLK_NAME, 0);
    gptmr_freq = clock_get_frequency(BOARD_CALLBACK_TIMER_CLK_NAME);

    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(BOARD_CALLBACK_TIMER, BOARD_CALLBACK_TIMER_CH, &config, false);
    gptmr_enable_irq(BOARD_CALLBACK_TIMER, GPTMR_CH_RLD_IRQ_MASK(BOARD_CALLBACK_TIMER_CH));
    intc_m_enable_irq_with_priority(BOARD_CALLBACK_TIMER_IRQ, 1);

    gptmr_start_counter(BOARD_CALLBACK_TIMER, BOARD_CALLBACK_TIMER_CH);
}


void board_init_usb_pins(void)
{
    init_usb_pins();
    /* As QFN48 has no vbus pin, so should be call usb_phy_using_internal_vbus() API to use internal vbus. */
    /* usb_phy_using_internal_vbus(BOARD_USB); */
}

void board_init_uart(UART_Type *ptr)
{
    /* configure uart's pin before opening uart's clock */
    init_uart_pins(ptr);
    board_init_uart_clock(ptr);
}


void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level)
{
}

uint32_t board_init_adc16_clock(ADC16_Type *ptr, bool clk_src_ahb)
{
    uint32_t freq = 0;

    if (ptr == HPM_ADC0) {
        if (clk_src_ahb) {
            /* Configure the ADC clock from AHB (@200MHz by default)*/
            clock_set_adc_source(clock_adc0, clk_adc_src_ahb0);
        } else {
            /* Configure the ADC clock from pll0_clk0 divided by 2 (@200MHz by default) */
            clock_set_adc_source(clock_adc0, clk_adc_src_ana0);
            clock_set_source_divider(clock_ana0, clk_src_pll0_clk2, 2U);
        }

        freq = clock_get_frequency(clock_adc0);
    } else if (ptr == HPM_ADC1) {
        if (clk_src_ahb) {
            /* Configure the ADC clock from AHB (@200MHz by default)*/
            clock_set_adc_source(clock_adc1, clk_adc_src_ahb0);
        } else {
            /* Configure the ADC clock from pll0_clk0 divided by 2 (@200MHz by default) */
            clock_set_adc_source(clock_adc1, clk_adc_src_ana1);
            clock_set_source_divider(clock_ana1, clk_src_pll0_clk2, 2U);
        }

        freq = clock_get_frequency(clock_adc1);
    }

    return freq;
}

void board_DCDC_power_config(void)
{ 
    #if USING_EXTERNEL_DCDC
    HPM_PCFG->DCDC_MODE &= 0xfff8ffff;
    #endif
}

void board_init_adc16_pins(void)
{
    init_adc_pins();
}

void board_init_pmp(void)
{
}

uint32_t board_init_uart_clock(UART_Type *ptr)
{
    uint32_t freq = 0U;
    if (ptr == HPM_UART0) {
        clock_set_source_divider(clock_uart0, clk_src_osc24m, 1);
        clock_add_to_group(clock_uart0, 0);
        freq = clock_get_frequency(clock_uart0);
    } else if (ptr == HPM_UART1) {
        clock_set_source_divider(clock_uart1, clk_src_osc24m, 1);
        clock_add_to_group(clock_uart1, 0);
        freq = clock_get_frequency(clock_uart1);
    } else if (ptr == HPM_UART2) {
        clock_set_source_divider(clock_uart2, clk_src_pll0_clk2, 8);
        clock_add_to_group(clock_uart2, 0);
        freq = clock_get_frequency(clock_uart2);
    }

    return freq;
}
