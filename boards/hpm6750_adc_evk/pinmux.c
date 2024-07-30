/*
 * Copyright (c) 2021-2022 HPMicro
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
        /* PY port IO needs to configure PIOC as well */
        HPM_PIOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY06_FUNC_CTL_SOC_PY_06;
        HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY07_FUNC_CTL_SOC_PY_07;
    } else if (ptr == HPM_UART6) {
        HPM_IOC->PAD[IOC_PAD_PE27].FUNC_CTL = IOC_PE27_FUNC_CTL_UART6_RXD;
        HPM_IOC->PAD[IOC_PAD_PE28].FUNC_CTL = IOC_PE28_FUNC_CTL_UART6_TXD;
    } else if (ptr == HPM_UART7) {
        HPM_IOC->PAD[IOC_PAD_PC02].FUNC_CTL = IOC_PC02_FUNC_CTL_UART7_RXD;
        HPM_IOC->PAD[IOC_PAD_PC03].FUNC_CTL = IOC_PC03_FUNC_CTL_UART7_TXD;
    } else if (ptr == HPM_UART13) {
        HPM_IOC->PAD[IOC_PAD_PZ08].FUNC_CTL = IOC_PZ08_FUNC_CTL_UART13_RXD;
        HPM_IOC->PAD[IOC_PAD_PZ09].FUNC_CTL = IOC_PZ09_FUNC_CTL_UART13_TXD;
        /* PZ port IO needs to configure BIOC as well */
        HPM_BIOC->PAD[IOC_PAD_PZ08].FUNC_CTL = IOC_PZ08_FUNC_CTL_SOC_PZ_08;
        HPM_BIOC->PAD[IOC_PAD_PZ09].FUNC_CTL = IOC_PZ09_FUNC_CTL_SOC_PZ_09;
    } else if (ptr == HPM_UART14) {
        HPM_IOC->PAD[IOC_PAD_PZ10].FUNC_CTL = IOC_PZ10_FUNC_CTL_UART14_RXD;
        HPM_IOC->PAD[IOC_PAD_PZ11].FUNC_CTL = IOC_PZ11_FUNC_CTL_UART14_TXD;
        /* PZ port IO needs to configure BIOC as well */
        HPM_BIOC->PAD[IOC_PAD_PZ10].FUNC_CTL = IOC_PZ10_FUNC_CTL_SOC_PZ_10;
        HPM_BIOC->PAD[IOC_PAD_PZ11].FUNC_CTL = IOC_PZ11_FUNC_CTL_SOC_PZ_11;
    } else if (ptr == HPM_PUART) {
        HPM_PIOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY07_FUNC_CTL_PUART_RXD;
        HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_PUART_TXD;
    }
}

void init_led_pins_as_gpio(void)
{
    HPM_IOC->PAD[IOC_PAD_PB18].FUNC_CTL = IOC_PB18_FUNC_CTL_GPIO_B_18;
}

void init_adc12_pins(void)
{
    /* ADC0/1/2/.VINP4 */
    HPM_IOC->PAD[IOC_PAD_PE18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC0/1/2/.VINP5 */
    HPM_IOC->PAD[IOC_PAD_PE19].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC0.VINP13 */
    HPM_IOC->PAD[IOC_PAD_PE27].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
}

void init_adc16_pins(void)
{
     /* ADC0.INA4 */
    HPM_IOC->PAD[IOC_PAD_PE18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC3.INA0 */
    HPM_IOC->PAD[IOC_PAD_PE19].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC3.INA1 */
    HPM_IOC->PAD[IOC_PAD_PE27].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
}

void init_usb_pins(USB_Type *ptr)
{
    if (ptr == HPM_USB0) {
        /* USB0 ID */
        HPM_IOC->PAD[IOC_PAD_PF10].FUNC_CTL = IOC_PF10_FUNC_CTL_GPIO_F_10;
        HPM_IOC->PAD[IOC_PAD_PF10].PAD_CTL = IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1);

        /* USB0 OC */
        HPM_IOC->PAD[IOC_PAD_PF08].FUNC_CTL = IOC_PF08_FUNC_CTL_GPIO_F_08;
        HPM_IOC->PAD[IOC_PAD_PF08].PAD_CTL = IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1);
    }
}
