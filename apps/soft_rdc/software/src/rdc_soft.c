/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_adc16_drv.h"
#include "SEGGER_RTT.h"
#include "adc_data_process.h"
#include "adc_dma.h"
#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif

#ifndef ADC_SOC_NO_HW_TRIG_SRC
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#endif
#include "adc_dma.h"
#include "pwm_dma.h"


#define SEGGER_RTT_DEBUG 1

int32_t buffer[2048] = {0};

int main(void)
{
    /* Bsp initialization */
    #if SEGGER_RTT_DEBUG
    board_init_rtt_test();
    SEGGER_RTT_ConfigUpBuffer(0, NULL, &buffer, sizeof(buffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
  #else
    board_init();
  #endif
    init_pwm_out_rdc();
    DAC_cfg();
    init_adc_motor1();
    timer_init();
    while (1) {
        adc_data_process();
     }
}
