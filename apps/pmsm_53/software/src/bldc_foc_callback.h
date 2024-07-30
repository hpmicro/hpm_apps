/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef BLDC_FOC_CALLBACK_H
#define BLDC_FOC_CALLBACK_H

#include "board.h"
#include "hpm_pwm_drv.h"
#include "hpm_bldc_define.h"
#include "parm_global.h"

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
 * @brief        pwm disable;PWM下使能
 * @param[in]    motor_index    motor id;电机ID
 * @param[in]    pin_name   pwm pin;PWM引脚名
 */
void bldc_pwm_disable(uint8_t motor_index,uint8_t pin_name);

/**
 * @brief        set pwm cmp value;PWM cmp值下发
 * @param[in]    pwm_x    PWM struct; PWM 结构体
 * @param[in]    index   motor id;电机ID
 * @param[in]    cmp   cmp value;比较值
 */
void pwm_cmp_force_value_step(PWM_Type *pwm_x, uint8_t index, uint32_t cmp);


/**
 * @brief        set pwm cmp value;PWM占空比更新
 * @param[in]    par    PWM output parameters; PWM 输出参数
 */
void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par);

/**
 * @brief        disable pwm output;PWM输出下使能
 * @param[in]    ptr    PWM struct; PWM结构体
 */
void disable_all_pwm_output(PWM_Type *ptr);

/**
 * @brief        enable pwm output;PWM输出使能
 * @param[in]    ptr    PWM struct; PWM结构体
 */
void enable_all_pwm_output(PWM_Type *ptr);

/** @} */

#endif //__BLDC_FOC_CALLBACK_H
