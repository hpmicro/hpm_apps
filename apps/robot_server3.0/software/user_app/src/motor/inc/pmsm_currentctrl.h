/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef Steppping_Motor_CurrentCtrl_H_
#define Steppping_Motor_CurrentCtrl_H_

#include "pmsm_encoder.h"
#include "Parm_Global.h"
#include "hpm_pwmv2_drv.h"
#include "libhpm_motor.h"
#if defined  (CONFIG_USE_FUNCTION) && (CONFIG_USE_FUNCTION)
#include "pmsm_detection.h"
#endif

extern MOTOR_CONTROL_Global Motor_Control_Global;
extern CMDGENE_PARA CMDGENEObj;
extern MOTOR_PARA motor;
extern qei_CalObj qeiCalObj;
extern function_par func_par;

/**
 * @brief PMSM CURRENTCTRL API ;电流环控制接口函数
 * @addtogroup PMSM_CURRENTCTRL_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     calculate electrical Angle from by encoder position
 *            ;电气角度计算
 * @param[in]    qei_CalHdl    encoder param;编码器参数
 * @param[in]    global    global struct;全局变量结构体
 */
void electric_angle_cal( qei_CalObj* qei_CalHdl, MOTOR_CONTROL_Global* global);
/**
 * @brief     Config PWM  and electrical Angle while looking for initial phase angle
 *            ;配置电角度以及pwm占空比执行预定位操作
 * @param[in]    motor    motor param;电机控制结构体
 * @param[in]    global    global struct;全局变量结构体
 * @param[in]    qei_CalHdl    encoder param;编码器参数
 */
void motor_angle_align_loop(MOTOR_PARA* motor, MOTOR_CONTROL_Global* global);
/**
 * @brief     current loop process
 *            ;电流环运行
 * @param[in]    motor    motor param;电机控制结构体
 * @param[in]    global    global struct;全局变量结构体
 * @param[in]    qei_CalHdl    encoder param;编码器参数
 */
void  motor_highspeed_loop(MOTOR_PARA* motor, MOTOR_CONTROL_Global* global, qei_CalObj* qei_CalHdl,function_par* func);

/** @} */

#endif //__PMSM_CURRENTCTRL_H