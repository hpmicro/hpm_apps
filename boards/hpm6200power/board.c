/*
 * Copyright (c) 2023 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *
 */

#include "board.h"
#include "hpm_uart_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_i2c_drv.h"
#include "hpm_gpio_drv.h"
#include "pinmux.h"
#include "hpm_pmp_drv.h"
#include "assert.h"
#include "hpm_clock_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_pllctlv2_drv.h"
#include "hpm_pcfg_drv.h"

static board_timer_cb timer_cb;
ATTR_PLACE_AT_NONCACHEABLE_BSS static bool init_delay_flag;

/**
 * @brief FLASH configuration option definitions:
 * option[0]:
 *    [31:16] 0xfcf9 - FLASH configuration option tag
 *    [15:4]  0 - Reserved
 *    [3:0]   option words (exclude option[0])
 * option[1]:
 *    [31:28] Flash probe type
 *      0 - SFDP SDR / 1 - SFDP DDR
 *      2 - 1-4-4 Read (0xEB, 24-bit address) / 3 - 1-2-2 Read(0xBB, 24-bit address)
 *      4 - HyperFLASH 1.8V / 5 - HyperFLASH 3V
 *      6 - OctaBus DDR (SPI -> OPI DDR)
 *      8 - Xccela DDR (SPI -> OPI DDR)
 *      10 - EcoXiP DDR (SPI -> OPI DDR)
 *    [27:24] Command Pads after Power-on Reset
 *      0 - SPI / 1 - DPI / 2 - QPI / 3 - OPI
 *    [23:20] Command Pads after Configuring FLASH
 *      0 - SPI / 1 - DPI / 2 - QPI / 3 - OPI
 *    [19:16] Quad Enable Sequence (for the device support SFDP 1.0 only)
 *      0 - Not needed
 *      1 - QE bit is at bit 6 in Status Register 1
 *      2 - QE bit is at bit1 in Status Register 2
 *      3 - QE bit is at bit7 in Status Register 2
 *      4 - QE bit is at bit1 in Status Register 2 and should be programmed by 0x31
 *    [15:8] Dummy cycles
 *      0 - Auto-probed / detected / default value
 *      Others - User specified value, for DDR read, the dummy cycles should be 2 * cycles on FLASH datasheet
 *    [7:4] Misc.
 *      0 - Not used
 *      1 - SPI mode
 *      2 - Internal loopback
 *      3 - External DQS
 *    [3:0] Frequency option
 *      1 - 30MHz / 2 - 50MHz / 3 - 66MHz / 4 - 80MHz / 5 - 100MHz / 6 - 120MHz / 7 - 133MHz / 8 - 166MHz
 *
 * option[2] (Effective only if the bit[3:0] in option[0] > 1)
 *    [31:20]  Reserved
 *    [19:16] IO voltage
 *      0 - 3V / 1 - 1.8V
 *    [15:12] Pin group
 *      0 - 1st group / 1 - 2nd group
 *    [11:8] Connection selection
 *      0 - CA_CS0 / 1 - CB_CS0 / 2 - CA_CS0 + CB_CS0 (Two FLASH connected to CA and CB respectively)
 *    [7:0] Drive Strength
 *      0 - Default value
 * option[3] (Effective only if the bit[3:0] in option[0] > 2, required only for the QSPI NOR FLASH that not supports
 *              JESD216)
 *    [31:16] reserved
 *    [15:12] Sector Erase Command Option, not required here
 *    [11:8]  Sector Size Option, not required here
 *    [7:0] Flash Size Option
 *      0 - 4MB / 1 - 8MB / 2 - 16MB
 */
#if defined(FLASH_XIP) && FLASH_XIP
__attribute__((section(".nor_cfg_option"))) const uint32_t option[4] = { 0xfcf90001, 0x00000007, 0x0, 0x0 };
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
    uart rx pin when configuring pin function will cause a wrong data to be received.
    And a uart rx dma request will be generated by default uart fifo dma trigger level. */
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

void board_print_clock_freq(void)
{
    printf("==============================\n");
    printf(" %s clock summary\n", BOARD_NAME);
    printf("==============================\n");
    printf("cpu0:\t\t %dHz\n", clock_get_frequency(clock_cpu0));
    printf("cpu1:\t\t %luHz\n", clock_get_frequency(clock_cpu1));
    printf("axi:\t\t %dHz\n", clock_get_frequency(clock_axi));
    printf("ahb:\t\t %dHz\n", clock_get_frequency(clock_ahb));
    printf("mchtmr0:\t %dHz\n", clock_get_frequency(clock_mchtmr0));
    printf("mchtmr1:\t %luHz\n", clock_get_frequency(clock_mchtmr1));
    printf("xpi0:\t\t %dHz\n", clock_get_frequency(clock_xpi0));
    printf("==============================\n");
}

void board_init_uart(UART_Type *ptr)
{
    /* configure uart's pin before opening uart's clock */
    init_uart_pins(ptr);
    board_init_uart_clock(ptr);
}

void board_print_banner(void)
{
    const uint8_t banner[] = { "\n\
----------------------------------------------------------------------\n\
$$\\   $$\\ $$$$$$$\\  $$\\      $$\\ $$\\\n\
$$ |  $$ |$$  __$$\\ $$$\\    $$$ |\\__|\n\
$$ |  $$ |$$ |  $$ |$$$$\\  $$$$ |$$\\  $$$$$$$\\  $$$$$$\\   $$$$$$\\\n\
$$$$$$$$ |$$$$$$$  |$$\\$$\\$$ $$ |$$ |$$  _____|$$  __$$\\ $$  __$$\\\n\
$$  __$$ |$$  ____/ $$ \\$$$  $$ |$$ |$$ /      $$ |  \\__|$$ /  $$ |\n\
$$ |  $$ |$$ |      $$ |\\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |\n\
$$ |  $$ |$$ |      $$ | \\_/ $$ |$$ |\\$$$$$$$\\ $$ |      \\$$$$$$  |\n\
\\__|  \\__|\\__|      \\__|     \\__|\\__| \\_______|\\__|       \\______/\n\
----------------------------------------------------------------------\n"};
#ifdef SDK_VERSION_STRING
    printf("hpm_sdk: %s\n", SDK_VERSION_STRING);
#endif
    printf("%s", banner);
}

uint8_t board_get_led_pwm_off_level(void)
{
    return BOARD_LED_OFF_LEVEL;
}

uint8_t board_get_led_gpio_off_level(void)
{
    return BOARD_LED_OFF_LEVEL;
}

void board_ungate_mchtmr_at_lp_mode(void)
{
    /* Keep cpu clock on wfi, so that mchtmr irq can still work after wfi */
    sysctl_set_cpu_lp_mode(HPM_SYSCTL, BOARD_RUNNING_CORE, cpu_lp_mode_ungate_cpu_clock);
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

void board_delay_us(uint32_t us)
{
    static uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    if (init_delay_flag == false) {
        init_delay_flag = true;
        clock_add_to_group(BOARD_DELAY_TIMER_CLK_NAME, 0);
        gptmr_freq = clock_get_frequency(BOARD_DELAY_TIMER_CLK_NAME);
        gptmr_channel_get_default_config(BOARD_DELAY_TIMER, &config);
        gptmr_channel_config(BOARD_DELAY_TIMER, BOARD_DELAY_TIMER_CH, &config, false);
    }

    gptmr_channel_config_update_reload(BOARD_DELAY_TIMER, BOARD_DELAY_TIMER_CH, gptmr_freq / 1000000 * us);
    gptmr_start_counter(BOARD_DELAY_TIMER, BOARD_DELAY_TIMER_CH);
    while (!gptmr_check_status(BOARD_DELAY_TIMER, GPTMR_CH_RLD_STAT_MASK(BOARD_DELAY_TIMER_CH))) {
        __asm("nop");
    }
    gptmr_stop_counter(BOARD_DELAY_TIMER, BOARD_DELAY_TIMER_CH);
    gptmr_clear_status(BOARD_DELAY_TIMER, GPTMR_CH_RLD_STAT_MASK(BOARD_DELAY_TIMER_CH));
}

void board_delay_ms(uint32_t ms)
{
    board_delay_us(1000 * ms);
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

void board_i2c_bus_clear(I2C_Type *ptr)
{
    init_i2c_pins_as_gpio(ptr);
}

void board_init_i2c(I2C_Type *ptr)
{
    (void)ptr;
}

uint32_t board_init_spi_clock(SPI_Type *ptr)
{
    if (ptr == HPM_SPI1) {
        /* SPI1 clock configure */
        clock_add_to_group(clock_spi1, 0);
        clock_set_source_divider(clock_spi1, clk_src_pll0_clk0, 5U); /* 80MHz */

        return clock_get_frequency(clock_spi1);
    } else if (ptr == HPM_SPI3) {
        /* SPI3 clock configure */
        clock_add_to_group(clock_spi3, 0);
        clock_set_source_divider(clock_spi3, clk_src_pll0_clk0, 5U); /* 80MHz */

        return clock_get_frequency(clock_spi3);
    }
    return 0;
}

uint32_t board_init_lin_clock(LIN_Type *ptr)
{
    if (ptr == HPM_LIN0) {
        clock_add_to_group(clock_lin0, 0);
        clock_set_source_divider(clock_lin0, clk_src_pll0_clk0, 20U); /* 20MHz */

        return clock_get_frequency(clock_lin0);
    }
    return 0;
}

void board_init_gpio_pins(void)
{
    // init_gpio_pins();
}

void board_leds_gpio_pins(void)
{
    init_leds_gpio_pins();
}

void board_normal_gpio_pins(uint8_t index)
{
    init_normal_gpio_pins(index);
}

void board_keys_gpio_pins(void)
{
    init_butn_pins();
}

void board_init_spi_pins(SPI_Type *ptr)
{
    init_spi_pins(ptr);
}

void board_init_spi_pins_with_gpio_as_cs(SPI_Type *ptr)
{
    init_spi_pins_with_gpio_as_cs(ptr);
    gpio_set_pin_output_with_initial(BOARD_SPI_CS_GPIO_CTRL, GPIO_GET_PORT_INDEX(BOARD_SPI_CS_PIN),
                                     GPIO_GET_PIN_INDEX(BOARD_SPI_CS_PIN), !BOARD_SPI_CS_ACTIVE_LEVEL);
}

void board_write_spi_cs(uint32_t pin, uint8_t state)
{
    gpio_write_pin(BOARD_SPI_CS_GPIO_CTRL, GPIO_GET_PORT_INDEX(pin), GPIO_GET_PIN_INDEX(pin), state);
}

void board_init_led_pins(void)
{
    init_led_pins_as_gpio();
}

void board_led_toggle(void)
{
#ifdef BOARD_LED_TOGGLE_RGB
    static uint8_t i;
    switch (i) {
    case 1:
        gpio_write_pin(BOARD_R_GPIO_CTRL, BOARD_R_GPIO_INDEX, BOARD_R_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        gpio_write_pin(BOARD_G_GPIO_CTRL, BOARD_G_GPIO_INDEX, BOARD_G_GPIO_PIN, BOARD_LED_ON_LEVEL);
        gpio_write_pin(BOARD_B_GPIO_CTRL, BOARD_B_GPIO_INDEX, BOARD_B_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        break;

    case 2:
        gpio_write_pin(BOARD_R_GPIO_CTRL, BOARD_R_GPIO_INDEX, BOARD_R_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        gpio_write_pin(BOARD_G_GPIO_CTRL, BOARD_G_GPIO_INDEX, BOARD_G_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        gpio_write_pin(BOARD_B_GPIO_CTRL, BOARD_B_GPIO_INDEX, BOARD_B_GPIO_PIN, BOARD_LED_ON_LEVEL);
        break;

    case 0:
    default:
        gpio_write_pin(BOARD_R_GPIO_CTRL, BOARD_R_GPIO_INDEX, BOARD_R_GPIO_PIN, BOARD_LED_ON_LEVEL);
        gpio_write_pin(BOARD_G_GPIO_CTRL, BOARD_G_GPIO_INDEX, BOARD_G_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        gpio_write_pin(BOARD_B_GPIO_CTRL, BOARD_B_GPIO_INDEX, BOARD_B_GPIO_PIN, BOARD_LED_OFF_LEVEL);
        break;
    }
    i++;
    i = i % 3;
#else
    gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
#endif
}

void board_led_write(uint8_t state)
{
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, state);
}

void board_init_usb_pins(void)
{
    /* set pull-up for USBx ID pin */
    init_usb_pins();

    /* configure USBx ID pin as input function */
    gpio_set_pin_input(BOARD_USB0_ID_PORT, BOARD_USB0_ID_GPIO_INDEX, BOARD_USB0_ID_GPIO_PIN);
}

uint8_t board_get_usb_id_status(void)
{
    return gpio_read_pin(BOARD_USB0_ID_PORT, BOARD_USB0_ID_GPIO_INDEX, BOARD_USB0_ID_GPIO_PIN);
}

void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level)
{
    (void)usb_index;
    (void)level;
}

void board_init_pmp(void)
{
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t length;
    pmp_entry_t pmp_entry[16];
    uint8_t index = 0;

    /* Init noncachable memory */
    extern uint32_t __noncacheable_start__[];
    extern uint32_t __noncacheable_end__[];
    start_addr = (uint32_t)__noncacheable_start__;
    end_addr = (uint32_t)__noncacheable_end__;
    length = end_addr - start_addr;
    if (length > 0) {
        /* Ensure the address and the length are power of 2 aligned */
        assert((length & (length - 1U)) == 0U);
        assert((start_addr & (length - 1U)) == 0U);
        pmp_entry[index].pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
        pmp_entry[index].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
        pmp_entry[index].pma_addr = PMA_NAPOT_ADDR(start_addr, length);
        pmp_entry[index].pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);
        index++;
    }

    /* Init share memory */
    extern uint32_t __share_mem_start__[];
    extern uint32_t __share_mem_end__[];
    start_addr = (uint32_t)__share_mem_start__;
    end_addr = (uint32_t)__share_mem_end__;
    length = end_addr - start_addr;
    if (length > 0) {
        /* Ensure the address and the length are power of 2 aligned */
        assert((length & (length - 1U)) == 0U);
        assert((start_addr & (length - 1U)) == 0U);
        pmp_entry[index].pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
        pmp_entry[index].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
        pmp_entry[index].pma_addr = PMA_NAPOT_ADDR(start_addr, length);
        pmp_entry[index].pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);
        index++;
    }

    pmp_config(&pmp_entry[0], index);
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
    /* Add most Clocks to group 0 */
    /* not open uart clock in this API, uart should configure pin function before opening clock */
    clock_add_to_group(clock_cpu0, 0);
    clock_add_to_group(clock_ahbp, 0);
    clock_add_to_group(clock_axic, 0);
    clock_add_to_group(clock_axis, 0);

    clock_add_to_group(clock_mchtmr0, 0);
    clock_add_to_group(clock_xpi0, 0);
    clock_add_to_group(clock_gptmr0, 0);
    clock_add_to_group(clock_gptmr1, 0);
    clock_add_to_group(clock_gptmr2, 0);
    clock_add_to_group(clock_gptmr3, 0);
    clock_add_to_group(clock_i2c0, 0);
    clock_add_to_group(clock_i2c1, 0);
    clock_add_to_group(clock_i2c2, 0);
    clock_add_to_group(clock_i2c3, 0);
    clock_add_to_group(clock_lin0, 0);
    clock_add_to_group(clock_lin1, 0);
    clock_add_to_group(clock_lin2, 0);
    clock_add_to_group(clock_lin3, 0);
    clock_add_to_group(clock_spi0, 0);
    clock_add_to_group(clock_spi1, 0);
    clock_add_to_group(clock_spi2, 0);
    clock_add_to_group(clock_spi3, 0);
    clock_add_to_group(clock_can0, 0);
    clock_add_to_group(clock_can1, 0);
    clock_add_to_group(clock_can2, 0);
    clock_add_to_group(clock_can3, 0);
    clock_add_to_group(clock_ptpc, 0);
    clock_add_to_group(clock_ref0, 0);
    clock_add_to_group(clock_ref1, 0);
    clock_add_to_group(clock_watchdog0, 0);
    clock_add_to_group(clock_sdp, 0);
    clock_add_to_group(clock_xdma, 0);
    clock_add_to_group(clock_ram0, 0);
    clock_add_to_group(clock_usb0, 0);
    clock_add_to_group(clock_kman, 0);
    clock_add_to_group(clock_gpio, 0);
    clock_add_to_group(clock_mbx0, 0);
    clock_add_to_group(clock_hdma, 0);
    clock_add_to_group(clock_rng, 0);
    clock_add_to_group(clock_mot0, 0);
    clock_add_to_group(clock_mot1, 0);
    clock_add_to_group(clock_mot2, 0);
    clock_add_to_group(clock_mot3, 0);
    clock_add_to_group(clock_acmp, 0);
    clock_add_to_group(clock_synt, 0);
    clock_add_to_group(clock_lmm0, 0);
    clock_add_to_group(clock_lmm1, 0);

    clock_add_to_group(clock_adc0, 0);
    clock_add_to_group(clock_adc1, 0);
    clock_add_to_group(clock_adc2, 0);

    clock_add_to_group(clock_dac0, 0);
    clock_add_to_group(clock_dac1, 0);

    clock_add_to_group(clock_tsns, 0);
    clock_add_to_group(clock_crc0, 0);
    clock_add_to_group(clock_sdm0, 0);

    /* Connect Group0 to CPU0 */
    clock_connect_group_to_cpu(0, 0);

    /* Add the CPU1 clock to Group1 */
    clock_add_to_group(clock_mchtmr1, 1);

    /* Connect Group1 to CPU1 */
    clock_connect_group_to_cpu(1, 1);

    /* Bump up DCDC voltage to 1275mv */
    pcfg_dcdc_set_voltage(HPM_PCFG, 1275);

    /* Configure CPU to 600MHz, AXI/AHB to 200MHz */
    sysctl_config_cpu0_domain_clock(HPM_SYSCTL, clock_source_pll1_clk0, 1, 3, 3);
    /* Connect CAN2/CAN3 to pll0clk0*/
    clock_set_source_divider(clock_can2, clk_src_pll0_clk0, 1);
    clock_set_source_divider(clock_can3, clk_src_pll0_clk0, 1);

    /* Configure PLL1_CLK0 Post Divider to 1 */
    pllctlv2_set_postdiv(HPM_PLLCTLV2, 1, 0, 0);
    pllctlv2_init_pll_with_freq(HPM_PLLCTLV2, 1, 600000000);
    clock_update_core_clock();

    /* Configure AHB to 200MHz */
    clock_set_source_divider(clock_ahb, clk_src_pll1_clk1, 2);

    /* Configure mchtmr to 24MHz */
    clock_set_source_divider(clock_mchtmr0, clk_src_osc24m, 1);
    clock_set_source_divider(clock_mchtmr1, clk_src_osc24m, 1);
}

uint32_t board_init_adc12_clock(ADC16_Type *ptr)
{
    uint32_t freq = 0;
    switch ((uint32_t)ptr) {
    case HPM_ADC0_BASE:
        /* Configure the ADC clock to 200MHz */
        clock_set_adc_source(clock_adc0, clk_adc_src_ana0);
        clock_set_source_divider(clock_ana0, clk_src_pll1_clk1, 2U);
        freq = clock_get_frequency(clock_adc0);
        break;
    case HPM_ADC1_BASE:
        /* Configure the ADC clock to 200MHz */
        clock_set_adc_source(clock_adc1, clk_adc_src_ana0);
        clock_set_source_divider(clock_ana0, clk_src_pll1_clk1, 2U);
        freq = clock_get_frequency(clock_adc1);
        break;
    case HPM_ADC2_BASE:
        /* Configure the ADC clock to 200MHz */
        clock_set_adc_source(clock_adc2, clk_adc_src_ana0);
        clock_set_source_divider(clock_ana0, clk_src_pll1_clk1, 2U);
        freq = clock_get_frequency(clock_adc2);
        break;
    default:
        /* Invalid ADC instance */
        break;
    }

    return freq;
}

uint32_t board_init_adc16_clock(ADC16_Type *ptr)
{
    (void)ptr;
    return 0;
}

uint32_t board_init_dac_clock(DAC_Type *ptr, bool clk_src_ahb)
{
    uint32_t freq = 0;

    if (ptr == HPM_DAC0) {
        if (clk_src_ahb == true) {
            /* Configure the DAC clock to 133MHz */
            clock_set_dac_source(clock_dac0, clk_dac_src_ahb0);
        } else {
            /* Configure the DAC clock to 166MHz */
            clock_set_dac_source(clock_dac0, clk_dac_src_ana3);
            clock_set_source_divider(clock_ana3, clk_src_pll0_clk1, 2);
        }

        freq = clock_get_frequency(clock_dac0);
    }

    return freq;
}

void board_init_can(MCAN_Type *ptr)
{
    init_can_pins(ptr);
}

uint32_t board_init_can_clock(MCAN_Type *ptr)
{
    uint32_t freq = 0;
    if (ptr == HPM_MCAN0) {
        /* Set the CAN0 peripheral clock to 8MHz */
        clock_set_source_divider(clock_can0, clk_src_pll0_clk0, 5);
        freq = clock_get_frequency(clock_can0);
    } else if (ptr == HPM_MCAN1) {
        /* Set the CAN1 peripheral clock to 8MHz */
        clock_set_source_divider(clock_can1, clk_src_pll0_clk0, 5);
        freq = clock_get_frequency(clock_can1);
    } else if (ptr == HPM_MCAN2) {
        /* Set the CAN2 peripheral clock to 8MHz */
        clock_set_source_divider(clock_can2, clk_src_pll0_clk0, 5);
        freq = clock_get_frequency(clock_can2);
    } else if (ptr == HPM_MCAN3) {
        /* Set the CAN2 peripheral clock to 8MHz */
        clock_set_source_divider(clock_can3, clk_src_pll0_clk0, 5);
        freq = clock_get_frequency(clock_can3);
    } else {
        /* Invalid CAN instance */
    }
    return freq;
}

void board_init_adc16_pins(void)
{
    init_adc_pins();
}

void board_init_rgb_pwm_pins(void)
{
    init_led_pins_as_pwm();
}

void board_init_dac_pins(DAC_Type *ptr)
{
    init_dac_pins(ptr);
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
        clock_set_source_divider(clock_uart2, clk_src_osc24m, 1);
        clock_add_to_group(clock_uart2, 0);
        freq = clock_get_frequency(clock_uart2);
    } else {
        /* Not supported */
    }
    return freq;
}
