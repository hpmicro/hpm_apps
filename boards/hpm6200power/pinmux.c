/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Note:
 *   PY and PZ IOs: if any SOC pin function needs to be routed to these IOs,
 *  besides of IOC, PIOC/BIOC needs to be configured SOC_GPIO_X_xx, so that
 *  expected SoC function can be enabled on these IOs.
 *
 */
#include "board.h"

void init_uart_pins(UART_Type *ptr)
{
    if (ptr == HPM_UART0) {
        HPM_IOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY07_FUNC_CTL_UART0_RXD;
        HPM_IOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_UART0_TXD;
        /* PY port IO needs to configure PIOC */
        HPM_PIOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY07_FUNC_CTL_SOC_GPIO_Y_07;
        HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_SOC_GPIO_Y_06;
    } else if (ptr == HPM_UART1) {
        HPM_IOC->PAD[IOC_PAD_PC24].FUNC_CTL = IOC_PC24_FUNC_CTL_UART1_TXD;
        HPM_IOC->PAD[IOC_PAD_PC25].FUNC_CTL = IOC_PC25_FUNC_CTL_UART1_RXD;
    } else if (ptr == HPM_UART2) {
        HPM_IOC->PAD[IOC_PAD_PC26].FUNC_CTL = IOC_PC26_FUNC_CTL_UART2_TXD;
        HPM_IOC->PAD[IOC_PAD_PC27].FUNC_CTL = IOC_PC27_FUNC_CTL_UART2_RXD;
    } else if (ptr == HPM_PUART) {
        HPM_PIOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY07_FUNC_CTL_UART_RXD;
        HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_UART_TXD;
    }
}

void init_i2c_pins_as_gpio(I2C_Type *ptr)
{
    if (ptr == HPM_I2C0) {
        /* I2C0 */
        HPM_IOC->PAD[IOC_PAD_PB22].FUNC_CTL = IOC_PB22_FUNC_CTL_GPIO_B_22;
        HPM_IOC->PAD[IOC_PAD_PB23].FUNC_CTL = IOC_PB23_FUNC_CTL_GPIO_B_23;
    } else {
        while (1) {
        }
    }
}

void init_i2c_pins(I2C_Type *ptr)
{
    if (ptr == HPM_I2C0) {
        HPM_IOC->PAD[IOC_PAD_PB22].FUNC_CTL = IOC_PB22_FUNC_CTL_I2C0_SCL
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PB23].FUNC_CTL = IOC_PB23_FUNC_CTL_I2C0_SDA
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PB22].PAD_CTL = IOC_PAD_PAD_CTL_OD_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        HPM_IOC->PAD[IOC_PAD_PB23].PAD_CTL = IOC_PAD_PAD_CTL_OD_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
    } else if (ptr == HPM_I2C3) {
        HPM_IOC->PAD[IOC_PAD_PC11].FUNC_CTL = IOC_PC11_FUNC_CTL_I2C3_SCL
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PC12].FUNC_CTL = IOC_PC12_FUNC_CTL_I2C3_SDA
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PC11].PAD_CTL = IOC_PAD_PAD_CTL_OD_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        HPM_IOC->PAD[IOC_PAD_PC12].PAD_CTL = IOC_PAD_PAD_CTL_OD_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
    } else {
        while (1) {
        }
    }
}

void init_sdm_pins(void)
{
    /* channel 3 */
    HPM_IOC->PAD[IOC_PAD_PA06].FUNC_CTL = IOC_PA06_FUNC_CTL_SDM0_CLK_3;
    HPM_IOC->PAD[IOC_PAD_PA07].FUNC_CTL = IOC_PA07_FUNC_CTL_SDM0_DAT_3;
}

void init_leds_gpio_pins(void)
{
#ifdef USING_GPIO0_FOR_GPIOZ
    HPM_IOC->PAD[IOC_PAD_PZ04].FUNC_CTL = IOC_PZ04_FUNC_CTL_GPIO_Z_04;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ04].FUNC_CTL = IOC_PZ04_FUNC_CTL_SOC_GPIO_Z_04;

    HPM_IOC->PAD[IOC_PAD_PZ06].FUNC_CTL = IOC_PZ06_FUNC_CTL_GPIO_Z_06;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ06].FUNC_CTL = IOC_PZ06_FUNC_CTL_SOC_GPIO_Z_06;

    HPM_IOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_GPIO_Z_02;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_SOC_GPIO_Z_02;
#endif
}

void init_normal_gpio_pins(uint8_t index)
{
    switch(index)
    {
        case 0:
        {
             HPM_IOC->PAD[IOC_PAD_PA17].FUNC_CTL = IOC_PA17_FUNC_CTL_GPIO_A_17;
             HPM_IOC->PAD[IOC_PAD_PA18].FUNC_CTL = IOC_PA18_FUNC_CTL_GPIO_A_18;
             HPM_IOC->PAD[IOC_PAD_PA19].FUNC_CTL = IOC_PA19_FUNC_CTL_GPIO_A_19;
        }
        break;
        case 1:
        {
            HPM_IOC->PAD[IOC_PAD_PA11].FUNC_CTL = IOC_PA11_FUNC_CTL_GPIO_A_11;
            HPM_IOC->PAD[IOC_PAD_PA13].FUNC_CTL = IOC_PA13_FUNC_CTL_GPIO_A_13;
            HPM_IOC->PAD[IOC_PAD_PA12].FUNC_CTL = IOC_PA12_FUNC_CTL_GPIO_A_12;
        }
        break;
        case 2:
        {
            HPM_IOC->PAD[IOC_PAD_PA09].FUNC_CTL = IOC_PA09_FUNC_CTL_GPIO_A_09;
            HPM_IOC->PAD[IOC_PAD_PA08].FUNC_CTL = IOC_PA08_FUNC_CTL_GPIO_A_08;
            HPM_IOC->PAD[IOC_PAD_PA10].FUNC_CTL = IOC_PA10_FUNC_CTL_GPIO_A_10;
        }
        break;
        case 3:
        {
            HPM_IOC->PAD[IOC_PAD_PA15].FUNC_CTL = IOC_PA15_FUNC_CTL_GPIO_A_15;
            HPM_IOC->PAD[IOC_PAD_PA14].FUNC_CTL = IOC_PA14_FUNC_CTL_GPIO_A_14;
            HPM_IOC->PAD[IOC_PAD_PA16].FUNC_CTL = IOC_PA16_FUNC_CTL_GPIO_A_16;
        }
        break;
        default:
        break;
    }
}

void init_spi_pins(SPI_Type *ptr)
{
    if (ptr == HPM_SPI1) {
        HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_SPI1_CSN;
        HPM_IOC->PAD[IOC_PAD_PB05].FUNC_CTL = IOC_PB05_FUNC_CTL_SPI1_MOSI;
        HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PB03_FUNC_CTL_SPI1_MISO;
        HPM_IOC->PAD[IOC_PAD_PB04].FUNC_CTL = IOC_PB04_FUNC_CTL_SPI1_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
    }
}

void init_spi_pins_with_gpio_as_cs(SPI_Type *ptr)
{
    if (ptr == HPM_SPI1) {
        HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_GPIO_B_02;
        HPM_IOC->PAD[IOC_PAD_PB05].FUNC_CTL = IOC_PB05_FUNC_CTL_SPI1_MOSI;
        HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PB03_FUNC_CTL_SPI1_MISO;
        HPM_IOC->PAD[IOC_PAD_PB04].FUNC_CTL = IOC_PB04_FUNC_CTL_SPI1_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
    }
}

void init_pins(void)
{
    init_uart_pins(BOARD_CONSOLE_BASE);
}

void init_gptmr_pins(GPTMR_Type *ptr)
{
    if (ptr == HPM_GPTMR2) {
        HPM_IOC->PAD[IOC_PAD_PC06].FUNC_CTL = IOC_PC06_FUNC_CTL_GPTMR2_CAPT_0;
        HPM_IOC->PAD[IOC_PAD_PC08].FUNC_CTL = IOC_PC08_FUNC_CTL_GPTMR2_COMP_0;
    }
}

void init_hall_trgm_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PB26].FUNC_CTL = IOC_PB26_FUNC_CTL_TRGM0_P_06;
    HPM_IOC->PAD[IOC_PAD_PB27].FUNC_CTL = IOC_PB27_FUNC_CTL_TRGM0_P_07;
    HPM_IOC->PAD[IOC_PAD_PB28].FUNC_CTL = IOC_PB28_FUNC_CTL_TRGM0_P_08;
}

void init_qei_trgm_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PB26].FUNC_CTL = IOC_PB26_FUNC_CTL_TRGM0_P_06;
    HPM_IOC->PAD[IOC_PAD_PB27].FUNC_CTL = IOC_PB27_FUNC_CTL_TRGM0_P_07;
}

void init_butn_pins(void)
{
    /* configure pad setting: pull enable and pull up, schmitt trigger enable */
    /* enable schmitt trigger to eliminate jitter of pin used as button */
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);

    /* Button */
#ifdef USING_GPIO0_FOR_GPIOZ
    HPM_IOC->PAD[IOC_PAD_PZ00].FUNC_CTL = IOC_PZ00_FUNC_CTL_GPIO_Z_00;
    HPM_IOC->PAD[IOC_PAD_PZ00].PAD_CTL = pad_ctl;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ00].FUNC_CTL = IOC_PZ00_FUNC_CTL_SOC_GPIO_Z_00;

    HPM_IOC->PAD[IOC_PAD_PZ05].FUNC_CTL = IOC_PZ05_FUNC_CTL_GPIO_Z_05;
    HPM_IOC->PAD[IOC_PAD_PZ05].PAD_CTL = pad_ctl;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ05].FUNC_CTL = IOC_PZ05_FUNC_CTL_SOC_GPIO_Z_05;

    HPM_IOC->PAD[IOC_PAD_PZ07].FUNC_CTL = IOC_PZ07_FUNC_CTL_GPIO_Z_07;
    HPM_IOC->PAD[IOC_PAD_PZ07].PAD_CTL = pad_ctl;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ07].FUNC_CTL = IOC_PZ07_FUNC_CTL_SOC_GPIO_Z_07;

    HPM_IOC->PAD[IOC_PAD_PZ03].FUNC_CTL = IOC_PZ03_FUNC_CTL_GPIO_Z_03;
    HPM_IOC->PAD[IOC_PAD_PZ03].PAD_CTL = pad_ctl;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ03].FUNC_CTL = IOC_PZ03_FUNC_CTL_SOC_GPIO_Z_03;
#endif
}

void init_acmp_pins(void)
{
    /* configure to CMP1_INN5 function */
    HPM_IOC->PAD[IOC_PAD_PC16].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* configure to ACMP_COMP_1 function */
    HPM_IOC->PAD[IOC_PAD_PC15].FUNC_CTL = IOC_PC15_FUNC_CTL_ACMP_COMP_1;
}

void init_pwm_pins(PWM_Type *ptr)
{
    if (ptr == HPM_PWM0) {
        HPM_IOC->PAD[IOC_PAD_PB17].FUNC_CTL = IOC_PB17_FUNC_CTL_PWM0_P_5;
        HPM_IOC->PAD[IOC_PAD_PB15].FUNC_CTL = IOC_PB15_FUNC_CTL_PWM0_P_3;
        HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PB13_FUNC_CTL_PWM0_P_1;
        HPM_IOC->PAD[IOC_PAD_PB16].FUNC_CTL = IOC_PB16_FUNC_CTL_PWM0_P_4;
        HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PB14_FUNC_CTL_PWM0_P_2;
        HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PB12_FUNC_CTL_PWM0_P_0;
    }
}

void init_hrpwm_pins(PWM_Type *ptr)
{
    if (ptr == HPM_PWM1) {
        HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PB00_FUNC_CTL_PWM1_P_0;
        HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_PWM1_P_2;
    }
}

void init_adc_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PC05].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* ADC_BUS:ADC0.INA1 */
    HPM_IOC->PAD[IOC_PAD_PC16].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* ADC_IW: ADC0.INA12/ADC1.INA8/ADC2.INA4 */
    HPM_IOC->PAD[IOC_PAD_PC17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* ADC_IV: ADC0.INA13/ADC1.INA9/ADC2.INA5 */
    HPM_IOC->PAD[IOC_PAD_PC15].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* ADC_IU: ADC0.INA11/ADC1.INA7.ADC2.INA3 */
}

void init_adc_bldc_pins(void)
{
   HPM_IOC->PAD[IOC_PAD_PC15].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
   HPM_IOC->PAD[IOC_PAD_PC16].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
   HPM_IOC->PAD[IOC_PAD_PC17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
}

void init_usb_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PC23].FUNC_CTL = IOC_PC23_FUNC_CTL_GPIO_C_23;
    HPM_IOC->PAD[IOC_PAD_PC23].PAD_CTL = IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1);
}

void  init_can_pins(MCAN_Type *ptr)
{
    if (ptr == HPM_MCAN3) {
        HPM_IOC->PAD[IOC_PAD_PC00].FUNC_CTL = IOC_PC00_FUNC_CTL_MCAN3_STBY | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PC02].FUNC_CTL = IOC_PC02_FUNC_CTL_MCAN3_TXD | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PC01].FUNC_CTL = IOC_PC01_FUNC_CTL_MCAN3_RXD | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;

        HPM_IOC->PAD[IOC_PAD_PC00].PAD_CTL = 0x10;
        HPM_IOC->PAD[IOC_PAD_PC02].PAD_CTL = 0x810;
        HPM_IOC->PAD[IOC_PAD_PC01].PAD_CTL = 0x810;
    }
}

void init_clk_obs_pins(void)
{
    /* HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_SYSCTL_CLK_OBS_0; */
}

void init_led_pins_as_gpio(void)
{
    /* configure pad setting: pull enable and pull up, schmitt trigger enable */
    /* enable schmitt trigger to eliminate jitter of pin used as button */
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);

    /* Button */
#ifdef USING_GPIO0_FOR_GPIOZ
    HPM_IOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_GPIO_Z_02;
    HPM_IOC->PAD[IOC_PAD_PZ02].PAD_CTL = pad_ctl;
    /* PZ port IO needs to configure BIOC as well */
    HPM_BIOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_SOC_GPIO_Z_02;
#endif
}

void init_dac_pins(DAC_Type *ptr)
{
    if (ptr == HPM_DAC0) {
        HPM_IOC->PAD[IOC_PAD_PC03].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* DAC0.OUT */
    } else if (ptr == HPM_DAC1) {
        HPM_IOC->PAD[IOC_PAD_PC04].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* DAC1.OUT */
    }
}

void init_trgmux_pins(uint32_t pin)
{
    /* all trgmux pin ALT_SELECT fixed to 16*/
    HPM_IOC->PAD[pin].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(17);
}

void init_pla_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PB25].FUNC_CTL = IOC_PB25_FUNC_CTL_TRGM0_P_05;
}

void init_lin_pins(LIN_Type *ptr)
{
    /** enable open drain and pull up */
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_OD_SET(1);
    if (ptr == HPM_LIN0) {
        HPM_IOC->PAD[IOC_PAD_PA10].FUNC_CTL = IOC_PA10_FUNC_CTL_LIN0_TXD;
        HPM_IOC->PAD[IOC_PAD_PA10].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PA11].FUNC_CTL = IOC_PA11_FUNC_CTL_LIN0_RXD;
        HPM_IOC->PAD[IOC_PAD_PA11].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PA15].FUNC_CTL = IOC_PA15_FUNC_CTL_LIN0_TREN;
        HPM_IOC->PAD[IOC_PAD_PA15].PAD_CTL = pad_ctl;
    } else if (ptr == HPM_LIN2) {
        HPM_IOC->PAD[IOC_PAD_PA07].FUNC_CTL = IOC_PA07_FUNC_CTL_LIN2_RXD;
        HPM_IOC->PAD[IOC_PAD_PA07].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PA06].FUNC_CTL = IOC_PA06_FUNC_CTL_LIN2_TXD;
        HPM_IOC->PAD[IOC_PAD_PA06].PAD_CTL = pad_ctl;
        /* missing TREN pin */
    }
}

void init_led_pins_as_pwm(void)
{
    /* Blue */
    HPM_IOC->PAD[IOC_PAD_PB19].FUNC_CTL = IOC_PB19_FUNC_CTL_PWM0_P_7;
    /* Green */
    HPM_IOC->PAD[IOC_PAD_PB01].FUNC_CTL = IOC_PB01_FUNC_CTL_PWM1_P_1;
    /* Red */
    HPM_IOC->PAD[IOC_PAD_PA27].FUNC_CTL = IOC_PA27_FUNC_CTL_PWM3_P_07;
}
