/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef _PMSM_PROTECT_H_
#define _PMSM_PROTECT_H_

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "Parm_Global.h"
#include "pmsm_currentctrl.h"
#include "hpm_pwmv2_drv.h"
#include "hpm_adc.h"
#include "hpm_gpio_drv.h"
#include "pmsm_define.h"
#include "hpm_qeiv2_drv.h"

/**
 * @brief PMSM PROTECT API ;电机保护函数接口
 * @addtogroup PMSM_PROTECT_API
 * @{
 *
 */

/**
 * @brief     protect parameter struct
 *            ;保护参数结构体
 */
typedef struct {
    uint16_t maxvoltage;  //过压阈值
    uint16_t minvoltage;  //欠压阈值
    uint16_t times_vol;  //母线电压检测周期
    uint16_t maxcurrent;
    uint16_t times_current;
    uint16_t maxtemp;
    uint16_t mintemp;
    uint16_t times_temp;
    float maxmin_speed;
    uint16_t times_speed;
    uint16_t times_encoder;
    uint16_t errorword;

}MOTOR_CONTROL_PROTECT;
#define BUS_VOLTAGE_THR_MAX   0X3700        //24V 0x32ad;26V 0X36E2 22V 0X2E6B

#define GPIO_IRQ_INPUT       IRQn_GPIO0_D

/**
 * @brief     Current protect initialization
 *            ;电流保护初始化
 */
void init_current_protect(void);
/**
 * @brief     Voltage and current protection detection
 *            ;电压、电流保护检测
 * @return    return 0 : no fault,-1 : with fault.
              ;0:表示没有异常，-1:表示有异常。
 */
int voltage_current_protect(void);
/**
 * @brief     Initialization of motor protection function parameters
 *            ;电机保护功能参数初始化
 */
void motor_protect_param_init(MOTOR_PARA *motor_par);
extern MOTOR_CONTROL_PROTECT Motor_Control_Protect;
/**
 * @brief     protect function
 *            ;电机故障保护功能
 */
extern void motor_protect(MOTOR_PARA *par, MOTOR_CONTROL_Global* global);

/** @} */

#endif//_PMSM_PROTECT_H_
