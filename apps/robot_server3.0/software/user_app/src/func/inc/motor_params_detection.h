/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MOTOR_PARAMS_DETECTION_H
#define MOTOR_PARAMS_DETECTION_H  
#include "math.h"
#include "stdint.h"
#include "board.h"

/**
 * @brief MOTOR PARAMS DETECTION API ;电机参数辨识API
 * @addtogroup MOTOR_PARAMS_DETECTION_API
 * @{
 *
 */

/**
 * @brief     set ud value
 *            ;给定ud值
 */
#define  Ud_REF (12)
/**
 * @brief     set uq value
 *            ;给定uq值
 */
#define  Uq_REF (12)

/**
 * @brief     param detection result
 *            ;电机参数辨识结果
 */
typedef struct {
    float rs;
    float ld;
    float lq;
} param_detection_result_t;

/**
 * @brief     param detection mode
 *            ;枚举参数辨识参数
 */
typedef enum {
    offline_param_detection_mode_init = 0,
    offline_param_detection_mode_rs = 1,
    offline_param_detection_mode_ld = 2,
    offline_param_detection_mode_lq = 3,
    offline_param_detection_mode_wait = 6,
    offline_param_detection_mode_success = 7,
    offline_param_detection_mode_error = 8,
} param_detection_mode_t;

/**
 * @brief     param detection status
 *            ;参数辨识状态
 */
typedef enum {
   success=1,
   fail=2,
   running=3,
}param_detection_status;

/**
 * @brief     param detection config
 *            ;参数辨识配置结构体
 */
typedef struct {
    float inductor_detection_times; /**< Number of times to detect the inductor, The time is not easy to be too large, usually 1ms*/
    float delay_times;  /**< Intervals between different tests, Ensure that the current drain is clean to prevent interference with the next item. */
    float detection_loop_ts;    /**< Recall interval for detecting loops */
    uint32_t Ts_RLD;
    uint32_t vbus;
} param_detection_cfg_t;

/**
 * @brief     pi params struct
 *            ;pi参数结构体
 */
typedef struct{
float kp_d;
float ki_d;
float kp_q;
float ki_q;
float i_max;
}pi_params;

/**
 * @brief     params detection struct
 *            ;参数辨识结构体
 */
 typedef struct motor_params_dection_t{
    param_detection_cfg_t cfg;
    param_detection_result_t result;
    pi_params pi_par;
    param_detection_mode_t mode;
    param_detection_mode_t last_mode;
    param_detection_status status; 
    float tick_count;
    struct {
    float i_alpha;
    float i_beta;
    float ud_ref;
    float uq_ref;
    }common_use;
    struct {
        float is_last;
        float is;
    } ls;
}params_detection_t;

/**
 * @brief     motor params detection struct
 *            ;电机参数辨识控制结构体
 */
typedef struct{
  bool enable_offline_param_detection;
  params_detection_t    detection_t;
}BLDC_CONTROL_DETECTION_PARA;

/**
 * @brief         motor Rs detcetion
 *                ;电阻辨识函数
 * @param[in]     detection        detection struct;参数辨识结构体
 */
param_detection_status  Rs_detection_pro(params_detection_t *detection);

/**
 * @brief         detcetion  init
 *                ;辨识初始化
 * @param[in]     detection        detection struct;参数辨识结构体
 */
void detection_init(params_detection_t *detection);

/**
 * @brief         motor Ld detcetion 
 *                ;d轴电感辨识函数
 * @param[in]     detection        detection struct;参数辨识结构体
 */
param_detection_status  Ld_detection_pro(params_detection_t *detection);

/**
 * @brief         motor Lq detcetion 
 *                ;q轴电感辨识函数
 * @param[in]     detection        detection struct;参数辨识结构体
 */
param_detection_status Lq_detection_pro(params_detection_t *detection);

/** @} */

#endif