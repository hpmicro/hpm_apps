/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef BLDC_FOC_CFG_H
#define BLDC_FOC_CFG_H
 

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief BLDC FOC API ;电机控制接口函数
 * @addtogroup BLDC_FOC_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief        pwm enabled;PWM使能
 * @param[in]    motor_index    motor id;电机ID
 * @param[in]    pin_name   pwm pin;PWM引脚名
 */
void bldc_pwm_enable(uint8_t motor_index,uint8_t pin_name);
/**
 * @brief        pwm disable;PWM下使能
 * @param[in]    motor_index    motor id;电机ID
 * @param[in]    pin_name   pwm pin;PWM引脚名
 */
void bldc_pwm_disable(uint8_t motor_index,uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** @} */

#endif //__BLDC_FOC_CFG_H

