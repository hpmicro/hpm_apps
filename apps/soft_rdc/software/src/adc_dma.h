/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef ADC_DMA_H
#define ADC_DMA_H

/**
 * @brief ADC_DMA_API ;DMA_搬运ADC
 * @addtogroup ADC_DMA_API
 * @{
 *
 */

#define ADC_BITS_16                     1
#define ADC_CORE                       HPM_CORE0
#define ADC_CH_SAMPLE_CYCLE            (20U)
#define ADC_TRIG_SRC_FREQUENCY         (1000000U)
#define ADC_CLK_NAME                  (clock_adc0)
#define ADC_PMT_DMA_BUFF_LEN_IN_4BYTES  48
#define ADC_DMA_BUFF_LEN_IN_BYTES      100


#if !ADC_BITS_16
#include "hpm_adc12_drv.h"
#define ADC_TYPE                       ADC12_Type
#define ADC_CONV_MODE_T                adc12_conversion_mode_t
#define ADC_CONV_MODE                  adc12_conv_mode_preemption
#define ADC_PMT_DATA_T                 adc12_pmt_dma_data_t

#define ADC_HW_TRIG_SRC_PWM_MOTOR1             HPM_TRGM1_DMA_SRC_PWM1_CMP16
#define ADC_HW_TRGM_IN_MOTOR1                  HPM_TRGM1_INPUT_SRC_PWM1_CH16REF
#define ADC_HW_TRIG_SRC_MOTOR1                 HPM_PWM1
#define ADC_HW_TRGM_MOTOR1                     HPM_TRGM1
#define ADC_HW_TRGM_OUT_MOTOR1                 TRGM_TRGOCFG_ADCX_PTRGI0A
#define ADC_PMT_TRIG_CH_MOTOR1                 ADC12_CONFIG_TRG1A
#define ADC_SRC_ADDR_OFFSET_MOTOR1             (16 * ADC_PMT_TRIG_CH_MOTOR1) 


#define ADC_DMA_SRC_MOTOR1_U                          HPM_TRGM1_DMA_SRC_PWM1_CMP20
#define ADC_MOTOR1_U                                  HPM_ADC0
#define ADC_CHANNEL_MOTOR1_U                          8                   
#define ADC_DMA_CH_MOTOR1_U                           7//max 7
#define ADC_TRGM_DMACFG_MOTOR1_U                     TRGM_DMACFG_0
#define ADC_DMA_SRC_ADC_MOTOR1_U                      HPM_DMA_SRC_MOT1_0


#define ADC_DMA_SRC_MOTOR1_V                         HPM_TRGM1_DMA_SRC_PWM1_CMP21
#define ADC_MOTOR1_V                                  HPM_ADC1
#define ADC_CHANNEL_MOTOR1_V                          9
#define ADC_DMA_CH_MOTOR1_V                           6
#define ADC_TRGM_DMACFG_MOTOR1_V                      TRGM_DMACFG_2    
#define ADC_DMA_SRC_ADC_MOTOR1_V                      HPM_DMA_SRC_MOT1_2

#define ADC_DMA_SRC_MOTOR1_W                          HPM_TRGM1_DMA_SRC_PWM1_CMP22
#define ADC_MOTOR1_W                                   HPM_ADC2
#define ADC_CHANNEL_MOTOR1_W                           10
#define ADC_DMA_CH_MOTOR1_W                            4 
#define ADC_TRGM_DMACFG_MOTOR1_W                       TRGM_DMACFG_3
#define ADC_DMA_SRC_ADC_MOTOR1_W                       HPM_DMA_SRC_MOT1_3

typedef struct {
  ADC_TYPE *ptr;
  uint8_t   pmt_trig_ch;
  uint32_t  buff_addr;

  TRGM_Type *hw_trgm;
  uint8_t   trgm_in;
  uint8_t   trgm_out;

  PWM_Type *pwm;
  uint8_t   pwm_ch;
  uint8_t   ch_len;
  uint8_t   *trig_adc_channel;
  uint8_t   ph_index;
  uint32_t  src_addr;
  uint32_t  dst_addr;

  uint8_t  dma_src;
  uint8_t  trgm_dma;
  uint8_t  dma_src_adc;

}adc_trig_param_t;
#else
#include "hpm_adc16_drv.h"
#define ADC_TYPE                       ADC16_Type
#define ADC_CONV_MODE_T                adc16_conversion_mode_t
#define ADC_CONV_MODE                  adc16_conv_mode_preemption
#define ADC_PMT_DATA_T                 adc16_pmt_dma_data_t

#define ADC_HW_TRIG_SRC_PWM_MOTOR1             HPM_TRGM0_DMA_SRC_PWM0_CMP8
#define ADC_HW_TRGM_IN_MOTOR1                  HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define ADC_HW_TRIG_SRC_MOTOR1                 HPM_PWM0
#define ADC_HW_TRGM_MOTOR1                     HPM_TRGM0
#define ADC_HW_TRGM_OUT_MOTOR1                 TRGM_TRGOCFG_ADCX_PTRGI0A
#define ADC_PMT_TRIG_CH_MOTOR1                 ADC16_CONFIG_TRG0A
#define ADC_SRC_ADDR_OFFSET_MOTOR1             (16 * ADC_PMT_TRIG_CH_MOTOR1) 


#define ADC_DMA_SRC_MOTOR1                      HPM_TRGM0_DMA_SRC_PWM0_CMP9
#define ADC_MOTOR1                              HPM_ADC0                 
#define ADC_DMA_CH_MOTOR1                       7//max 7
#define ADC_TRGM_DMACFG_MOTOR1                  TRGM_DMACFG_0
#define ADC_DMA_SRC_ADC_MOTOR1                  HPM_DMA_SRC_MOT_0

#define ADC_CHANNEL_MOTOR1_U                    5 //OSIN
#define ADC_CHANNEL_MOTOR1_V                    6//OCOS
#define ADC_CHANNEL_MOTOR1_W                    10//4//OEXC
#endif
                
typedef enum  {
    MOTOR_ADC_U,
    MOTOR_ADC_V,
    MOTOR_ADC_W,
    MOTOR_PHASE_CNT,
} motor_adc_ph_t;

extern uint32_t pmt_buff[ADC_PMT_DMA_BUFF_LEN_IN_4BYTES];
extern uint8_t trig_adc_channel[3];
extern uint8_t adc_done[2];
extern uint16_t adc_buff_u[ADC_DMA_BUFF_LEN_IN_BYTES * 2];
extern uint16_t adc_buff_v[ADC_DMA_BUFF_LEN_IN_BYTES * 2];
extern uint16_t adc_buff_w[ADC_DMA_BUFF_LEN_IN_BYTES * 2];

/**
 * @brief     pwm triggers adc sampling, and dma carries adc sampling value configuration
 *            ;pwm触发adc采样，dma搬运adc采样值配置
 */
void init_adc_motor1(void);

/**
 * @brief     Automatic loop transfers ADC data to buffer
 *            ;自动的循环搬运ADC数据到buffer
 */
void hdma_auto_config(uint8_t dma_ch,uint32_t src_addr,uint32_t dst_addr);
/**
 * @brief     Process the rdc signal sampled by the adc
 *            ;处理adc采样回来的旋变信号
 */
void adc_data_process(void);
/**
 * @brief     print adc result
 *            ;打印adc结果
 */
void adc_data_printf(void);

/** @} */

#endif /* ADC_DMA_H */
