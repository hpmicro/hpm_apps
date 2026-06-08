/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef BLDC_FOC_CALLBACK_H
#define BLDC_FOC_CALLBACK_H

#include "pmsm_currentctrl_svpwm.h"
#include "bldc_foc_cfg.h"
#include "pmsm_define.h"
#include "board.h"
/**
 * @brief BLDC FOC CALLBACK API ;电机控制回调接口函数
 * @addtogroup BLDC_FOC_CALLBACK_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/


/**
 * @brief        set pwm cmp value;PWM占空比更新
 * @param[in]    par    PWM output parameters; PWM 输出参数
 */
void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par);

/**
 * @brief        disable pwm output;PWM输出下使能
 * @param[in]    ptr    PWM struct; PWM结构体
 */
void disable_all_pwm_output(PWMV2_Type *ptr);

/**
 * @brief        enable pwm output;PWM输出使能
 * @param[in]    ptr    PWM struct; PWM结构体
 */
void enable_all_pwm_output(PWMV2_Type *ptr);



/** @} */

#endif //__BLDC_FOC_CALLBACK_H
