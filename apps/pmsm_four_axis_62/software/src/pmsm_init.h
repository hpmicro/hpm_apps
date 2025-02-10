/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef PMSM_INIT_H_
#define PMSM_INIT_H_

#include "parm_global.h"
#include "hpm_adc.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_bldc_define.h"
#include "hpm_foc.h"
#include "hpm_smc.h"
#include "hpm_qei_drv.h"
#include "bldc_foc_callback.h"
#include "pmsm_currentctrl.h"
#include "pmsm_speedctrl.h"

/**
 * @brief PMSM INIT API ;电机初始化接口函数
 * @addtogroup PMSM_INIT_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     sync time
 *            ;四轴同步时间定义
 */
#define SYNC_TIME_0                 (1000U)
#define SYNC_TIME_1                 (2000U)
#define SYNC_TIME_2                 (3000U)
#define SYNC_TIME_3                 (4000U)
/**
 * @brief     PWM freq uint HZ
 *            ;pwm 频率
 */
#define PWM_FREQUENCY               (20000) 
/**
 * @brief     pwm reload value
 *            ;pwm 重载值
 */
#define PWM_RELOAD                  (motor0_clock_hz/PWM_FREQUENCY) 
/**
 * @brief     Zero drift calculate times(ms),must less than 250.
 *            ;电流采样次数，用于计算零漂
 */
#define BLDC_CURRENT_SET_TIME_MS    (200) 

extern uint32_t adc_buff[3][BOARD_PMSM0_ADC_SEQ_DMA_SIZE_IN_4BYTES];


/**
 * @brief     Parameter initialization
 *            ;参数初始化
 */
void parm_global_init(void);

/**
 * @brief     init kp,ki,pi control function
 *            ;pi控制器初始化
 * @param[in]    par   motor para to operate on;电机参数结构体
 * @param[in]    KP    proportional gain;比例增益
 * @param[in]    KI    integral gain;积分
 * @param[in]    MAX   output max;输出限幅
 */
void pi_para_init(BLDC_CONTRL_PID_PARA *par, float KP, float KI, float MAX);


 /**
 * @brief     init foc control para
 *            ;电机矢量控制初始化
 * @param[in]    par   foc contorl para to operate on;电机矢量控制参数结构体
 * @param[in]    MOTOR_ID    motor index;电机轴号
 */
void pmsm_param_init(BLDC_CONTROL_FOC_PARA *par, uint8_t MOTOR_ID);


 /**
 * @brief     Config PWM
 *            ;pwm配置
 * @param[in]    ptr   pwm struct;pwm结构体
 * @param[in]    PWM_PRD   pwm timer counter reload value;pwm重载值
 * @param[in]    CMP_SHADOW_REGISTER_UPDATE_TYPE   compare configuration update trigger;CMP值更新时机
 * @param[in]    CMP_COMPARE   compare work mode;比较器工作模式
 */
void pwm_duty_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE);


 /**
 * @brief     Config the PWM channel for ADC trigger
 *            ;pwm触发adc采样
 * @param[in]    ptr   pwm struct;pwm结构体
 * @param[in]    PWM_PRD   pwm timer counter reload value;pwm重载值
 * @param[in]    CMP_SHADOW_REGISTER_UPDATE_TYPE   compare configuration update trigger;CMP值更新时机
 * @param[in]    CMP_COMPARE   compare work mode;比较器工作模式
 * @param[in]    PWM_CH_TRIG_ADC   coutput channel compare start index;比较器通道号
 */
void pwm_trigfor_adc_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE, uint8_t PWM_CH_TRIG_ADC);

 /**
 * @brief      Config the PWM channel for current loop interrupt
 *            ;pwm触发电流环中断
 * @param[in]    ptr   pwm struct;pwm结构体
 * @param[in]    PWM_PRD   pwm timer counter reload value;pwm重载值
 * @param[in]    CMP_SHADOW_REGISTER_UPDATE_TYPE   compare configuration update trigger;CMP值更新时机
 * @param[in]    CMP_COMPARE   compare work mode;比较器工作模式
 * @param[in]    PWM_CH_TRIG_CURRENTCtrl   coutput channel compare start index;比较器通道号
 */
void pwm_trigfor_currentctrl_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE, uint8_t PWM_CH_TRIG_CURRENTCtrl);


/**
 * @brief     1ms timer for speed loop.
 *            ;1ms中断配置gptimer
 */
void timer_init(void);

 /**
 * @brief      Config TRGM output&input
 *            ;互联管理器配置
 * @param[in]    ptr   TRGM base address;trgm结构体
 * @param[in]    TRAG_INPUT   input selection;互联管理器输入
 * @param[in]    TRAG_INPUT_FOR_ADC   target output;互联管理器目标输出
 */
void init_trigger_mux(TRGM_Type * ptr, uint8_t TRAG_INPUT, uint8_t TRAG_INPUT_FOR_ADC);

/**
 * @brief      Config the preemption mode for an ADC instancet
 *            ;adc抢占模式配置
 * @param[in]    ptr   ADC  base address;adc结构体
 * @param[in]    trig_ch   trig channel;触发通道
 * @param[in]    channel   adc channel;adc采样通道
 * @param[in]    inten     inten;中断使能标志
 * @param[in]    ADC_MODULE   adc module;16位adc
 * @param[in]    ADC_PREEMPT_TRIG_LEN   tirg len;抢占转换队列长度
 */
void init_trigger_cfg(ADC16_Type *ptr, uint8_t trig_ch, uint8_t channel, bool inten, uint32_t ADC_MODULE, uint8_t ADC_PREEMPT_TRIG_LEN);


 /**
 * @brief     adc cfg init
 *            ;adc配置初始化
 * @param[in]    ptr   ADC  base address;adc结构体
 * @param[in]    channel   adc channel;adc通道
 * @param[in]    sample_cycle     adc sample cycle;adc采样个数
 * @param[in]    ADC_MODULE   adc module;16位adc
 */
void adc_cfg_init(ADC16_Type *ptr, uint8_t channel, uint32_t sample_cycle, uint32_t ADC_MODULE);

 /**
 * @brief     adc pins init
 *            ;adc引脚初始化
 * @param[in]    ptr   PWM base address;pwm结构体
 */
void adc_pins_init(PWM_Type *ptr);

 /**
 * @brief      get current value in steady state
 *            ;零漂计算
 * @param[in]    par   foc struct;矢量控制结构体
 * @param[in]    ADC_TRG    adc trig channel;adc通道
 */
void lv_set_adval_middle(BLDC_CONTROL_FOC_PARA *par, uint32_t ADC_TRG);

 /**
 * @brief     pwm pins init
 *            ;pwm引脚初始化
 * @param[in]    ptr   PWM base address;pwm结构体
 */
void pwm_pins_init(PWM_Type *ptr);


 /**
 * @brief      qei cfg
 *            ;qei配置
 * @param[in]    ptr   trgm struct;互联管理器结构体
 * @param[in]    ptr_qei    qei struct;qei结构体
 * @param[in]    trag_in_A    input selection;互联管理器输入
 * @param[in]    trag_out_A   target output;互联管理器输出
 * @param[in]    trag_in_B    input selection;互联管理器输入
 * @param[in]    trag_out_B   target output;互联管理器输出
 * @param[in]    QEI_IRQ    interrupt number;qei中断
 * @param[in]    MAX_cnt    maximum phcnt number;电机一圈脉冲数
 */
void qei_cfg_init(TRGM_Type *ptr, QEI_Type *ptr_qei, uint8_t trag_in_A, uint8_t trag_out_A, uint8_t trag_in_B, uint8_t trag_out_B, uint32_t QEI_IRQ, uint32_t MAX_cnt);

  /**
 * @brief     qei pins init
 *            ;qei引脚初始化
 * @param[in]    ptr   PWM base address;pwm结构体
 */
void qei_pins_init(PWM_Type *ptr);

/**
 * @brief     pwm enable at synci.
 *            ;pwm 同步使能配置
 */
void pwm_sync(void);

/**
 * @brief     PMSM Motor1 initialization
 *            ;轴0 adc pwm qei等初始化配置
 */
void pmsm_motor0_init(void);
void pmsm_motor1_init(void);
void pmsm_motor2_init(void);
void pmsm_motor3_init(void);
void pmsm_motor_init(void);
/**
 * @brief     Rotor Angle alignment aligns the encoder midpoint value with the actual physical Angle midpoint value .
 *            ;给定d轴电流，使得d轴与A轴重合，电角度为0.
 */
void pmsm0_foc_angle_align(void);
void pmsm1_foc_angle_align(void);
void pmsm2_foc_angle_align(void);
void pmsm3_foc_angle_align(void);
void pmsm_foc_angle_align(void);

/** @} */

#endif //PMSM_INIT_H_
