#ifndef PARM_GLOBAL_H_
#define PARM_GLOBAL_H_

#include "hpm_adc.h"
#include "hpm_bldc_define.h"
#include "hpm_clock_drv.h"
#include "board.h"

/**
 * @brief PARM GLOBAL API ;全局参数定义
 * @addtogroup PARM_GLOBAL_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     Motor0 adc define
 *            ;轴0 adc相关宏定义
 */
#define BOARD_PMSM0_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM0_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM0_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM0_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM0_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM0_ADC_CH_U                    (13U)
#define BOARD_PMSM0_ADC_CH_V                    (4U)
#define BOARD_PMSM0_ADC_CH_W                    (13U)
#define BOARD_PMSM0_ADC_IRQn                    IRQn_ADC1
#define BOARD_PMSM0_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM0_ADC_TRG                    ADC16_CONFIG_TRG0A
#define BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM0_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM0_TRIGMUX_IN_NUM              HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_PMSM0_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM0_ADC_IRQn                    IRQn_ADC1
/**
 * @brief     Motor0 pwm define
 *            ;轴0 pwm相关宏定义
 */
#define BOARD_PMSM0PWM                          HPM_PWM0
#define BOARD_PMSM0_UH_PWM_OUTPIN               (0U)
#define BOARD_PMSM0_UL_PWM_OUTPIN               (1U)
#define BOARD_PMSM0_VH_PWM_OUTPIN               (2U)
#define BOARD_PMSM0_VL_PWM_OUTPIN               (3U)
#define BOARD_PMSM0_WH_PWM_OUTPIN               (4U)
#define BOARD_PMSM0_WL_PWM_OUTPIN               (5U)
#define BOARD_PMSM0PWM_TRGM                     HPM_TRGM0
#define BOARD_PMSM0APP_PWM_IRQ                  IRQn_PWM0
#define BOARD_PMSM0PWM_CMP_INDEX_0              (0U)
#define BOARD_PMSM0PWM_CMP_INDEX_1              (1U)
#define BOARD_PMSM0PWM_CMP_INDEX_2              (2U)
#define BOARD_PMSM0PWM_CMP_INDEX_3              (3U)
#define BOARD_PMSM0PWM_CMP_INDEX_4              (4U)
#define BOARD_PMSM0PWM_CMP_INDEX_5              (5U)
#define BOARD_BLDCPWM_CMP_TRIG_CMP              (15U)
/**
 * @brief     Motor0 qei define
 *            ;轴0 qei相关宏定义
 */
#define BOARD_PMSM0_QEI_BASE                    HPM_QEI0
#define BOARD_PMSM0_QEI_IRQ                     IRQn_QEI0
#define BOARD_PMSM0_QEI_TRGM                    HPM_TRGM0
#define BOARD_PMSM0_QEI_TRGM_QEI_A_SRC          HPM_TRGM0_INPUT_SRC_TRGM0_P0
#define BOARD_PMSM0_QEI_TRGM_QEI_B_SRC          HPM_TRGM0_INPUT_SRC_TRGM0_P1
#define BOARD_PMSM0_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM0_QEI_CLOCK_SOURCE            clock_mot0
#define BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/**
 * @brief     Motor1 adc define
 *            ;轴1 adc相关宏定义
 */
#define BOARD_PMSM1_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM1_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM1_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM1_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM1_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM1_ADC_CH_U                    (15U)
#define BOARD_PMSM1_ADC_CH_V                    (5U)
#define BOARD_PMSM1_ADC_CH_W                    (15U)
#define BOARD_PMSM1_ADC_IRQn                    IRQn_ADC1
#define BOARD_PMSM1_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM1_ADC_TRG                    ADC16_CONFIG_TRG1A
#define BOARD_PMSM1_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM1_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM1_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM1_TRIGMUX_IN_NUM              HPM_TRGM1_INPUT_SRC_PWM1_CH8REF
#define BOARD_PMSM1_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM1_ADC_IRQn                    IRQn_ADC1
/**
 * @brief     Motor1 pwm define
 *            ;轴1 pwm相关宏定义
 */
#define BOARD_PMSM1PWM                          HPM_PWM1
#define BOARD_PMSM1_UH_PWM_OUTPIN               (0U)
#define BOARD_PMSM1_UL_PWM_OUTPIN               (1U)
#define BOARD_PMSM1_VH_PWM_OUTPIN               (2U)
#define BOARD_PMSM1_VL_PWM_OUTPIN               (3U)
#define BOARD_PMSM1_WH_PWM_OUTPIN               (4U)
#define BOARD_PMSM1_WL_PWM_OUTPIN               (5U)
#define BOARD_PMSM1PWM_TRGM                     HPM_TRGM1
#define BOARD_PMSM1APP_PWM_IRQ                  IRQn_PWM1
#define BOARD_PMSM1PWM_CMP_INDEX_0              (0U)
#define BOARD_PMSM1PWM_CMP_INDEX_1              (1U)
#define BOARD_PMSM1PWM_CMP_INDEX_2              (2U)
#define BOARD_PMSM1PWM_CMP_INDEX_3              (3U)
#define BOARD_PMSM1PWM_CMP_INDEX_4              (4U)
#define BOARD_PMSM1PWM_CMP_INDEX_5              (5U)
/**
 * @brief     Motor1 qei define
 *            ;轴1 qei相关宏定义
 */
#define BOARD_PMSM1_QEI_BASE                    HPM_QEI1
#define BOARD_PMSM1_QEI_IRQ                     IRQn_QEI1
#define BOARD_PMSM1_QEI_TRGM                    HPM_TRGM1
#define BOARD_PMSM1_QEI_TRGM_QEI_A_SRC          HPM_TRGM1_INPUT_SRC_TRGM1_P6
#define BOARD_PMSM1_QEI_TRGM_QEI_B_SRC          HPM_TRGM1_INPUT_SRC_TRGM1_P7
#define BOARD_PMSM1_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM1_QEI_CLOCK_SOURCE            clock_mot1
#define BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/**
 * @brief     Motor2 adc define
 *            ;轴2 adc相关宏定义
 */
#define BOARD_PMSM2_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM2_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM2_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM2_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM2_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM2_ADC_CH_U                    (6U)
#define BOARD_PMSM2_ADC_CH_V                    (6U)
#define BOARD_PMSM2_ADC_CH_W                    (6U)
#define BOARD_PMSM2_ADC_IRQn                    IRQn_ADC0
#define BOARD_PMSM2_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM2_ADC_TRG                    ADC16_CONFIG_TRG2A
#define BOARD_PMSM2_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM2_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM2_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM2_TRIGMUX_IN_NUM              HPM_TRGM2_INPUT_SRC_PWM2_CH8REF
#define BOARD_PMSM2_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM2_ADC_IRQn                    IRQn_ADC0
/**
 * @brief     Motor2 pwm define
 *            ;轴2 pwm相关宏定义
 */
#define BOARD_PMSM2PWM                          HPM_PWM2
#define BOARD_PMSM2_UH_PWM_OUTPIN               (0U)
#define BOARD_PMSM2_UL_PWM_OUTPIN               (1U)
#define BOARD_PMSM2_VH_PWM_OUTPIN               (2U)
#define BOARD_PMSM2_VL_PWM_OUTPIN               (3U)
#define BOARD_PMSM2_WH_PWM_OUTPIN               (4U)
#define BOARD_PMSM2_WL_PWM_OUTPIN               (5U)
#define BOARD_PMSM2PWM_TRGM                     HPM_TRGM2
#define BOARD_PMSM2APP_PWM_IRQ                  IRQn_PWM2
#define BOARD_PMSM2PWM_CMP_INDEX_0              (0U)
#define BOARD_PMSM2PWM_CMP_INDEX_1              (1U)
#define BOARD_PMSM2PWM_CMP_INDEX_2              (2U)
#define BOARD_PMSM2PWM_CMP_INDEX_3              (3U)
#define BOARD_PMSM2PWM_CMP_INDEX_4              (4U)
#define BOARD_PMSM2PWM_CMP_INDEX_5              (5U)
/**
 * @brief     Motor2 qei define
 *            ;轴2 qei相关宏定义
 */
#define BOARD_PMSM2_QEI_BASE                    HPM_QEI2
#define BOARD_PMSM2_QEI_IRQ                     IRQn_QEI2
#define BOARD_PMSM2_QEI_TRGM                    HPM_TRGM2
#define BOARD_PMSM2_QEI_TRGM_QEI_A_SRC          HPM_TRGM2_INPUT_SRC_TRGM2_P6
#define BOARD_PMSM2_QEI_TRGM_QEI_B_SRC          HPM_TRGM2_INPUT_SRC_TRGM2_P7
#define BOARD_PMSM2_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM2_QEI_CLOCK_SOURCE            clock_mot2
#define BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/**
 * @brief     Motor3 adc define
 *            ;轴3 adc相关宏定义
 */
#define BOARD_PMSM3_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM3_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM3_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM3_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM3_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM3_ADC_CH_U                    (7U)
#define BOARD_PMSM3_ADC_CH_V                    (7U)
#define BOARD_PMSM3_ADC_CH_W                    (7U)
#define BOARD_PMSM3_ADC_IRQn                    IRQn_ADC0
#define BOARD_PMSM3_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM3_ADC_TRG                    ADC16_CONFIG_TRG3A
#define BOARD_PMSM3_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM3_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM3_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM3_TRIGMUX_IN_NUM              HPM_TRGM3_INPUT_SRC_PWM3_CH8REF
#define BOARD_PMSM3_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM3_ADC_IRQn                    IRQn_ADC0
/**
 * @brief     Motor3 pwm define
 *            ;轴3 pwm相关宏定义
 */
#define BOARD_PMSM3PWM                          HPM_PWM3
#define BOARD_PMSM3_UH_PWM_OUTPIN               (0U)
#define BOARD_PMSM3_UL_PWM_OUTPIN               (1U)
#define BOARD_PMSM3_VH_PWM_OUTPIN               (2U)
#define BOARD_PMSM3_VL_PWM_OUTPIN               (3U)
#define BOARD_PMSM3_WH_PWM_OUTPIN               (4U)
#define BOARD_PMSM3_WL_PWM_OUTPIN               (5U)
#define BOARD_PMSM3PWM_TRGM                     HPM_TRGM3
#define BOARD_PMSM3APP_PWM_IRQ                  IRQn_PWM3
#define BOARD_PMSM3PWM_CMP_INDEX_0              (0U)
#define BOARD_PMSM3PWM_CMP_INDEX_1              (1U)
#define BOARD_PMSM3PWM_CMP_INDEX_2              (2U)
#define BOARD_PMSM3PWM_CMP_INDEX_3              (3U)
#define BOARD_PMSM3PWM_CMP_INDEX_4              (4U)
#define BOARD_PMSM3PWM_CMP_INDEX_5              (5U)
/**
 * @brief     Motor3 qei define
 *            ;轴3 qei相关宏定义
 */
#define BOARD_PMSM3_QEI_BASE                    HPM_QEI3
#define BOARD_PMSM3_QEI_IRQ                     IRQn_QEI3
#define BOARD_PMSM3_QEI_TRGM                    HPM_TRGM3
#define BOARD_PMSM3_QEI_TRGM_QEI_A_SRC          HPM_TRGM3_INPUT_SRC_TRGM3_P1
#define BOARD_PMSM3_QEI_TRGM_QEI_B_SRC          HPM_TRGM3_INPUT_SRC_TRGM3_P2
#define BOARD_PMSM3_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM3_QEI_CLOCK_SOURCE            clock_mot3
#define BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/**
 * @brief     Motor trgm define
 *            ;互联管理器相关宏定义
 */
#define BOARD_PMSM0PWM_TRGM                     HPM_TRGM0
#define BOARD_PMSM1PWM_TRGM                     HPM_TRGM1
#define BOARD_PMSM2PWM_TRGM                     HPM_TRGM2
#define BOARD_PMSM3PWM_TRGM                     HPM_TRGM3
#define MOTOR_PMSM_PWM_SYNC                     HPM_SYNT
#define BOARD_PMSM_MOTOR_CLOCK_SOURCE           clock_mot0


/////////////////////////////////////////////////*****///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief     Motor speed loop ki/kp params
 *            ;速度环kp/ki
 */
#define SPEEDLOOP_KP                            50//60
#define SPEEDLOOP_KI                            0.01
#define SPEEDLOOP_OUTMAX                        500

/**
 * @brief     Motor position loop ki/kp params
 *            ;位置环kp/ki
 */
#define POSITIONLOOP_KP                         0.0005//0.001
#define POSITIONLOOP_KI                         0
#define POSITIONLOOP_OUTMAX                     50
///////////////////////////////////////////////user define motor move params/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief     user define motor absloute position
 *            ;绝对运动距离，单位：r
 */
#define MOVE_ABSOLUTE_POS_r                     20
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
#define CONSTANT_SPEED_TIME_s                   5
/**
 * @brief     user define motor move cycle count
 *            ;位置模式下，电机轴往返运动次数
 */
#define CYCLE_CNT                               1
/**
 * @brief     user define motor in  continous round move type 
 *            ;位置模式下，电机轴以连续往返模式运动
 */
#define CONTINOUS_RONUD_MOVE_TYPE               0 
/**
 * @brief     user define motor in signle move type 
 *            ;位置模式下，电机轴以单次往返模式运动
 */
#define SIGNLE_MOVE_TYPE                        1
/**
 * @brief     user define motor in multiple move type 
 *            ;位置模式下，电机轴以多次往返模式运动
 */
#define MULTIPLE_MOVE_TYPE                      2
/**
 * @brief     user define motor dwell time  
 *            ;位置模式下，运行相对位置后轴停止时间，单位：ms
 */
#define DWELLTIME_ms                            500   
/**
 * @brief     set isr time   
 *            ;更新位置/速度序列的中断时间，单位：s
 */
#define ISR_TIME_s                              0.001

/**                                            
 * @brief     Global control variable
 *            ;全局控制变量
 */
typedef struct {

    /** @brief Control word    
     *         ;电机轴使能控制字 */
    uint8_t        ControlWord[4];
    /** @brief Operation mode   
     *         ;电机轴运行模式 */
    uint8_t        OpMode[4];
    /** @brief flag 1ms   
     *         ;1ms中断标志位 */
    uint8_t        flag_1ms;
    /** @brief  user define elec_theta  
     *         ;预定位控制电角度 */
    float          commu_theta;
    /** @brief  user define id value  
     *         ;预定位控制d轴电流 */
    float          commu_IdRef;
}MOTOR_CONTROL_Global;


/**
 * @brief     enum for motor operation mode
 *            ;电机运行模式
 */
typedef enum _tag_OpMode{
    /** @brief  position mode  
     *         ;位置模式 */
    POSITION_MODE,
    /** @brief  speed mode  
     *         ;速度模式 */
    SPEED_MODE,
}OpMode_Typ;
/**
 * @brief     Motor parameter structure
 *            ;电机参数结构体
 */
typedef struct {
    /** @brief Control parameter structure    
     *         ;控制参数结构体 */
    BLDC_CONTROL_FOC_PARA       foc_para;
    /** @brief Velocity parameter structure    
     *         ;速度参数结构体 */
    BLDC_CONTRL_PID_PARA        speedloop_para; 
#if MOTOR0_SMC_EN
    BLDC_CONTROL_SMC_PARA       smc_para;
#endif
    /** @brief Positional parameter structure    
     *         ;位置参数结构体 */
    BLDC_CONTRL_PID_PARA        position_para;
    void (*adc_trig_event_callback)(void);
}MOTOR_PARA;


extern MOTOR_PARA motor0;
extern MOTOR_PARA motor1;
extern MOTOR_PARA motor2;
extern MOTOR_PARA motor3;
extern MOTOR_CONTROL_Global Motor_Control_Global;

/** @} */

#endif //__PARM_GLOBAL_H