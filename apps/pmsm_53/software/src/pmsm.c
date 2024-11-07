/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "pmsm.h"


int main(void)
{
   
    board_init();
    parm_UpdatePowerOn();

    monitor_init();

    pmsm_motor1_init();
    intc_m_enable_irq_with_priority(BOARD_PMSM0APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_PMSM0PWM, PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP));

    while(1)
    {
        monitor_handle();
        parm_GlobalManage();
    }
    return 0;
}

