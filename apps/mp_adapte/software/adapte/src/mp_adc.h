/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MP_ADC_H
#define MP_ADC_H

#include "mp_common.h"

/**
 * @brief HPM Power ADC interface; HPM电源ADC接口
 * @addtogroup POWER_ADC_API
 * @{
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get ADC MAP index 
 *        ;获取ADC MAP数组序号
 *
 * @param [in] adc_index ADC index number(an abstract MAP sequence number)     ;ADC序号(MAP抽象序号)
 * @retval ADC MAP index number    ;ADC MAP数组序号
 */
uint8_t mp_adc_get_mapindex(uint8_t adc_index);

/**
 * @brief Get the default value of the ADC data structure
 *        ;获取ADC数据结构默认值
 *
 * @param [in] adc_index ADC index number(an abstract MAP sequence number)     ;ADC序号(MAP抽象序号)
 * @param [out] adc_t Address of the ADC data structure object     ;ADC数据结构对象地址
 */
void mp_adc_get_default(uint8_t adc_index, hpm_adc_t* adc_t);

/**
 * @brief MP ADC init
 *        ;MP ADC初始化
 *
 * @param [in] type PWM type @ref mp_pwm_type        ;PWM类型 @ref mp_pwm_type
 * @param [in] handle Address of the PWM or a pair of PWM data structure object @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 *                    ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @retval ==0  initialization succeeded; !=0  initialization failed
 *         ;==0 初始化成功; !=0 初始化失败
 */
int mp_adc_init(uint8_t type, void* handle);

/**
 * @brief MP ADC trigger the preemption sampling configuration
 *        ;MP ADC触发抢占采样配置
 *
 * @param [in] type PWM type @ref mp_pwm_type       ;PWM类型 @ref mp_pwm_type
 * @param [in] handle Address of the PWM or a pair of PWM data structure object @ref hpm_pwm_t or @ref hpm_pwm_pair_t
 *                    ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @retval ==0  configuration succeeded; !=0  configuration failed
 *        ;==0 配置成功; !=0 配置失败
 */
int mp_adc_trigger_config(uint8_t type, void* handle);

/**
 * @brief MP ADC enable interrupt
 *        ;MP ADC中断使能
 * 
 * @param [in] type PWM type @ref mp_pwm_type       ;PWM类型 @ref mp_pwm_type
 * @param [in] handle Address of the PWM or a pair of PWM data structure object @ref hpm_pwm_t or @ref hpm_pwm_pair_t
 *                    ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @param [in] enable true:enable; false:disable;         ; true:使能; false:禁止;
 * @retval ==0  configuration succeeded; !=0  configuration failed
 *        ;==0 配置成功; !=0 配置失败
 */
int mp_adc_interrupt_enable(uint8_t type, void* handle, bool enable);

/**
 * @brief  MP HDMA init(HDMA is used to transmit ADC sampling values. It can be used to continuously sample multiple adc values, and HDMA automatically carries the sampling results to the specified memory area)
 *         Note: After configuration, @ref mp_hdma_transfer_start  needs to be triggered to initiate the transfer
 *        ;MP HDMA初始配置(HDMA搬用ADC采样值; 常用于连续采样多个adc值，HDMA自动搬用到指定内存区域)
 *        注意:配置完后需要 @ref mp_hdma_transfer_start 触发开始传输
 *       
 * @param [in] type PWM type @ref mp_pwm_type      ;PWM类型 @ref mp_pwm_type
 * @param [in] handle  Address of the PWM or a pair of PWM data structure object @ref hpm_pwm_t or @ref hpm_pwm_pair_t
 *                    ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @param [in] cb HDMA transmission done callback      ;HDMA传输结束回调
 * @retval ==0  configuration succeeded; !=0  configuration failed      ;==0 配置成功; !=0 配置失败
 */
int mp_hdma_init_config(uint8_t type, void* handle, hpm_mp_hdma_over_callback cb);

/**
 * @brief MP HDMA trigger and start transmission       ;MP HDMA触发开始传输
 *
 * @param [in] size Number of bytes transmitted by HDMA     ;HDMA传输的字节数
 * @retval  ==0  transmission  succeeded; !=0  transmission  failed       ;==0 触发传输成功; !=0 触发传输失败;
 */
int mp_hdma_transfer_start(uint32_t size);

/**
 * @brief  MP HDMA chain transmission config, double buffered mode(In HDMA chain mode, multiple tasks are configured. In dual-buffer mode, the PING/PONG dual-buffer switching task is automatically performed without CPU intervention)
 *        ;MP HDMA链式传输配置，双缓冲模式(HDMA链式配置多任务，双缓冲模式下，自动PING/PONG双缓冲切换搬用，无需CPU干预)
 *
 * @param [in] size Number of bytes transmitted by HDMA     ;HDMA传输的字节数
 * @retval ==0  chain transmission  succeeded; !=0  chain transmission  failed       ;==0 链式传输配置成功; !=0 链式传输配置失败;
 */
//MP hdma链式传输配置，双缓冲模式
int mp_dma_chained_transfer_config(uint32_t size);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //MP_ADC_H