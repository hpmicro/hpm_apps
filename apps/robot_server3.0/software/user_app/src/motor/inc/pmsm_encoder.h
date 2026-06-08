/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef PMSM_ENCODER_H_
#define PMSM_ENCODER_H_

#include "Parm_Global.h"
#include "pmsm_currentctrl.h"
#include "hpm_qeiv2_drv.h"
#include "hpm_spi_drv.h"


#if USE_MOTOR_DM

#define MT6701_ENCODER_SPI_BASE                      HPM_SPI0
#define MT6701_ENCODER_SPI_CLK_NAME                  clock_spi0
#define MT6701_ENCODER_SPI_SCLK_FREQ                 (20000000UL)
#define MT6701_ENCODER_SPI_DATA_LEN_IN_BITS          (24U)

typedef enum {
    spi_op_write = 0,
    spi_op_read,
    spi_op_no_data
} spi_op_t;


void init_mt6701_encoder_spi_pins(void);

void init_encoder_mt6701(void);
uint32_t encoder_read_mt6701(void);
#endif

void motor_encoder_spi_elecinit(MOTOR_CONTROL_Global* global);




/**
 * @brief PMSM ENCODER API ;电机编码器计算接口
 * @addtogroup PMSM_ENCODER_API
 * @{
 *
 */

extern MOTOR_CONTROL_Global Motor_Control_Global;
extern MOTOR_PARA motor;
extern qei_CalObj qeiCalObj;

/**
 * @brief     Encoder parameter calculation
 *            ;spi编码器参数计算
 */
typedef struct {
    int32_t init_angle;
    int32_t ph;
    int32_t maxph;
    uint8_t motor_pole;
    int32_t elec_angle;
    int32_t pos;
}spi_CalObj;


/**
 * @brief        calculate electric theta 
 *               ;电气角度计算
 * @param[in]    qei_CalHdl      qei param;编码器计算结构体
 * @param[in]    global      global param;全局变量结构体
 */
extern void electric_angle_cal(qei_CalObj* qei_CalHdl,MOTOR_CONTROL_Global* global);

/** @} */

#endif//PMSM_ENCODER_H_