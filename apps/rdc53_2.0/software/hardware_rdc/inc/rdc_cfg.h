/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef RDC_CFG_H
#define RDC_CFG_H

#include "board.h"
#include "hpm_dac_drv.h"
#include "pll_init.h"
#include "sei_init.h"
#include "qeo_init.h"

/**
 * @brief RDC CONFIG API ;RDC配置接口函数
 * @addtogroup RDC_CONFIG_API
 * @{
 *
 */

/**
 * @brief     SPI debug
 *            SPI通信
 */
#define  SPI_DEBUG_CONTROL     0

/**
 * @brief     ABZ mode
 *            abz模式输出
 */
#define  ABZ_OUTPUT            0

/**
 * @brief     UART debug
 *            串口通信
 */
#define UART_DEBUG_CONTROL     0

/**
 * @brief     ABS encoder
 *            23位绝对值编码器
 */
#define ABS_ENCODER_23BIT      0

/**
 * @brief     SEGGER RTT mode
 *            RTT方式获取数据
 */
#define SEGGER_RTT_DEBUG       0

#define TAMAGAWA_SLAVE_POS_HARDWARE_INJECT         0

#define TAMAGAWA_SLAVE                  0

#define  BISSC_SLAVE                   0
#define  BISSC_SLAVE_POS_HARDWARE_INJECT  0//rdc位置trig到bissc

#define PLL                             1
#define PLL_II                           0

extern pll_para_t pll;
extern speed_Cal_Obj speed_CalObj[2];
extern sei_angle_CalObj sei_angleCalObj;
extern qeo_abz_Obj qeo_abzObj;
extern pll_ii_t pll_ii;


/**
 * @brief      ADC16 trigger configuration struct for the preemption mode  
 *            ;ADC抢占模式配置
 * @param[in]    trig_ch   trig src;抢占触发源
 * @param[in]    inten   isr enable;中断使能
 */
void init_trigger_cfg(uint8_t trig_ch, bool inten);

/**
 * @brief      ADC16 init  
 *            ;ADC初始化
 */
void adc_init(void);

/**
 * @brief      pwm trig adc sample  
 *            ;pwm触发adc采样配置
 * @param[in]    ptr   trig src;pwm触发信号
 */
void init_pwm_pla_trgm(TRGM_Type *ptr);

/**
 * @brief      rdc confg  
 *            ;rdc 配置
 * @param[in]    rdc   rdc struct;RDC结构体
 */
void rdc_cfg(RDC_Type *rdc);

/**
 * @brief      dac confg  
 *            ;dac配置
 * @param[in]    mode   dac mode;DAC工作模式
 */
void init_common_config(dac_mode_t mode);

/**
 * @brief      fault diagnosis  
 *            ;断线检测
 * @param[in]    sin_i   sin value;外包络线sin
 * @param[in]    cos_q   cos value;外包络线cos
 */
void fault_diagnosis(int32_t sin_i,int32_t cos_q);

/**
 * @brief      board init in rtt mode  
 *            ;rtt模式下板级初始化
 */
void board_init_rtt_test(void);

/** @} */

#endif /* RDC_CFG_H */
