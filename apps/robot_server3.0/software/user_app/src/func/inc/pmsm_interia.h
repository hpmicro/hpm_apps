/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef PMSM_INTERIA_H_
#define PMSM_INTERIA_H_

#include "Parm_Global.h"

#include "pmsm_currentctrl.h"
#include "libhpm_motor.h"
#include "data_trans.h"
#include "Parm_Global.h"
#include "libhpm_motor_inertia_identification.h"

/**
 * @brief PMSM interia function;惯量辨识功能API
 * @addtogroup PMSM_INTERIA_API
 * @{
 *
 */ 
extern INERTIA_CAL_Obj INTERIA_CalHdl;

/**
 * @brief         Convert a 3p3z controller to a pid controller
 *                ;惯量辨识参数初始化
 * @param[in]     para   Structural body with inertia identification related parameters
 *                 ;惯量辨识相关参数的结构体
 */
void interia_param_init(Para_Obj* para);
/**
 * @brief         Convert a 3p3z controller to a pid controller
 *                ;惯量辨识参数初始化
 * @param[in]     para   Structural body with inertia identification related parameters
 *                 ;惯量辨识相关参数的结构体
 */
void motor_interiactrl(MOTOR_PARA* par, uint8_t interia_ctrl, CMDGENE_PARA* par_cmd);
/** @} */

#endif //_PMSM_INTERIA_H_