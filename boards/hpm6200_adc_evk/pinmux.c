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

void init_adc_pins(void)
{
    /* ADC0.INA8 */
    HPM_IOC->PAD[IOC_PAD_PC12].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC0.INA9 */
    HPM_IOC->PAD[IOC_PAD_PC13].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC0.INA10 */
    HPM_IOC->PAD[IOC_PAD_PC14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    /* ADC0.INA11 */
    HPM_IOC->PAD[IOC_PAD_PC15].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
}

void init_usb_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PC23].FUNC_CTL = IOC_PC23_FUNC_CTL_GPIO_C_23;
    HPM_IOC->PAD[IOC_PAD_PC23].PAD_CTL = IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1);
}

