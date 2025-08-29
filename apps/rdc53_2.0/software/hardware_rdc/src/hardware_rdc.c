/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"

#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_adc.h"
#include "rdc_init.h"
#include "hpm_rdc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_dac_drv.h"
#include "math.h"
#include "hpm_adc16_drv.h"



int main(void)
{
/***************system clock init***********************/
    board_init(); 
/*******************************************************/
/************RDC Init***********************************/
    rdc_init();
/*******************************************************/
/************pwm init for pwm ISR **************************/
    Pwm_Duty_Init();
/*******************************************************/
/************pwm ISR for speed cal*********************/
    Pwm_TrigFor_interrupt_Init(); 
/******************************************************/
    
    while(1)
    {

    }
    
}


