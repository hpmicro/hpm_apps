/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MP_API_H
#define MP_API_H

#include "mp_common.h"

/**
 * @brief HPM Power interface;HPM电源接口
 * @addtogroup POWER_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/
#define MP_VERSION         "V0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the default value of the power data structure
 * ;电源数据结构默认值获取
 *
 * @param [out] mp_t Address of the Power data structure object, used for returning default values.
 *                   ;电源数据结构对象地址，用于返回默认值
 * @retval ==0 Successfully getting the default values of the power data structure; !=0 Failed to get the default value of the power data structure;
 *         ;==0 获取电源数据结构默认值成功; !=0 获取电源数据结构默认值错误;
 */
int hpm_mp_api_get_default(hpm_mp_t* mp_t);

/**
 * @brief Get the default value of a pair of PWM (two PWM channels as a pair) data structure  
 * ;获取PWM对(两路PWM成一对)数据结构默认值
 *
 * @param [in] pwm_pair_index a pair of PWM index number(an abstract MAP sequence number)
 *                            ;PWM对序号index(该序号为抽象的MAP序号)
 * @param [in] freq  a pair of PWM frequency  ;PWM对频率
 * @param [out] pwm_pair_t Address of a pair of PWM data structure object, used for returning default values.
 *                         ;PWM对数据结构对象地址，用于返回默认值
 * @retval ==0 Successfully getting the default values of a pair of PWM data structure; !=0 Failed to get the default value of a pair of PWM data structure;
 *         ;==0 获取PWM对数据结构默认值成功; !=0 获取PWM对数据结构默认值错误;
 */
int hpm_mp_pwm_pair_get_default(uint8_t pwm_pair_index, uint32_t freq, hpm_pwm_pair_t* pwm_pair_t);

/**
 * @brief Get the default value of the single PWM data structure
 * ;获取PWM(单路)数据结构默认值
 *
 * @param [in] pwm_index PWM index number(an abstract MAP sequence number)  ;PWM序号index(该序号为抽象的MAP序号)
 * @param [in] freq PWM frequency    ;PWM频率
 * @param [out] pwm_t Address of the PWM data structure object, used for returning default values    ;PWM数据结构对象地址，用于返回默认值
 * @retval  ==0 Successfully getting the default values of the PWM data structure; !=0 Failed to get the default value of the PWM data structure;
 *          ;==0 获取PWM数据结构默认值成功; !=0 获取PWM数据结构默认值错误;
 */
int hpm_mp_pwm_get_default(uint8_t pwm_index, uint32_t freq, hpm_pwm_t* pwm_t);

/**
 * @brief Get the default value of the ADC data structure
 * ;获取ADC数据结构默认值
 *
 * @param [in] adc_index ADC index number(an abstract MAP sequence number)      ;ADC序号index(该序号为抽象的MAP序号)
 * @param [out] adc_t Address of the ADC data structure object, used for returning default values     ;ADC数据结构对象地址，用于返回默认值
 * @retval ==0 Successfully getting the default values of the ADC data structure; !=0 Failed to get the default value of the ADC data structure;
 *         ;==获取ADC数据结构默认值成功; !=0 获取ADC数据结构默认值错误;
 */
int hpm_mp_adc_get_default(uint8_t adc_index, hpm_adc_t* adc_t);

/**
 * @brief Power API initialization
 * ;电源API初始化
 *
 * @param [in] mp_t Address of the Power data structure object   ;电源数据结构对象地址
 * @retval ==0 Power API initialized successfully; !=0 Power API initialized failed;
 *         ;==0 电源API初始化成功; !=0 电源API初始化失败;
 */
int hpm_mp_api_init(hpm_mp_t* mp_t);

/**
 * @brief A pair of PWM gets the overloaded value 'reload_count' based on frequency
 * ;PWM对根据频率获取重载值reload_count
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object  ;PWM对数据结构对象地址
 * @param [in] freq a pair of PWM frequency  ;PWM对频率
 * @retval A pair of PWM gets the overloaded value 'reload_count' based on frequency  ;PWM对根据频率获取的重载值reload_count
 */
uint32_t hpm_mp_api_pwmpair_get_reload_of_freq(hpm_pwm_pair_t* pwm_pair_t, uint32_t freq);

/**
 * @brief A pair of PWM, configured frequency and duty cycle
 *        Note: Get reload_count by frequency first,by using @ref hpm_mp_api_pwmpair_get_reload_of_freq 
 *        ;PWM对同时设置频率和占空比;
 *        注意: 先通过频率获取reload_count，调用 @ref hpm_mp_api_pwmpair_get_reload_of_freq 接口获取
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object  ;PWM对数据结构对象地址
 * @param [in] reload_count The reload_count of a pair of PWM   ;PWM对重载值reload_count
 * @param [in] duty_count duty_count(0..reload_count)    ;占空比值(0..reload_count)
 * @retval ==0  configuration succeeded; !=0  configuration failed    ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwmpair_set_reload_and_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t reload_count, uint32_t duty_count);

/**
 * @brief A pair of PWM frequency Settings;
 *        Note: After setting the frequency, the duty cycle automatically converts to the original percentage. For example, with a 20% duty cycle, after setting the frequency, the duty cycle remains at 20%
 *        ;PWM对频率设置;
 *        注意: 设置频率后，占空比自动转换为原始百分比。如:20%占空比，设置频率后，占空比仍然为20%
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object ;PWM对数据结构对象地址
 * @param [in] freq a pair of PWM frequency ;PWM对频率
 * @retval ==0  configuration succeeded; !=0  configuration failed    ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwmpair_freq(hpm_pwm_pair_t* pwm_pair_t, uint32_t freq);

/**
 * @brief Setting the frequency for a pair of PWM and configuring a 50% duty cycle
 *        Note: After setting the frequency, the duty cycle is simultaneously set to 50%
 *        ;PWM对频率设置且固定半占空比
 *        注意: 设置频率后，占空比同时设置为50%
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object ;PWM对数据结构对象地址
 * @param [in] freq a pair of PWM frequency ;PWM对频率
 * @retval ==0  configuration succeeded; !=0  configuration failed  ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwmpair_freq_of_half_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t freq);

/**
 * @brief A pair of PWM duty cycle Settings
 *        Note: The duty cycle ranges from 0 to reload_count(reload_count is a member of pwm_pair_t)
 *        ;PWM对占空比设置
 *        注意: 占空比范围为0~reload_count(reload_count为pwm_pair_t成员)
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object  ;PWM对数据结构对象地址
 * @param [in] duty Duty cycle(0..reload_count)    ;占空比值(0..reload_count)
 * @retval  ==0  configuration succeeded; !=0  configuration failed      ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwmpair_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t duty);

/**
 * @brief Setting forced output for a pair of PWM  (setting a pair of PWM signals to high or low level)
 *        Note: This interface is for configuration only and does not enable the active state. Enable or disable the following interfaces separately
 *        Enable (force output to be high or low level) @ref hpm_mp_api_pwmpair_force_enable_output
 *        Disable (Restore PWM waveform) @ref hpm_mp_api_pwmpair_force_disable_output
 *        ;PWM对强制输出设置(将PWM对强制设置为高电平或低电平)
 *        注意: 此接口只是配置，并不会使能生效状态，使能或禁止需单独调用以下接口
 *        使能生效(强制输出为高或低) @ref hpm_mp_api_pwmpair_force_enable_output
 *        禁止生效(恢复PWM波形)  @ref hpm_mp_api_pwmpair_force_disable_output
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object ;PWM对数据结构对象地址
 * @param [in] index One channel of the PWM pairs(two channels = one pair)     ;PWM对某一路(PWM对两路为一对)
 *  @arg 0: First channel   ;PWM对第一路
 *  @arg 1: Second channel  ;PWM对第二路
 * @param [in] level level  ;电平
 *  @arg 0: low level      ;低电平
 *  @arg 1: high level     ;高电平
 * @retval  ==0  configuration succeeded; !=0  configuration failed     ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwmpair_force_config(hpm_pwm_pair_t* pwm_pair_t, uint8_t index, uint8_t level);

/**
 * @brief Enable a pair of PWM signals forced outputs (force output to be high or low level)
 *  ;PWM对强制输出使能生效(强制输出为高或低)
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object  ;PWM对数据结构对象地址
 * @param [in] index One channel of the PWM pairs(two channels = one pair)   ;PWM对某一路(PWM对两路为一对)
 *  @arg 0: First channel   ;PWM对第一路
 *  @arg 1: Second channel  ;PWM对第二路
 */
void hpm_mp_api_pwmpair_force_enable_output(hpm_pwm_pair_t* pwm_pair_t, uint8_t index);

/**
 * @brief Disable a pair of PWM signals forced outputs(Restore PWM waveform)      ;PWM对强制输出禁止生效(恢复PWM波形)
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object   ;PWM对数据结构对象地址
 * @param [in] index One channel of the PWM pairs(two channels = one pair)     ;PWM对某一路(PWM对两路为一对)
 *  @arg 0: First channel   ;PWM对第一路
 *  @arg 1: Second channel  ;PWM对第二路
 */
void hpm_mp_api_pwmpair_force_disable_output(hpm_pwm_pair_t* pwm_pair_t, uint8_t index);

/**
 * @brief A pair of PWM fault states are cleared by software 
 *        Note: When the fault recovery is set to software recovery, use this interface to clear the fault status after confirming fault clearance.
 *        ;PWM对故障状态软件清除
 *        注意: 当故障恢复设置为软件恢复后，在确认故障清除后，使用此接口清除故障状态
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object    ;PWM对数据结构对象地址
 */
void hpm_mp_api_pwmpair_fault_software_clear(hpm_pwm_pair_t* pwm_pair_t);

/**
 * @brief PWM frequency Settings;
 *        Note: After setting the frequency, the duty cycle automatically converts to the original percentage. For example, with a 20% duty cycle, after setting the frequency, the duty cycle remains at 20%
 *        ;PWM频率设置;
 *        注意: 设置频率后，占空比自动转换为原始百分比。如:20%占空比，设置频率后，占空比仍然为20%
 *
 * @param [in] pwm_t Address of the PWM data structure object ;PWM数据结构对象地址
 * @param [in] freq  PWM frequency       ;PWM频率
 * @retval ==0  configuration succeeded; !=0  configuration failed      ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_freq(hpm_pwm_t* pwm_t, uint32_t freq);

/**
 * @brief Setting the frequency for the PWM and configuring a 50% duty cycle
 *        Note: After setting the frequency, the duty cycle is simultaneously set to 50%
 *        ;PWM频率设置且固定半占空比
 *        注意: 设置频率后，占空比同时设置为50%
 *
 * @param [in] pwm_t Address of the PWM data structure object   ;PWM数据结构对象地址
 * @param [in] freq  PWM frequency       ;PWM频率
 * @retval ==0  configuration succeeded; !=0  configuration failed       ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_freq_of_half_duty(hpm_pwm_t* pwm_t, uint32_t freq);

/**
 * @brief The PWM duty cycle Settings
 *        Note: The duty cycle ranges from 0 to reload_count(reload_count is a member of pwm_pair_t)
 *        ;PWM占空比设置
 *        注意: 占空比范围为0~reload_count(reload_count为pwm_t成员)
 *
 * @param [in] pwm_t Address of the PWM data structure object  ;PWM数据结构对象地址
 * @param [in] duty Duty cycle(0..reload_count)    ;占空比值(0..reload_count)
 * @retval ==0  configuration succeeded; !=0  configuration failed     ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_duty(hpm_pwm_t* pwm_t, uint32_t duty);

/**
 * @brief Setting forced output for the PWM  (setting the PWM signal to high or low level)
 *        Note: This interface is for configuration only and does not enable the active state. Enable or disable the following interfaces separately
 *        Enable (force output to be high or low level) @ref hpm_mp_api_pwm_force_enable_output
 *        Disable (Restore PWM waveform) @ref hpm_mp_api_pwm_force_disable_output
 *        ;PWM强制输出设置(将PWM强制设置为高电平或低电平)
 *        注意: 此接口只是配置，并不会使能生效状态，使能或禁止需单独调用以下接口
 *        使能生效(强制输出为高或低) @ref hpm_mp_api_pwm_force_enable_output
 *        禁止生效(恢复PWM波形)  @ref hpm_mp_api_pwm_force_disable_output
 *
 * @param [in] pwm_t Address of the PWM data structure object  ;PWM数据结构对象地址
 * @param [in] level level  ;电平
 *  @arg 0: low level   ;低电平
 *  @arg 1: high level  ;高电平
 * @retval ==0  configuration succeeded; !=0  configuration failed      ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_force_config(hpm_pwm_t* pwm_t, uint8_t level);

/**
 * @brief  Enable the PWM signal forced outputs (force output to be high or low level)
 *      ;PWM强制输出使能生效(强制输出为高或低)
 *
 * @param [in] pwm_t Address of the PWM data structure   ;PWM数据结构地址
 */
void hpm_mp_api_pwm_force_enable_output(hpm_pwm_t* pwm_t);

/**
 * @brief Disable the PWM signal forced outputs(Restore PWM waveform)  
 *    ;PWM强制输出禁止生效(恢复PWM波形)
 *
 * @param [in] pwm_t Address of the PWM data structure object  ;PWM数据结构对象地址
 */
void hpm_mp_api_pwm_force_disable_output(hpm_pwm_t* pwm_t);

/**
 * @brief The PWM fault states are cleared by software 
 *        Note: When the fault recovery is set to software recovery, use this interface to clear the fault status after confirming fault clearance.
 *        ;PWM故障状态软件清除
 *        注意: 当故障恢复设置为软件恢复后，在确认故障清除后，使用此接口清除故障状态
 *
 * @param [in] pwm_t Address of the PWM data structure object  ;PWM数据结构对象地址
 */
void hpm_mp_api_pwm_fault_software_clear(hpm_pwm_t* pwm_t);

/**
 * @brief Setting the duty cycle for a pair of PWM signals that trigger ADC sampling moments
 *        Note: Duty cycle ranges from 0 to reload_count(pwm_t)
 *        ;PWM对触发ADC采样时刻占空比设置
 *        注意: 占空比范围为0~reload_count(pwm_pair_t)
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object   ;PWM对数据结构对象地址
 * @param [in] duty duty cycle(0..reload_count)      ;占空比值(0..reload_count)
 * @retval ==0  configuration succeeded; !=0  configuration failed     ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_pair_trig_set_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t duty);

/**
 * @brief Setting the duty cycle for the PWM signal that trigger ADC sampling moments
 *        Note: Duty cycle ranges from 0 to reload_count(pwm_t)
 *        ;PWM 触发ADC采样时刻占空比设置
 *        注意: 占空比范围为0~reload_count(pwm_t)
 *
 * @param [in] pwm_t PWM Address of the PWM data structure object   ;PWM数据结构对象地址
 * @param [in] duty duty cycle(0..reload_count)   ;占空比值(0..reload_count)
 * @retval ==0  configuration succeeded; !=0  configuration failed    ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_trig_set_duty(hpm_pwm_t* pwm_t, uint32_t duty);

/**
 * @brief A pair of PWM dead zone Settings
 *        ;PWM对前后死区设置
 *
 * @param [in] pwm_pair_t Address of a pair of PWM data structure object   ;PWM对数据结构对象地址
 * @param [in] start_halfcycle Starting time for deadarea. The unit is half of the PWM bus frequency
 *                             ;起始死区时间，单位为PWM总线频率的一半
 * @param [in] end_halfcycle Ending time for deadarea. The unit is half of the PWM bus frequency
 *                           ;结束死区时间, 单位为PWM总线频率的一半
 * @retval ==0  configuration succeeded; !=0  configuration failed   ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_pair_set_deadzone(hpm_pwm_pair_t* pwm_pair_t, uint32_t start_halfcycle, uint32_t end_halfcycle);

/**
 * @brief PWM DMA enable 
 *        ;PWM DMA请求使能
 *
 * @param [in] enable true:enable; flase:disable;        ;true:使能; flase:禁止;
 * @param [in] type PWM type@ref mp_pwm_type      ;PWM类型 @ref mp_pwm_type
 * @param [in] handle Address of a pair of PWM or single PWM data structure object @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t 
 *                   ;PWM 或 PWM对数据结构对象地址 @ref hpm_pwm_t 或 @ref hpm_pwm_pair_t
 * @retval ==0  configuration succeeded; !=0  configuration failed
 *         ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_dma_request_enable(bool enable, uint8_t type, void* handle);

/**
 * @brief High precision timer creation
 *        ;高精定时器创建
 *
 * @param [in] us Timer time(unit:us)   ;定时器时刻(单位:微妙)
 * @param [in] cb Timer callback        ;定时器响应回调callback
 * @retval ==0  configuration succeeded; !=0  configuration failed    ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_timer_create(uint32_t us, mp_timer_cb cb);

/**
 * @brief  Settings for a pair of PWM MAP
 *         ;PWM对 MAP设置
 *
 * @param [in] map_t Address of a pair of PWM MAP data structure object  ;PWM对 MAP数据结构对象地址
 * @param [in] count Count for a pair of PWM MAP data              ;PWM对MAP数组总和
 * @retval ==0  configuration succeeded; !=0  configuration failed    ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_pair_set_map(const mp_pwm_pair_map_t* map_t, int count);

/**
 * @brief Settings for the PWM MAP
 *        ;PWM MAP设置
 *
 * @param [in] map_t Address of the PWM MAP data structure object ;PWM MAP数据结构对象地址
 * @param [in] count Count for the PWM MAP data      ;PWM MAP数组总和
 * @retval ==0  configuration succeeded; !=0  configuration failed     ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_pwm_set_map(const mp_pwm_map_t* map_t, int count);

/**
 * @brief Settings for the ADC MAP
 *        ;ADC MAP设置
 *
 * @param [in] map_t Address of the PWM MAP data structure object  ;ADC MAP数据结构对象地址
 * @param [in] count Count for ADC MAP data   ;ADC MAP数组总和
 * @retval ==0  configuration succeeded; !=0  configuration failed    ;==0 设置成功; !=0 设置失败
 */
int hpm_mp_api_adc_set_map(const mp_adc_map_t* map_t, int count);

/**
 * @brief A pair of PWM IO init 
 *        ;PWM 对 IO初始化
 *
 * @param [in] pair_index a pair of PWM index number(an abstract MAP sequence number)      ;PWM 对序号(MAP抽象序号)
 */
void hpm_mp_api_pair_pwm_io_init(uint8_t pair_index);

/**
 * @brief PWM IO init 
 *        ;PWM IO初始化
 *
 * @param [in] index The PWM index number(an abstract MAP sequence number)   ;PWM 序号(MAP抽象序号)
 */
void hpm_mp_api_pwm_io_init(uint8_t index);

/**
 * @brief ADC IO init
 *        ;ADC IO初始化
 *
 * @param [in] adc_index ADC index number(an abstract MAP sequence number)    ;ADC 序号(MAP抽象序号)
 */
void hpm_mp_api_adc_io_init(uint8_t adc_index);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //MP_API_H