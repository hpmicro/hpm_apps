/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef ADC_16_PMT_H
#define ADC_16_PMT_H

#include "hpm_adc16_drv.h"
#include "hpm_pwm_drv.h"

/**
 * @brief LWIP_ADC 接口
 * @addtogroup LWIP_ADC 接口
 * @{
 *
 */
 
/**
 * @brief APP_ADC_TRIG_FREQ:ADC sampling frequency.
 *        ;ADC采样频率
 */
#define APP_ADC_TRIG_FREQ                 (2000000U)
/**
 * @brief APP_ADC16_DMA_BUFF_LEN_IN_BYTES:DMA BUFF size for storing ADC16 sampling data.
 *        ;存放ADC16采样数据的DMA BUFF大小
 */
#define APP_ADC16_DMA_BUFF_LEN_IN_BYTES   (1024U)
/**
 * @brief APP_ADC16_TCP_SEND_SIZE:The number of 16 bit sampling data sent by TCP each time.
 *        ;TCP每次发送的16位采样数据的个数。
 */
#define APP_ADC16_TCP_SEND_SIZE           (APP_ADC16_DMA_BUFF_LEN_IN_BYTES*sizeof(uint16_t))

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/**
 * @brief HDMA configuration: Configure HDMA chain transmission, including the data width, 
 * amount of data transmitted each time, and DMA transmission mode.
 *        ;HDMA配置：配置HDMA链式传输，配置HDMA传输的数据宽度、每次传输的数据量、DMA传输的模式等
 */
void hdma_auto_config(void);
/**
 * @brief HDMA chain transmission configuration.task 1:ADC sampling data is 
 * stored in the first half of the adc_buff;task 2:adc_done[0] set 1;
 * task 3:ADC sampling data is stored in the second half of the adc_buff;
 * tasj 4:adc_done[1] set 1;
 * Each task is connected through a linked list adc_descriptors1 and executed in 
 * a sequential loop. 
 *       ;HDMA链式传输配置。任务1：ADC采样数据存放至adc_buff前半部分；任务2：adc_done[0]置1；
 * 任务3：ADC采样数据存放至adc_buff前后半部分；任务4：adc_done[1]置1；
 * 各任务通过链表adc_descriptors1连接起来,按照顺序循环执行。

 */
void hdma_dma_chain_config(void);
/**
 * @brief HDMA start transfer.enable HDMA and start transmitting data.
 *        ;HDMA开始传输：使能HDMA并开始传输数据。
 */
int hdma_transfer_start(void);
/**
 * @brief Initialize trigger source.
 *        ;初始化触发源PWM.
 *
 * @param [in] ptr PWM used by ADC     ;ADC使用的PWM。
 * @param [in] sample_freq ADC sampling frequency   ;ADC采样率。
 */
void init_trigger_source(PWM_Type *ptr, uint32_t sample_freq);
/**
 * @brief Initialize TRGM.
 *        ;初始化互联管理器。
 *
 * @param [in] ptr TRGM used by ADC ;ADC使用的TRGM。
 */
void init_trigger_mux(TRGM_Type *ptr);
/**
 * @brief Initialize trigger target.
 *        ;初始化触发目标。
 * 
 * @param [in] ptr ADC used ;使用的ADC。
 * @param [in] trig_ch Trigger ADC channel selection  ;触发的ADC通道选择。
 * @param [in] inten Interrupt enable settings  ;中断使能设置。
 */
void init_trigger_target(ADC16_Type *ptr, uint8_t trig_ch, bool inten);
/**
 * @brief Initialize ADC common configuration.
 *        ;初始化ADC通用配置。
 * 
 * @param [in] conv_mode ADC conversion mode  ;ADC转换模式选择，支持如下模式。  
 * selection, supports the following modes: 
 * - adc16_conv_mode_oneshot: oneshot conversion mode ;读取转换模式
 * - adc16_conv_mode_period:  period conversion mode  ;周期转换模式
 * - adc16_conv_mode_sequence: sequence conversion mode ;读取转换模式
 * - adc16_conv_mode_preemption: preemption conversion mode ;读取抢占转换模式 
 * @return status_success If ADC configuration is successful ;如果ADC配置成功，返回status_success
 */
hpm_stat_t init_common_config(adc16_conversion_mode_t conv_mode);
/**
 * @brief Initialize preemption conversion mode.Including trigger source initialization,
 * trigger target initialization, TRGM initialization, and DMA initialization
 *        ;ADC抢占转换模式初始化，包括触发源初始化、触发目标初始化、互联管理器初始化和DMA初始化。
 */
void init_preemption_config(void);
/**
 * @brief ADC preemption conversion mode configuration, including ADC pin initialization,
 * ADC clock initialization, ADC common configuration, and ADC preemption conversion 
 * mode initialization
 *        ;ADC抢占转换模式配置，包括ADC引脚初始化、ADC时钟初始化、ADC通用配置和ADC抢占转换模式初始化。
 */
void adc_pmt_auto_config(void);
/**
 * @brief adc_done flag clear: when adc_done flag is cleared, corresponding 
 * to the adc_buff data will be sent out via Ethernet
 *        ;adc_done标志位清除：当adc_done标志位被清除，对应的adc_buff数据将通过以太网发送出去。
 * 
 * @param [in] index index of adc_done flag.  ;adc_done标志位索引
 * - 0: corresponding data in the first half of the adc_buff. ;对应adc_buff前半部分的数据。
 * - 1: corresponding data in the second half of the adc_buff. ;对应adc_buff后半部分的数据
 */
void adc_clear_done(uint8_t index);
/**
 * @brief adc_done flag clear: when adc_done flag is 1, corresponding 
 * to the adc_buff data will be sent out via Ethernet.
 *        ;获取adc_done标志位：当adc_done标志位为1，对应的adc_buff数据将通过以太网发送出去。
 * 
 * @param [in] index adc_done flag index. ;adc_done标志位索引。
 * - 0: corresponding data in the first half of the adc_buff. ;对应adc_buff前半部分的数据
 * - 1: corresponding data in the second half of the adc_buff. ;对应adc_buff后半部分的数据 
 * @return return the value of the adc_done flag corresponding to the index.  ;返回对应索引的adc_done标志位的值
 */
uint8_t adc_get_done(uint8_t index);
/**
 * @brief get the starting address for sending data.
 *        ;获取发送数据的起始地址
 * 
 * @param [in] index adc_done flag index. ;adc_done标志位索引。
 * - 0: corresponding data in the first half of the adc_buff. ;对应adc_buff前半部分的数据
 * - 1: corresponding data in the second half of the adc_buff. ;对应adc_buff后半部分的数据 
 * @return return the starting address of the corresponding index for sending data.
 *         ;返回对应索引的发送数据的起始地址。
 */
uint8_t *adc_get_buf_addr_point(uint8_t index);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** @} */

#endif /* TCP_ECHO_H */
