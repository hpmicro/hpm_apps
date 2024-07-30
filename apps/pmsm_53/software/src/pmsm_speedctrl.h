#ifndef PMSM_SpeedCtrl_H_
#define PMSM_SpeedCtrl_H_

#include "hpm_gptmr_drv.h"
#include "hpm_qeiv2_drv.h"
#include "parm_global.h"
#include "libhpm_motor.h"
#include "bldc_foc_callback.h"

/**
 * @brief PMSM SPEEDCTRL API ;速度控制接口函数
 * @addtogroup PMSM_SPEEDTCTRL_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     pos_vel loop 
 *            ;速度环/位置环API
 * @param[in]    par   motor struct;电机参数结构体
 * @param[in]    ptr   pwm struct;pwm结构体
 */
void motor_speed_ctrl(MOTOR_PARA *par, PWM_Type *ptr);

/** @} */

#endif //__PMSM_SPEEDTCTRL_H