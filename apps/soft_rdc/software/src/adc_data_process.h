/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef ADC_DATA_PROCESS_H
#define ADC_DATA_PROCESS_H
#include "board.h"

/**
 * @brief ADC sample API ;adc采样函数
 * @addtogroup adc_sample_API
 * @{
 *
 */

 /**
 * @brief     adc sample count
 *            ;adc采样个数
 */
#define RT_SAMPLE_NUM     100

/**
 * @brief     filter coefficient
 *            ;滤波系数
 */
#define SIN_COS_COF       0.31415926535898

/**
 * @brief     envelope print button
 *            ;包络线取出观测宏定义
 */
#define sin_cos_rtt_print    0
/**
 * @brief     Sampling signal processing structure
 *            ;采样信号处理结构体
 */
typedef struct {
    uint16_t        Sample_OSin[RT_SAMPLE_NUM];
    uint16_t        Sample_OCos[RT_SAMPLE_NUM];
    float           Sample_OExc[RT_SAMPLE_NUM];
    float           Sample_Sin[RT_SAMPLE_NUM];
    float           Sample_Cos[RT_SAMPLE_NUM];
    float           Sample_Exc[RT_SAMPLE_NUM];
    float           Sample_Sin_Out;
    float           Sample_Sin_Out_Last;
    float           Sample_Cos_Out;
    float           Sample_Cos_Out_Last;
    float           Sample_Exc_Out;
    float           Sample_Exc_Out_Last;
}MOTOR_CONTROL_Global;


/**
 * @brief     speed calculation
 *            ;速度计算结构体
 */
typedef struct _tag_speed_Cal_Obj{
      float speedtheta;
      float speedlasttheta ;
      float speedthetalastn ;
      int num ;
      float o_speedout_filter ;
      float o_speedout;
}speed_Cal_Obj;

/*Timer define*/
#define BOARD_BLDC_TMR_1MS    HPM_GPTMR2
#define BOARD_BLDC_TMR_CH     0
#define BOARD_BLDC_TMR_CMP    0
#define BOARD_BLDC_TMR_IRQ    IRQn_GPTMR2
#define BOARD_BLDC_TMR_CLOCK  clock_gptmr2
#define BOARD_BLDC_1MS_RELOAD 10000U//(100000U)


/**
 * @brief     timer init
 *            ;计时器
 */
void timer_init(void);

/** @} */

#endif /* ADC_DMA_H */
