/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef MP_PWM_H
#define MP_PWM_H

#include "mp_common.h"

/**
 * @brief HPM Power PWM interface;HPM电源PWM接口
 * @addtogroup POWER_PWM_API
 * @{
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the PWM MAP index number of a pair of PWM
 *        ;获取PWM对 MAP数组序号
 *
 * @param [in] pair_index A pair of PWM index number(an abstract MAP sequence number)     ;PWM对序号(MAP抽象序号)
 * @retval A pair of PWM MAP index number       ;PWM对 MAP数组序号
 */
uint8_t mp_pwm_pair_get_mapindex(uint8_t pair_index);

/**
 * @brief Get the PWM MAP index number of the PWM
 *        ;获取PWM MAP数组序号
 *
 * @param [in] index The PWM index number(an abstract MAP sequence number)    ;PWM序号(MAP抽象序号)
 * @retval The PWM MAP index number       ;PWM MAP数组序号
 */
uint8_t mp_pwm_get_mapindex(uint8_t index);

/**
 * @brief Get the bus frequency of a pair of PWM
 *        ;获取PWM对总线频率
 *
 * @param [in] pair_index A pair of PWM index number(an abstract MAP sequence number)      ;PWM对序号(MAP抽象序号)
 * @retval The bus frequency of a pair of PWM         ;PWM对总线频率
 */
uint32_t mp_pwm_pair_get_busfreq(uint8_t pair_index);

/**
 * @brief Get the bus frequency of the PWM
 *        ;获取PWM总线频率
 *
 * @param [in] index The PWM index number(an abstract MAP sequence number)      ;PWM序号(MAP抽象序号)
 * @retval The bus frequency of the PWM    ;PWM总线频率
 */
uint32_t mp_pwm_get_busfreq(uint8_t index);

/**
 * @brief Get the reload_count of a pair of PWM
 *        ;获取PWM对重载值reload_count
 *
 * @param [in] pair_index A pair of PWM index number(an abstract MAP sequence number)      ;PWM对序号(MAP抽象序号)
 * @param [in] freq The frequency of a pair of PWM        ;PWM对频率
 * @retval The reload_count of a pair of PWM      ;PWM对重载值reload_count
 */
uint32_t mp_pwm_pair_get_reload(uint8_t pair_index, uint32_t freq);

/**
 * @brief Get the reload_count of the PWM
 *       ;获取PWM重载值reload_count
 *
 * @param [in] index The PWM index number(an abstract MAP sequence number)     ;PWM序号(MAP抽象序号)
 * @param [in] freq The frequency of the PWM    ;PWM频率
 * @retval The reload_count of the PWM          ;PWM重载值reload_count
 */
uint32_t mp_pwm_get_reload(uint8_t index, uint32_t freq);

/**
 * @brief A pair of PWM init config     
 *        ;PWM对初始配置
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object   ;PWM对数据结构对象地址
 * @retval  ==0  initialization succeeded; !=0  initialization failed 
 *          ;==0 PWM对初始配置成功; !=0 PWM对初始配置失败;
 */
int mp_pwm_pair_config(hpm_pwm_pair_t* pwm_pair_t);

/**
 * @brief PWM init config
 *        ;PWM初始配置
 *
 * @param [in] pwm_t Address of the PWM data structure object  ;PWM数据结构对象地址
 * @retval ==0  initialization succeeded; !=0  initialization failed        ;==0 PWM初始配置成功; !=0 PWM初始配置失败;
 */
int mp_pwm_config(hpm_pwm_t* pwm_t);

/**
 * @brief Enable a pair of PWM waveform outputs
 *         ;PWM对波形输出使能
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object     ;PWM对数据结构对象地址
 * @param [in] enable true:enable; flase:disable;      ; true:使能; flase:禁止;
 * @retval ==0  configuration succeeded; !=0  configuration failed        ;==0 设置成功; !=0 设置失败;
 */
int mp_pwm_pair_output_enable(hpm_pwm_pair_t* pwm_pair_t, bool enable);

/**
 * @brief Enable the PWM waveform outputs
 *       ;PWM波形输出使能
 *
 * @param [in] pwm_t Address of the PWM data structure object    ;PWM数据结构对象地址
 * @param [in] enable true:enable; flase:disable;          ; true:使能; flase:禁止;
 * @retval ==0  configuration succeeded; !=0  configuration failed      ;==0 设置成功; !=0 设置失败;
 */
int mp_pwm_output_enable(hpm_pwm_t* pwm_t, bool enable);

/**
 * @brief The PWM counter enables counting
 *         ;PWM计数器使能计数
 *
 * @param [in] pwm_t Address of the PWM data structure object    ;PWM数据结构对象地址
 * @param [in] enable true:enable; flase:disable;           ; true:使能; flase:禁止;
 * @retval ==0  configuration succeeded; !=0  configuration failed       ;==0 设置成功; !=0 设置失败;
 */
int mp_pwm_enable_counter(hpm_pwm_t* pwm_t, bool enable);

/**
 * @brief PWM TRGM for fault protection
 *       ;PWM故障保护互联配置
 *
 * @param [in] type PWM type @ref mp_pwm_type       ;PWM类型 @ref mp_pwm_type
 * @param [in] handle Address of the PWM or a pair of PWM data structure object @ref hpm_pwm_t or @ref hpm_pwm_pair_t 
 *                   ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @retval ==0  configuration succeeded; !=0  configuration failed
 *         ;==0 配置成功; !=0 配置失败
 */
int mp_pwm_fault_mux_config(uint8_t type, void* handle);

/**
 * @brief PWM TRGM (trigger ADC sampling at a specific moment during PWM, etc)
 *        ;PWM互联触发配置(PWM某一时刻触发ADC采样等)
 *
 * @param [in] type PWM type @ref mp_pwm_type         ;PWM类型 @ref mp_pwm_type
 * @param [in] handle  Address of the PWM or a pair of PWM data structure object @ref hpm_pwm_t or @ref hpm_pwm_pair_t
 *                     ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @retval ==0  configuration succeeded; !=0  configuration failed
 *         ;==0 配置成功; !=0 配置失败
 */
int mp_pwm_trigger_mux_config(uint8_t type, void* handle);

/**
 * @brief Get the PWM TRGM for MAP table
 *        ;获取PWM互联触发MAP表
 *
 * @param [in] type PWM type @ref mp_pwm_type         ;PWM类型 @ref mp_pwm_type
 * @param [in] index A pair of PWM or PWM index number(an abstract MAP sequence number)
 *                   ;PWM序号或PWM对序号(MAP抽象序号)
 * @return MAP table address      ;返回PWM互联触发MAP表地址
 */
const mp_pwm_trigger_map_t* mp_pwm_get_trigger_map(uint8_t type, uint8_t index);

/**
 * @brief PWM synchronous configuration
 *        Note: PWM synchronization only supports different PWM controllers
 *        ;PWM同步配置
 *        注意: PWM同步只支持不同的PWM控制器
 *
 * @param [in] mp_t Address of the POWER MP data structure object    ;电源MP数据结构对象地址
 * @retval ==0  configuration succeeded; !=0  configuration failed
 *         ;==0 配置成功; !=0 配置失败
 */
int mp_pwm_sync_config(hpm_mp_t* mp_t);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__MP_PWM_H