/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PMSM_DETECTON_H_
#define PMSM_DETECTION_H_

//#include "parm_global.h"
#include "pmsm_currentctrl_svpwm.h"

 
/**
 * @brief PMSM detection API;电机参数辨识API
 * @addtogroup PMSM_DETECTION_API
 * @{
 *
 */

#define SQRT3      (1.7320508075688773f)    /**< sqrt(3) */
#define SQRT3_DIV3 (0.5773502691896258f)    /**< sqrt(3)/3 */
#define ADC_REF_VOL  (3.3)  ////adc参考电压
#define SAMPLE_PRECISION (4095)  ///3.3---4095
#define OPAMP_GAIN    (10)    ///放大倍数
#define MOTOR_Rs      (0.001)  ///电阻1mΩ
#define  CURRENLOOP_BANDWITH (2000)         //单位Hz,电流环带宽

/**
 * @brief        motor params detection config
 *               ;电机参数辨识配置函数
 * @param[in]    dection_para      detection param;辨识结构体
 * @param[in]    par               motor struct;电机参数结构体
 */
void motor_param_detection_cfg(BLDC_CONTROL_DETECTION_PARA* dection_para,MOTOR_PARA *par);

/**
 * @brief        motor params detection loop
 *               ;电机参数辨识函数
 * @param[in]    dection_para      detection param;辨识结构体
 * @param[in]    par               motor struct;电机参数结构体
 */
void  motor_param_detection_loop(BLDC_CONTROL_DETECTION_PARA* dection_para,MOTOR_PARA *par);


/**
 * @brief        calculate pi params
 *               ;pi参数计算函数
 * @param[in]    dection_para      detection param;辨识结构体
 */
void pi_params_get(BLDC_CONTROL_DETECTION_PARA* dection_para);

/** @} */
#endif