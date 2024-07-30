/*
 * Copyright (c) 2023 HPMicro
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
#include "pinmux.h"

void init_uart_pins(UART_Type *ptr)
{
    if (ptr == HPM_UART0) {
        HPM_IOC->PAD[IOC_PAD_PA00].FUNC_CTL = IOC_PA00_FUNC_CTL_UART0_TXD;
        HPM_IOC->PAD[IOC_PAD_PA01].FUNC_CTL = IOC_PA01_FUNC_CTL_UART0_RXD;
    } else if (ptr == HPM_UART2) {
        HPM_IOC->PAD[IOC_PAD_PB08].FUNC_CTL = IOC_PB08_FUNC_CTL_UART2_TXD;
        HPM_IOC->PAD[IOC_PAD_PB09].FUNC_CTL = IOC_PB09_FUNC_CTL_UART2_RXD;
    } else {
        ;
    }
}

void init_adc_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB01].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB11].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;    
}
