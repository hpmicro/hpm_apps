#ifndef PMSM_Motor_Init_H_
#define PMSM_Motor_Init_H_

#include "parm_global.h"
#include "hpm_adc.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_bldc_define.h"
#include "hpm_foc.h"
#include "hpm_smc.h"
#include "bldc_foc_callback.h"
#include "pmsm_currentctrl.h"
#include "pmsm_speedctrl.h"
#include "hpm_qeiv2_drv.h"
#include "hpm_sei_drv.h"


/**
 * @brief PMSM INIT API ;电机初始化接口函数
 * @addtogroup PMSM_INIT_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     BLDC current set time
 *            ;电流采样时间设置
 */
#define BLDC_CURRENT_SET_TIME_MS            (200U)
/**
 * @brief     PWM frequency
 *            ;PWM频率，单位hz
 */
#define PWM_FREQUENCY                       (20000) /*PWM 频率  单位HZ*/

/**
 * @brief     PWM reload
 *            ;pwm 重载值
 */
#define PWM_RELOAD                          (motor0_clock_hz/PWM_FREQUENCY) /*20K hz  = 200 000 000/PWM_RELOAD */


extern uint32_t adc_buff[3][BOARD_BLDC_ADC_SEQ_DMA_SIZE_IN_4BYTES];

/**
 * @brief        Automatic phase finding
 *               ;电机自动寻相
 * @param[in]    i_motor_id    motor id;电机ID
 */
void motor_foc_angle_align(uint8_t i_motor_id);

/**
 * @brief     PMSM Motor1 initialization
 *            ;电机初始化
 */
void pmsm_motor1_init(void);

/** @} */

#endif //PMSM_INIT_H_
