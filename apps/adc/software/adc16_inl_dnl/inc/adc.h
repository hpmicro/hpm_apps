/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef ADC_H
#define ADC_H

/**
 * @brief ADC setting API;ADC设置接口
 * @defgroup ADC_API
 * @ingroup ADC_API 
 * @{
 *
 */
 

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "hpm_pwm_drv.h"

/**
 * @brief Initialize ADC;初始化ADC
 */
void adc_init(void);

/**
 * @brief Setting ADC sampling trigger by PWM hardware;ADC采样触发设置,硬件PWM触发
 *
 * @param [in] ptr PWM base address;触发PWM基地址
 * @param [in] sample_freq PWM trigger frequency for sampling in KHz;PWM触发采样频率，以KHz为单位
 */
void trigger_init(PWM_Type * ptr,uint32_t sample_freq);

/**
 * @brief Startup ADC sampling trigger source;启动ADC触发源
 *
 * @param [in] ptr PWM base address;触发PWM基地址
 */
void trigger_start(PWM_Type * ptr);

/**
 * @brief Get ADC DMA buffer the base address;获取ADC DMA Buffer首地址
 *
 * @retval ADC DMA buffer base address;ADC DMA buffer首地址
 */
uint32_t* adc_dma_buffer_get(void);



#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** @} */

#endif  /* ADC_H */