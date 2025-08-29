/*
 * Copyright (c) 2024 hpmicro
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
        HPM_IOC->PAD[IOC_PAD_PA00].FUNC_CTL = IOC_PA00_FUNC_CTL_UART0_TXD;
        HPM_IOC->PAD[IOC_PAD_PA01].FUNC_CTL = IOC_PA01_FUNC_CTL_UART0_RXD;
    } else if (ptr == HPM_UART4) {
        HPM_PIOC->PAD[IOC_PAD_PY00].FUNC_CTL = PIOC_PY00_FUNC_CTL_SOC_PY_00;
        HPM_IOC->PAD[IOC_PAD_PY00].FUNC_CTL = IOC_PY00_FUNC_CTL_UART4_TXD;
        /* pull-up */
        HPM_IOC->PAD[IOC_PAD_PY01].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        HPM_PIOC->PAD[IOC_PAD_PY01].FUNC_CTL = PIOC_PY01_FUNC_CTL_SOC_PY_01;
        HPM_IOC->PAD[IOC_PAD_PY01].FUNC_CTL = IOC_PY01_FUNC_CTL_UART4_RXD;
    } else if (ptr == HPM_PUART) {
        HPM_PIOC->PAD[IOC_PAD_PY00].FUNC_CTL = PIOC_PY00_FUNC_CTL_PURT_TXD;
        HPM_PIOC->PAD[IOC_PAD_PY01].FUNC_CTL = PIOC_PY01_FUNC_CTL_PURT_RXD;
    } else {
        ;
    }
}

/* for uart_lin case, need to configure pin as gpio to sent break signal */
void init_uart_pin_as_gpio(UART_Type *ptr)
{
    if (ptr == BOARD_UART_LIN) {
        /* pull-up */
        HPM_IOC->PAD[IOC_PAD_PY00].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        HPM_IOC->PAD[IOC_PAD_PY01].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

        HPM_PIOC->PAD[IOC_PAD_PY00].FUNC_CTL = PIOC_PY00_FUNC_CTL_SOC_PY_00;
        HPM_IOC->PAD[IOC_PAD_PY00].FUNC_CTL = IOC_PY00_FUNC_CTL_GPIO_Y_00;

        HPM_PIOC->PAD[IOC_PAD_PY01].FUNC_CTL = PIOC_PY01_FUNC_CTL_SOC_PY_01;
        HPM_IOC->PAD[IOC_PAD_PY01].FUNC_CTL = IOC_PY01_FUNC_CTL_GPIO_Y_01;
    }
}


// /* board PWM provide clock to SDM sensor */
// void init_pwm_pin_as_sdm_clock(void)
// {
//     HPM_IOC->PAD[IOC_PAD_PC02].FUNC_CTL = IOC_PC02_FUNC_CTL_PWM0_P_2;
// }

void init_gpio_pins(void)
{
    /* configure pad setting: pull enable and pull up, schmitt trigger enable */
    /* enable schmitt trigger to eliminate jitter of pin used as button */

    /* Button */
    HPM_IOC->PAD[IOC_PAD_PC23].FUNC_CTL = IOC_PC23_FUNC_CTL_GPIO_C_23;
    HPM_IOC->PAD[IOC_PAD_PC23].PAD_CTL = IOC_PAD_PAD_CTL_HYS_SET(1) | IOC_PAD_PAD_CTL_OD_SET(1);
}

void init_butn_pins(void)
{
    /* configure pad setting: pull enable and pull up, schmitt trigger enable */
    /* enable schmitt trigger to eliminate jitter of pin used as button */

    /* Button */
    HPM_IOC->PAD[IOC_PAD_PC23].FUNC_CTL = IOC_PC23_FUNC_CTL_GPIO_C_23;
    HPM_IOC->PAD[IOC_PAD_PC23].PAD_CTL = IOC_PAD_PAD_CTL_HYS_SET(1) | IOC_PAD_PAD_CTL_OD_SET(1);
}

void init_acmp_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD26].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; /* CMP4_INN6/CMP5_INN6/CMP6_INN6/CMP7_INN6 */
}

void init_pwm_fault_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PC02].FUNC_CTL = IOC_PC02_FUNC_CTL_TRGM_P_02;
}

void init_pwm_pins(PWMV2_Type *ptr)
{
    if (ptr == HPM_PWM1) {
        HPM_IOC->PAD[IOC_PAD_PD08].FUNC_CTL = IOC_PD08_FUNC_CTL_PWM1_P_0;
        HPM_IOC->PAD[IOC_PAD_PD09].FUNC_CTL = IOC_PD09_FUNC_CTL_PWM1_P_1;
        HPM_IOC->PAD[IOC_PAD_PD10].FUNC_CTL = IOC_PD10_FUNC_CTL_PWM1_P_2;
        HPM_IOC->PAD[IOC_PAD_PD11].FUNC_CTL = IOC_PD11_FUNC_CTL_PWM1_P_3;
        HPM_IOC->PAD[IOC_PAD_PD12].FUNC_CTL = IOC_PD12_FUNC_CTL_PWM1_P_4;
        HPM_IOC->PAD[IOC_PAD_PD13].FUNC_CTL = IOC_PD13_FUNC_CTL_PWM1_P_5;
    }
}

void init_usb_pins(USB_Type *ptr)
{
    if (ptr == HPM_USB0) {
        /* USB0_P */
        HPM_IOC->PAD[IOC_PAD_PD00].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
        /* USB0_N */
        HPM_IOC->PAD[IOC_PAD_PD01].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
        /* USB0_ID */
        HPM_IOC->PAD[IOC_PAD_PC25].FUNC_CTL = IOC_PC25_FUNC_CTL_USB0_ID;
        /* USB0_OC */
        HPM_IOC->PAD[IOC_PAD_PC26].FUNC_CTL = IOC_PC26_FUNC_CTL_USB0_OC;
        /* USB0_PWR */
        HPM_IOC->PAD[IOC_PAD_PC27].FUNC_CTL = IOC_PC27_FUNC_CTL_USB0_PWR;
    }
}


void init_adc16_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC0/1.IN14 */
    HPM_IOC->PAD[IOC_PAD_PD15].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC0/1.IN115 */
    HPM_IOC->PAD[IOC_PAD_PD16].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC2/3.IN0 */
    HPM_IOC->PAD[IOC_PAD_PD17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC2/3.IN1 */
    HPM_IOC->PAD[IOC_PAD_PD18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC2/3.IN2 */
    HPM_IOC->PAD[IOC_PAD_PD19].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC2/3.IN3 */
 }

void init_adc_bldc_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC_IU: ADC0.14 / ADC1.14 */
    HPM_IOC->PAD[IOC_PAD_PD24].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC_IU: ADC2.08 / ADC3.08 */
    HPM_IOC->PAD[IOC_PAD_PD25].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC_IU: ADC2.09 / ADC3.09 */
}

void init_adc_qeiv2_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD25].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC_IW: ADC2.09 / ADC3.09 cos_ch */
    HPM_IOC->PAD[IOC_PAD_PD14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;        /* ADC_IU: ADC0.14 / ADC1.14 sin_ch */
}


void init_led_pins_as_gpio(void)
{
    HPM_IOC->PAD[IOC_PAD_PA25].FUNC_CTL = IOC_PA25_FUNC_CTL_GPIO_A_25;
}

void init_led_pins_as_pwm(void)
{
    HPM_IOC->PAD[IOC_PAD_PD08].FUNC_CTL = IOC_PD08_FUNC_CTL_TRGM_P_08;
}

void init_plb_pulse_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD10].FUNC_CTL = IOC_PD10_FUNC_CTL_TRGM_P_10;
}

void init_plb_ab_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PB23].FUNC_CTL = IOC_PB23_FUNC_CTL_TRGM_P_08;
    HPM_IOC->PAD[IOC_PAD_PB22].FUNC_CTL = IOC_PB22_FUNC_CTL_TRGM_P_09;
    HPM_IOC->PAD[IOC_PAD_PB21].FUNC_CTL = IOC_PB21_FUNC_CTL_TRGM_P_10;
}

void init_plb_lin_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD08].FUNC_CTL = IOC_PD08_FUNC_CTL_TRGM_P_08;
}

void init_plb_filter_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD10].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(0);
    HPM_IOC->PAD[IOC_PAD_PD10].FUNC_CTL = IOC_PD10_FUNC_CTL_TRGM_P_10;
    HPM_IOC->PAD[IOC_PAD_PD12].FUNC_CTL = IOC_PD12_FUNC_CTL_TRGM_P_12;
}

/* for uart_rx_line_status case, need to a gpio pin to sent break signal */
void init_uart_break_signal_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PY05].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
    HPM_IOC->PAD[IOC_PAD_PY05].FUNC_CTL = IOC_PY05_FUNC_CTL_GPIO_Y_05;
    HPM_PIOC->PAD[IOC_PAD_PY05].FUNC_CTL = PIOC_PY05_FUNC_CTL_SOC_PY_05;
}

void init_dac_pins(DAC_Type *ptr)
{
    if (ptr == HPM_DAC0) {
        HPM_IOC->PAD[IOC_PAD_PD07].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* DAC0.OUT */
    } else if (ptr == HPM_DAC1) {
        HPM_IOC->PAD[IOC_PAD_PD17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  /* DAC1.OUT */
    }
}


