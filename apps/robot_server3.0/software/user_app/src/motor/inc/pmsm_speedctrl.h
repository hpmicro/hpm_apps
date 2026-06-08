/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef Steppping_Motor_SpeedCtrl_H_
#define Steppping_Motor_SpeedCtrl_H_

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "Parm_Global.h"
#include "hpm_csr_regs.h"
#include "hpm_gptmr_drv.h"
#include "hpm_gpio_drv.h"
#include "parm_global.h"
#include "hpm_qeiv2_drv.h"
#include "pmsm_currentctrl.h"
#include "pmsm_protect.h"
#include "data_trans.h"
#include "libhpm_motor.h"
/**
 * @brief PMSM SPEEDCTRL API ;速度控制接口函数
 * @addtogroup PMSM_SPEEDTCTRL_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     Operation mode
 *            ;运行模式
 */
typedef enum _tag_opmode{
POSITION_MODE,
SPEED_MODE,
}OP_MODE;


extern MOTOR_CONTROL_Global Motor_Control_Global;
extern CMDGENE_PARA CMDGENEObj;
extern MOTOR_PARA motor;
extern qei_CalObj qeiCalObj;
extern Para_Obj paraObj[1];
/**
 * @brief     Speedloop ctrl
 *            ;速度环控制
 * @param[in]    par    motor  param;电机参数结构体
 * @param[in]    ptr    pwm  param;pwm结构体
 * @param[in]    Motor_COntrol_Word    controlword;运行控制指令
 * @param[in]    CMDGENEObj    cmd struct;指令规划结构体
 */
void speedloop_ctrl(MOTOR_PARA *par, PWMV2_Type *ptr, MOTOR_CONTROL_Global * Motor_Control_Word, CMDGENE_PARA* CMDGENEObj );
/**
 * @brief     Positionloop ctrlt
 *            ;位置环控制
 * @param[in]    par    motor  param;电机参数结构体
 * @param[in]    ptr    pwm  param;pwm结构体
 * @param[in]    Motor_COntrol_Word    controlword;运行控制指令
 * @param[in]    CMDGENEObj    cmd struct;指令规划结构体
 * @param[in]    qei_CalHdl    encoder struct;编码器结构体
 */
void positionloop_ctrl(MOTOR_PARA *par, PWMV2_Type *ptr, MOTOR_CONTROL_Global * Motor_Control_Word, CMDGENE_PARA* CMDGENEObj,qei_CalObj* qei_CalHdl );

/** @} */

#endif //__PMSM_SPEEDTCTRL_H