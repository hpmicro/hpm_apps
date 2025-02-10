/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef PMSM_CURRENTCTRL_H_
#define PMSM_CURRENTCTRL_H_

#include "parm_global.h"
#include "hpm_pwm_drv.h"
#include "hpm_adc.h"
#include "hpm_gpio_drv.h"
#include "hpm_bldc_define.h"
#include "hpm_qei_drv.h"

/**
 * @brief PMSM CURRENTCTRL API ;电流环控制接口函数
 * @addtogroup PMSM_CURRENTCTRL_API
 * @{
 *
 */

/**
 * @brief     current loop process
 *            ;电流环运行
 */
void motor0_highspeed_loop(void);

/**
 * @brief     Config PWM  and electrical Angle while looking for initial phase angle
 *            ;配置电角度以及pwm占空比执行预定位操作
 */
void motor0_angle_align_loop(void);

void motor1_highspeed_loop(void);
void motor1_angle_align_loop(void);

void motor2_highspeed_loop(void);
void motor2_angle_align_loop(void);

void motor3_highspeed_loop(void);
void motor3_angle_align_loop(void);

/** @} */

#endif //__PMSM_CURRENTCTRL_H