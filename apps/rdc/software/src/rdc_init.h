#ifndef RDC_INIT_H_
#define RDC_INIT_H_

#include "board.h"

/**
 * @brief RDC API ;rdc硬解码测速API
 * @addtogroup RDC_SPEED_CAL_API
 * @{
 *
 */


/**                                            
 * @brief     speed calculate struct
 *            ;速度计算结构体
 */
typedef struct _tag_speed_Cal_Obj{

    /** @brief motor current electrical angle    
     *         ;电机当前电角度 */
      float speedtheta;
    /** @brief motor last electrical angle    
     *         ;电机上一时刻电角度 */
      float speedlasttheta ;
    /** @brief total motor electrical angle     
     *         ;采样到的电机电角度变化指和 */
      float speedthetalastn ;
    /** @brief sample num   
     *         ;电角度采样次数 */
      int num ;
    /** @brief speed after filter    
     *         ;滤波之后的电机速度 */
      float o_speedout_filter ;
    /** @brief speedout    
     *         ;未滤波的电机速度 */
      float o_speedout;
}SPEED_CAL;

extern SPEED_CAL g_speed_cal;
 
/**
 * @brief           config  PWM.
 *                  ;pwm配置
 */
void Pwm_Duty_Init(void);
/**
 * @brief           Config the PWM channel trig for speed calculation interrupt.
 *                  ;配置pwm通道产生比较中断，用来进行电机速度计算
 */
void Pwm_TrigFor_interrupt_Init(void);
/**
 * @brief           rdc config.
 *                  ;rdc初始化配置
 */
void rdc_init(void);

/** @} */

#endif //__RDC_INIT_H_

