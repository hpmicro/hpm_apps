/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef PWM_DMA_H
#define PWM_DMA_H

#include "board.h"

/**
 * @brief PWM_DMA_API ;PMW_SIN生成
 * @addtogroup PWM_SIN_API
 * @{
 *
 */

#define PWM_RDC                   HPM_PWM0
#define PWM_CLOCK_NAME            clock_mot0
#define PWM_OUTPUT_PIN1           4//PA26
#define PWM_OUTPUT_PIN2           5//PA27
#define PWM_RDC_TRGM              HPM_TRGM0
#define TRGM_PWM_OUTPUT           TRGM_TRGOCFG_PWM0_SYNCI
#define PWM_RDC_DMA_SRC           HPM_TRGM0_DMA_SRC_PWM0_CMP10
#define PWM_RDC_TRGM_DMA          TRGM_DMACFG_1
#define PWM_RDC_DMA_SRC_TRIG      HPM_DMA_SRC_MOT_1
#define PWM_RDC_DMA_CHNNEL        2
#define PWM_RDC_CMP_INDEX         0
#define PWM_FREQ                  (1000000u)
#define SIN_DATA_LEN              101

/**
 * @brief     config dma carries the sin table to update the pwm comparison value to generate the sin waveform
 *            ;配置dma搬运sin_table更新pwm比较值生成sin波形
 */
void init_pwm_out_rdc(void);

/**
 * @brief     start pwm
 *            ;pwm开启
 */
void start_pwm_rdc(void);

/**
 * @brief     config DAC for 1.65v DC bias
 *            ;dac配置
 */
void DAC_cfg(void);

/**
 * @brief     board init in rtt mode
 *            ;rtt模式下板级初始化
 */
void board_init_rtt_test(void);

/** @} */

#endif  
