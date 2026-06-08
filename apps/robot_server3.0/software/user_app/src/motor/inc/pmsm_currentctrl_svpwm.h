/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_FOC_H
#define HPM_FOC_H
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#include "pmsm_math.h"
#include "pmsm_define.h"
#include "parm_global.h"
/**
 * @brief MCL_FOC_INTERFACE ;矢量控制接口函数
 * @addtogroup FOC_INTERFACE
 * @{
 *
 */

/**
 * @brief     Calculation of speed by angular difference of sampling
 *            ;根据角度差计算速度
 * @param[in]    par    speed param;速度参数
 */
void bldc_foc_al_speed(BLDC_CONTRL_SPD_PARA  *par);

 /**
 * @brief     Update output pwm according to duty cycle, provided by the user
 *            ;占空比更新
 * @param[in]    par    pwm param;pwm参数
 */
void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par);

/**
 * @brief     Reconfiguring three-phase currents
 *            ;三相电流计算
 * @param[in]    par    currnet param;电流参数
 */
void bldc_foc_current_cal(BLDC_CONTROL_CURRENT_PARA *par);

 /**
 * @brief     pi control function
 *            ;pi函数
 * @param[in]    par    PID param;pid参数
 */
void bldc_foc_pi_contrl(BLDC_CONTRL_PID_PARA *par);

/**
 * @brief      Clark Transformation
 *             ;CLARK变换
 * @param[in]  currentu         U-phase current;U相电流
 * @param[in]  currentv         V-phase current;V相电流
 * @param[in]  currentw         W-phase current;W相电流
 * @param[out] currentalpha    alpha-axis current;alpaha轴电流
 * @param[out] currentbeta     beta-axis current;beta轴电流
 */
void bldc_foc_clarke(HPM_MOTOR_MATH_TYPE currentu, HPM_MOTOR_MATH_TYPE currentv, HPM_MOTOR_MATH_TYPE currentw,
             HPM_MOTOR_MATH_TYPE *currentalpha, HPM_MOTOR_MATH_TYPE *currentbeta);

/**
 * @brief      Park transform
 *             ;park变换
 * @param[in]  currentalpha  alpha-axis current;alpaha轴电流
 * @param[in]  currentbeta   beta-axis current;beta轴电流
 * @param[out] currentd      d-axis current;d轴电流
 * @param[out] currentq      q-axis current;q轴电流
 * @param[in]  sin_angle     sin(theta);sin角度
 * @param[in]  cos_angle     cos(theta);cos角度
 */
void bldc_foc_park(HPM_MOTOR_MATH_TYPE currentalpha, HPM_MOTOR_MATH_TYPE currentbeta,
                   HPM_MOTOR_MATH_TYPE *currentd, HPM_MOTOR_MATH_TYPE *currentq,
                   HPM_MOTOR_MATH_TYPE sin_angle, HPM_MOTOR_MATH_TYPE cos_angle);

/**
 * @brief       anti_park transform
 *              ;反park变换
 * @param[in]   currentalpha      alpha-axis current;alpaha轴电流
 * @param[in]   currentbeta       beta-axis current;beta轴电流
 * @param[out]  currentd          d-axis current;d轴电流
 * @param[out]  currentq          q-axis current;q轴电流
 * @param[in]   sin_angle         sin(theta);sin角度
 * @param[in]   cos_angle         cos(theta);cos角度
 */
void bldc_foc_inv_park(HPM_MOTOR_MATH_TYPE ud, HPM_MOTOR_MATH_TYPE uq,
                    HPM_MOTOR_MATH_TYPE *ualpha, HPM_MOTOR_MATH_TYPE *ubeta,
                    HPM_MOTOR_MATH_TYPE sin_angle, HPM_MOTOR_MATH_TYPE cos_angle);

/**
 * @brief          svpwm function
 *                 ;SVPWM函数接口
 * @param[in]      par                 pwm params;pwm参数
 */
void bldc_foc_svpwm(BLDC_CONTROL_PWM_PARA *par);


/**
 * @brief         dq-axis voltage conversion to pwm output
 *                ;dq轴电压转化为pwm输出
 * @param[in]     par                  foc params;foc参数
 */
void bldc_foc_ctrl_dq_to_pwm(BLDC_CONTROL_FOC_PARA *par);
/**
 * @brief         alculation of speed by angular difference of sampling;转速计算
 *
 * @param[in]     par                  Speed parameters;速度计算参数
 */
void pmsm_foc_al_speed(MOTOR_PARA  *par);


#if defined(__cplusplus)
}
#endif /* __cplusplus */
/** @} */
#endif
