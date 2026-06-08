/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef _PMSM_3P3Z_H
#define _PMSM_3P3Z_H

#include <stdio.h>
#include <stdlib.h>
#if defined  (CONFIG_USE_HPM6E00) &&(CONFIG_USE_FUNCTION)
#include "hpm_clc_drv.h"
#endif
#include "Parm_Global.h"
#include "hpm_synt_drv.h"

/**
 * @brief PMSM 3P3Z API ;3P3Z功能API
 * @addtogroup PMSM_3P3Z_API
 * @{
 *
 */
/**
 * @brief     run status
 *            ;函数运行状态
 */
typedef uint32_t hpm_mcl_stat_t;

/**
 * @brief     3p-3z Coefficients for 3p-3z controllers
 *            ;3p3z系数配置结构体
 */
typedef struct {
    float b0;
    float b1;
    float b2;
    float b3;
    float a0;
    float a1;
    float a2;
} mcl_clc_coeff_cfg_t;

/**
 * @brief         Convert a 3p3z controller to a pid controller
 *                ;转变3p3z控制器为pi控制器
 * @param[in]     cfg_pid   pid param;pid参数结构体
 * @param[out]          cfg_3p3z  3p3z struct;3p3z结构体
 */
hpm_mcl_stat_t hpm_mcl_pid_to_3p3z(BLDC_CONTRL_PID_PARA *cfg_pid, mcl_clc_coeff_cfg_t *cfg_3p3z);
/**
 * @brief         3p3z function initialization
 *                ;3p3z功能初始化
 */
void clc_3p3z_init(void);
/**
 * @brief         3p3z function
 *                ;3p3z功能函数
 * @param[out]     par   foc control;FOC控制结构体
 */
void CLC_control(BLDC_CONTROL_FOC_PARA* par);
/** @} */

#endif //_PMSM_3P3Z_H