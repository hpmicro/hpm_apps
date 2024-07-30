/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef ADC16_H
#define ADC16_H



/**
 * @brief HPM PWM ADC SPI sample API ;HPM PWM SPI例程接口
 * @addtogroup PWM_ADC_SPI_API 
 * 
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/
#define API_VERSION         "V0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ADC common configuration 
 * ;初始化ADC通用配置
 *
 * @param [in] conv_mode ADC conversion mode selection, supports the following modes: 
 * ;ADC 转换模式选择，支持如下模式： 
 * - adc16_conv_mode_oneshot: oneshot conversion mode  ;读取转换模式
 * - adc16_conv_mode_period:  period conversion mode   ;周期转换模式
 * - adc16_conv_mode_sequence: sequence conversion mode  ;读取转换模式
 * - adc16_conv_mode_preemption: preemption conversion mode  ;抢占转换模式 
 * @return status_success If ADC configuration is successful 
 * 如果ADC配置成功，返回status_success
 */
hpm_stat_t init_common_config(adc16_conversion_mode_t conv_mode);


/**
 * @brief  Initialize preemption conversion mode.Including trigger source initialization, 
 * trigger target initialization, TRGM initialization, and DMA initialization
 * ;ADC抢占转换模式初始化，包括触发源初始化、触发目标初始化、互联管理器初始化和DMA初始化
 */
void init_preemption_config(void);


/**
 * @brief Initialize TRGM 
 * ;初始化互联管理器
 *
 * @param [in] ptr ADC TRGM TRGM
 * ;使用的TRGM
 */
void init_trigger_mux(TRGM_Type *ptr);


/**
 
 * @brief Initialize trigger source 
 * ;初始化触发源PWM
 *
 * @param [in] ptr PWM selected ;使用的PWM
 *
 */
void init_trigger_source(PWM_Type *ptr);



/**
 * @brief Initialize trigger target
 * ;初始化触发目标
 *
 * @param [in] ptr ADC selected ;使用的ADC
 * @param [in] trig_ch Trigger ADC channel selection ;触发的ADC通道选择
 * @param [in] inten Interrupt enable settings ;中断使能设置
 *
 */
void init_trigger_target(ADC16_Type *ptr, uint8_t trig_ch, bool inten);



/**
 * @brief  set SPI sclk frequency, format config, control config for master
 * ;配置成主模式传输，SPI时钟频率，数据格式，传输控制，控制配置
 * 
 */
void spi_init(void);




/**
 * @brief  HDMA chain transmission configuration;HDMA链式传输配置
 * task 1:SPI->TRANSCTRL configuration; ;SPI数据格式配置
 * task 2:SPI->CMD configuration;SPI命令寄存器配置
 * task 3:SPI->DATA write;SPI数据寄存器写入
 * task 4:dummy buff copy;空字节buff拷贝
 * task 5:SPI->DATA read;SPI->DATA读取
 * Each task is connected through a linked list adc_descriptors1 and executed in 
 * a sequential loop ;各任务通过链表adc_descriptors1连接起来,按照顺序循环执行
 * 
 */
void hdma_spi_tx_chain_config(void);






/**
 * @}
 *
 */

#ifdef __cplusplus
}
#endif

#endif //MP_API_H