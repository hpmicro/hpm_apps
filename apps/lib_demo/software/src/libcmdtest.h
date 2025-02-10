#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_synt_drv.h"
#include "libhpm_motor.h"

/*Timer define*/

#define BOARD_BLDC_TMR_MS    HPM_GPTMR2
#define BOARD_BLDC_TMR_CH     0
#define BOARD_BLDC_TMR_CMP    0
#define BOARD_BLDC_TMR_IRQ    IRQn_GPTMR2
#define BOARD_BLDC_TMR_RELOAD (1000000U)//10ms
#define BOARD__BLDC_GPTMR_CLK_NAME                      clock_gptmr2


/**
 * @brief     user define motor relative position
 *            ;相对运动距离，单位：r
 */
#define MOVE_BASOLUTE_POS_R                    20
/**
 * @brief     user define motor move speed
 *            ;运动速度，单位：r/s
 */
#define MOVE_SPEED                              10
/**
 * @brief     user define motor move acc
 *            ;运动加速度，单位：r/s/s
 */
#define MOVE_ACC                                100
/**
 * @brief     user define motor move jerk
 *            ;运动加加速度，单位：r/s/s/s
 */
#define MOVE_JERK                               1000
/**
 * @brief     user define motor move time with constant speed
 *            ;速度模式下，匀速运动时间，单位：s
 */
#define CONSTANT_SPEED_TIME_S                   1
/**
 * @brief     user define motor move cycle count
 *            ;位置模式下，电机轴往返运动次数
 */
#define CYCLE_CNT                               2

/**
 * @brief     user define motor dwell time  
 *            ;位置模式下，运行相对位置后轴停止时间，单位：ms
 */
#define DWELLTIME_MS                            500   
/**
 * @brief     set isr time   
 *            ;更新位置/速度序列的中断时间，与定时器中断时间保持一致，单位：s
 */
#define ISR_TIME_S                              0.01

typedef enum {
/**
 * @brief     user define motor in  continous round move type 
 *            ;位置模式下，电机轴以连续往返模式运动
 */
 CONTINOUS_RONUD_MOVE_TYPE ,              
/**
 * @brief     user define motor in signle move type 
 *            ;位置模式下，电机轴以单次往返模式运动
 */
SIGNLE_MOVE_TYPE    ,                    
/**
 * @brief     user define motor in multiple move type 
 *            ;位置模式下，电机轴以多次往返模式运动
 */
MULTIPLE_MOVE_TYPE  ,                   

}move_mode;
