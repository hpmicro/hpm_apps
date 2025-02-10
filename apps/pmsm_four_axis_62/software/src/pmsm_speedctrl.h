/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef PMSM_SPEEDCTRL_H_
#define PMSM_SPEEDCTRL_H_

#include "hpm_gptmr_drv.h"
#include "hpm_qei_drv.h"
#include "parm_global.h"
#include "libhpm_motor.h"

/**
 * @brief PMSM SPEED/POSITION API ;速度环/位置环控制接口函数
 * @addtogroup POS_VEL_CTRL_API
 * @{
 *
 */

/**
 * @brief     calculate Feedback position 
 *            ;反馈位置计算
 * @param[in]    ptr_qei   qei struct;qei结构体
 * @param[in]    MAX_cnt   max pluse count;编码器一圈脉冲数
 * @return       motor position;反馈位置
 */
int32_t pmsm_foc_get_pos(QEI_Type *ptr_qei, int32_t MAX_cnt);

/**
 * @brief     pos_vel loop 
 *            ;速度环/位置环API
 * @param[in]    par   motor struct;电机参数结构体
 * @param[in]    ptr   pwm struct;pwm结构体
 * @param[in]    global_control   global variable struct;全局变量结构体
 * @param[in]    ptr_qei   qei struct;qei结构体
 * @param[in]    MAX_cnt   max pluse count;编码器一圈脉冲数
 */
void motor_speed_ctrl(MOTOR_PARA *par, PWM_Type *ptr, MOTOR_CONTROL_Global* global_control, QEI_Type *ptr_qei, uint32_t MAX_cnt);

/** @} */

#endif //__PMSM_SPEEDTCTRL_H