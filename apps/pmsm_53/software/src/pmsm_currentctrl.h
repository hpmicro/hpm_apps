#ifndef PMSM_CurrentCtrl_H_
#define PMSM_CurrentCtrl_H_

#include "hpm_pwm_drv.h"
#include "hpm_adc.h"
#include "hpm_gpio_drv.h"
#include "hpm_bldc_define.h"
#include "hpm_qeiv2_drv.h"
#include "hpm_sei_drv.h"
#include "parm_global.h"
#include "bldc_foc_callback.h"

/**
 * @brief PMSM CURRENTCTRL API ;电流环控制接口函数
 * @addtogroup PMSM_CURRENTCTRL_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     calculate electrical Angle from by encoder position .
 *            ;电气角度以及电机位置计算
* @param[in]  pos_angleCalHdl    pos_angle struct;电机位置角度结构体
 */
void electric_angle_cal(pos_angle_CalObj* pos_angleCalHdl);

/**
 * @brief     current loop
 *            ;电流环执行
 */
void motor0_highspeed_loop(void);

/**
 * @brief     Config PWM  and electrical Angle while looking for initial phase angle
 *            ;配置电角度以及pwm占空比执行预定位操作
 */
void motor0_angle_align_loop(void);

/** @} */

#endif //__PMSM_CURRENTCTRL_H