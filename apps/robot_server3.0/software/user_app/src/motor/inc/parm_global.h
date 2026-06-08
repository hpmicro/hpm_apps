/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PARM_GLOBAL_H_
#define PARM_GLOBAL_H_

#include "board.h"
#include "bldc_foc_cfg.h"
#include "hpm_adc.h"
#include "pmsm_define.h"
#if defined (CONFIG_USE_FUNCTION) && (CONFIG_USE_FUNCTION)
#include "motor_params_detection.h"
#endif

/**
 * @brief PARM GLOBAL API ;全局参数接口函数
 * @addtogroup PARM_GLOBAL_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/
/**
 * @brief     Encoder parameter calculation
 *            ;abz编码器参数计算
 */
typedef struct {
      int32_t z;
      int32_t ph;
      int32_t maxph;
      uint8_t motor_pole;
      int32_t elec_angle;
      int32_t pos;
}qei_CalObj;
/**
 * @brief     Motor0 clock frequency
 *            ;轴0时钟频率  单位hz
 */
#define motor0_clock_hz              clock_get_frequency(BOARD_STEPPER0_MOTOR_CLOCK_SOURCE)
/**
 * @brief    PWM frequency
 *            ;PWM频率  单位hz
 */
#define PWM_FREQUENCY               (20000) 
/**
 * @brief     PWM reload
 *            ;PWM reload值
 */
#define PWM_RELOAD                  (motor0_clock_hz/PWM_FREQUENCY) 
/**
 * @brief     SMC function switch
 *            ;SMC功能开关
 */
#define MOTOR0_SMC_EN              0

typedef enum {
param_detection = 1,
}function_btn;


typedef struct {
#if defined (CONFIG_USE_FUNCTION) && (CONFIG_USE_FUNCTION)
BLDC_CONTROL_DETECTION_PARA detection_par;
#endif
}function_par;
/**
 * @brief     Global control variable
 *            ;全局控制变量
 */
typedef struct {
    uint8_t        ControlWord;
    float          Speed_Ref; 
    float          Pulse_ref; 
    float          Pulse_Time_Ref; 
    int16_t        Iq_Ref;
    int16_t        Motor0_ControlWord;
    int16_t        motor_CW;
    uint8_t        function_CW;
    int16_t        zero_CW;
    float          Pulse_Time; 
    uint8_t        smc_start_flag;
    int16_t        Motor0_set_angle;
    uint8_t        OP_mode;
    int16_t        Motor0SVC_start_times;
    uint8_t        flag_1ms;
    float          commu_theta;
    float          commu_IdRef;
    uint8_t        motor_status;
    uint8_t        zero_cnt;
    int32_t        ph0;
    int32_t        pos0;
    float          speed;
    float          Motor_Encoder_Type;
    float          Motor_ElecInit;
    uint8_t        Motor_Currentctrl_3P3Z;
    uint8_t        Motor_Interia;
    uint8_t        Motor_Currentctrl_ParaObser;
    uint8_t        Motor_Currentctrl_ParaCal;
}MOTOR_CONTROL_Global;

/**
 * @brief     Control word
 *            ;运动控制字
 */
typedef enum _tag_controlword{
motor_zero_control = 0x1210,
}MOTOR_CW;
 
  /**
 * @brief     Status word
 *            ;预定位状态字
 */
typedef enum _tag_statusword{
motor_zero_status = 0x0001,
}MOTOR_SW;

/**
 * @brief     Preset bit control word
 *            ;预定位控制字
 */
typedef enum _tag_motor_zero_control{
motor0_zero_control = 0x1210,
}Motor_zero_control;

/**
 * @brief     Preset bit state
 *            ;预定位状态
 */
typedef struct _tag_motor_status{
int16_t zero_status;
}Motor_Status;



/**
 * @brief     Motor parameter structure
 *            ;电机参数结构体
 */
typedef struct {
    /** @brief Control parameter structure    
     *         ;控制参数结构体 */
    BLDC_CONTROL_FOC_PARA       foc_para;
    /** @brief Velocity parameter structure    
     *         ;速度参数结构体 */
    BLDC_CONTRL_PID_PARA        speedloop_para; 
#if MOTOR0_SMC_EN
    BLDC_CONTROL_SMC_PARA       smc_para;
#endif
    /** @brief Positional parameter structure    
     *         ;位置参数结构体 */
    BLDC_CONTRL_PID_PARA        position_para;
    void (*adc_trig_event_callback)();
}MOTOR_PARA;

/**
 * @brief User-defined data with data and enable bits
 *
 */
typedef struct {
    float value;
    bool enable;
} mcl_user_value_t;

/** @} */

#endif //__PARM_GLOBAL_H