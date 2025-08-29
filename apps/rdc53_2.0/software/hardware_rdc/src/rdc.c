/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "rdc.h"
#include "uart_init.h"
//#include "SEGGER_RTT.h"
#include "gpio_led.h"
#include "rdc_cfg.h"
#include "hpm_debug_console.h"
#include "tamagawa_slave_init.h"
#include "hpm_gpiom_drv.h"



int32_t buffer[2048] = {0};
int main(void)
{
#if SEGGER_RTT_DEBUG
    board_init_rtt_test();
    SEGGER_RTT_ConfigUpBuffer(0, NULL, &buffer, sizeof(buffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#else
    board_init();
#endif

#if TAMAGAWA_SLAV
#if TAMAGAWA_SLAVE_POS_HARDWARE_INJECT
    sei_slave_pos_get();
    sei_slave_tamagawa_init();
#else
    sei_slave_tamagawa_init();
#endif
#endif

#if BISSC_SLAVE
#if BISSC_SLAVE_POS_HARDWARE_INJECT
    sei_slave_pos_get();
    bissc_slave_init();
#else
     bissc_slave_init();
#endif
#endif

    init_led_display();
#if LED_STATUS_ENABLE
    led_green_ok();
#endif
#if ABS_ENCODER_23BIT
    sei_config();
    board_delay_ms(10);
    sei_params_init(&sei_angleCalObj);
#endif

#if UART_DEBUG_CONTROL
    uart_cfg();
#elif SPI_DEBUG_CONTROL
    spi_salve_interrupt_config();
#endif
#if ABZ_OUTPUT
    Pwm_Duty_Init();
    Pwm_TrigFor_interrupt_Init();
    qeo_pins_init();
    qeo_gen_abz_signal_hardware();
#endif

#if PLL
    pll_para_init(&pll);
    pll_type_ii_cfg(&pll_ii);
#elif PLL_II
    pll_type_ii_cfg(&pll_ii);
#endif

/////RDC_CFG////////////////////////////
    rdc_init();
   
    
#if LED_STATUS_ENABLE
    led_blue_work();
#endif
    while(1)
    {
#if UART_DEBUG_CONTROL
       uart_trans_pro();
#elif SPI_DEBUG_CONTROL
       spi_trans_data();
#endif

    }
}



