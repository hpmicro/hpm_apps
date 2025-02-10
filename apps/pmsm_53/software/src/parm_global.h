#ifndef PARM_GLOBAL_H_
#define PARM_GLOBAL_H_

#include "board.h"
#include "hpm_adc.h"
#include "hpm_bldc_define.h"
#include "wr_eeprom.h"

/**
 * @brief PARM GLOBAL API ;全局参数接口函数
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
 * @brief     definition of adc
 *            ;adc相关宏定义
 */
#define BOARD_PMSM0_ADC_MODULE                          ADCX_MODULE_ADC16
#define BOARD_PMSM0_ADC_U_BASE                          HPM_ADC0
#define BOARD_PMSM0_ADC_V_BASE                          HPM_ADC1
#define BOARD_PMSM0_ADC_TRIG_FLAG                       adc16_event_trig_complete
#define BOARD_PMSM0_ADC_CH_U                            (2U)
#define BOARD_PMSM0_ADC_CH_V                            (3U)
#define BOARD_PMSM0_ADC_CH_W                            (4U)
#define BOARD_PMSM0_ADC_IRQn                            IRQn_ADC1
#define BOARD_PMSM0_ADC_SEQ_DMA_SIZE_IN_4BYTES          (40U)
#define BOARD_PMSM0_ADC_TRG                             ADC16_CONFIG_TRG0A
#define BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN                (1U)
#define BOARD_PMSM0_PWM_TRIG_CMP_INDEX                  (8U)
#define BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP      (9U)
#define BOARD_PMSM0_TRIGMUX_IN_NUM                      HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_PMSM0_TRG_NUM                             TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_BLDC_ADC_SEQ_DMA_SIZE_IN_4BYTES           (40U)

/**
 * @brief     definition of pwm
 *            pwm相关宏定义
 */
#define BOARD_PMSM0PWM                                  HPM_PWM0
#define BOARD_PMSM0_UH_PWM_OUTPIN                       (0U)
#define BOARD_PMSM0_UL_PWM_OUTPIN                       (1U)
#define BOARD_PMSM0_VH_PWM_OUTPIN                       (4U)
#define BOARD_PMSM0_VL_PWM_OUTPIN                       (5U)
#define BOARD_PMSM0_WH_PWM_OUTPIN                       (6U)
#define BOARD_PMSM0_WL_PWM_OUTPIN                       (7U)
#define BOARD_PMSM0PWM_TRGM                             HPM_TRGM0
#define BOARD_PMSM0APP_PWM_IRQ                          IRQn_PWM0
#define BOARD_PMSM0PWM_CMP_INDEX_0                      (0U)
#define BOARD_PMSM0PWM_CMP_INDEX_1                      (1U)
#define BOARD_PMSM0PWM_CMP_INDEX_2                      (4U)
#define BOARD_PMSM0PWM_CMP_INDEX_3                      (5U)
#define BOARD_PMSM0PWM_CMP_INDEX_4                      (6U)
#define BOARD_PMSM0PWM_CMP_INDEX_5                      (7U)

/**
 * @brief     definition of timer
 *            1ms中断定时器
 */
#define BOARD_BLDC_TMR_1MS                              HPM_GPTMR2
#define BOARD_BLDC_TMR_CH                               0
#define BOARD_BLDC_TMR_CMP                              0
#define BOARD_BLDC_TMR_IRQ                              IRQn_GPTMR2
#define BOARD_BLDC_TMR_RELOAD                           (100000U)
#define BOARD__BLDC_GPTMR_CLK_NAME                      clock_gptmr2

/**
 * @brief     definition of encoder
 *            编码器相关宏定义
 */
#define BOARD_PMSM0_QEI_BASE                            HPM_QEI0
#define BOARD_PMSM0_QEI_IRQ                             IRQn_QEI0
#define BOARD_PMSM0_QEI_MOTOR_PHASE_COUNT_PER_REV       (16U)
#define BOARD_PMSM0_QEI_CLOCK_SOURCE                    clock_mot0
#define BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV         (4000U)
#define BOARD_SEI_PHASE_COUNT_MAX                       (131072U)
#define PMSM_SEI_CTRL                                   SEI_CTRL_0
#define PMSM_SEI_IRQn                                   IRQn_SEI0

/**
 * @brief     motor clock
 *            电机时钟
 */
#define BOARD_MOTOR_CLK_NAME                            clock_mot0

/**
 * @brief     motor axis number
 *            电机轴数
 */
#define MAX_AXIS_NUM                                    1

/**
 * @brief     user define motor absolute position
 *            ;绝对运动距离，单位：r
 */
#define MOVE_ABSOLUTE_POS_r                            20
/**
 * @brief     user define motor move speed
 *            ;运动速度，单位：r/s
 */
#define MOVE_SPEED                                     10
/**
 * @brief     user define motor move acc
 *            ;运动加速度，单位：r/s/s
 */
#define MOVE_ACC                                       100
/**
 * @brief     user define motor move jerk
 *            ;运动加加速度，单位：r/s/s/s
 */
#define MOVE_JERK                                      1000
/**
 * @brief     user define motor move time with constant speed
 *            ;速度模式下，匀速运动时间，单位：s
 */
#define CONSTANT_SPEED_TIME_s                          5
/**
 * @brief     user define motor move cycle count
 *            ;位置模式下，电机轴往返运动次数
 */
#define CYCLE_CNT                                      1
/**
 * @brief     user define motor in  continous round move type 
 *            ;位置模式下，电机轴以连续往返模式运动
 */
#define CONTINOUS_RONUD_MOVE_TYPE                      0 
/**
 * @brief     user define motor in signle move type 
 *            ;位置模式下，电机轴以单次往返模式运动
 */
#define SIGNLE_MOVE_TYPE                               1
/**
 * @brief     user define motor in multiple move type 
 *            ;位置模式下，电机轴以多次往返模式运动
 */
#define MULTIPLE_MOVE_TYPE                             2
/**
 * @brief     user define motor dwell time  
 *            ;位置模式下，运行相对位置后轴停止时间，单位：ms
 */
#define DWELLTIME_ms                                   500   
/**
 * @brief     set isr time   
 *            ;更新位置/速度序列的中断时间，单位：s
 */
#define ISR_TIME_s                                     0.001

/**
 * @brief     enum of encoder type
 *            ;编码器类型枚举
 */
typedef enum _tag_encoder_Type{
ABZ,
ABS,
}encoder_Type;

/**
 * @brief     Encoder parameter calculation
 *            ;编码器参数计算
 */
typedef struct _tag_pos_angle_cal_obj{
int32_t z;
int32_t ph;
int32_t maxph;
uint8_t motor_pole;
int32_t elec_angle;
int32_t pos;
int32_t pos0;
uint8_t encoder_type;
uint8_t motor_id;
}pos_angle_CalObj;


/**                                            
 * @brief     Global control variable
 *            ;全局控制变量
 */
typedef struct {

    /** @brief Control word    
     *         ;电机轴使能控制字 */
    uint8_t        ControlWord[MAX_AXIS_NUM];
    /** @brief Operation mode   
     *         ;电机轴运行模式 */
    uint8_t        OpMode[MAX_AXIS_NUM];
    /** @brief flag 1ms   
     *         ;1ms中断标志位 */
    uint8_t        flag_1ms;
    /** @brief  user define elec_theta  
     *         ;预定位控制电角度 */
    float          commu_theta;
    /** @brief  user define id value  
     *         ;预定位控制d轴电流 */
    float          commu_IdRef;
    /** @brief  motor run status  
     *         ;电机运动状态 */   
    uint8_t        motor_status;
    /** @brief  Number of phase finding execution  
     *         ;电机寻相执行次数 */ 
    uint8_t        zero_cnt;
    /** @brief  controlword of phase finding execution 
     *         ;电机寻相执行控制字 */ 
    int16_t        zero_CW[MAX_AXIS_NUM];

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


typedef enum _tag_eeprom_OP{
    NULL_Op,
    SET_TO_DEFAULT,
    SAVE_TO_EEPROM,
}eeprom_Op;

/**
 * @brief     Motor foc parameter structure
 *            ;电机foc控制结构体
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
    void (*adc_trig_event_callback)();
}MOTOR_PARA;


/**
 * @brief     parameters of the velocity loop.
 */
typedef struct _tag_Vel_Cfg_Obj{
int32_t Kp0x6000;/**< proportional gain. */
float Ki0x6001_3;/**< integral gain. */
int32_t Vel_ForWard_Gain0x6002;/**< speed feedforward gain. */
float LowPassWn0x6003_3;/**< low pass wn freq. */
int32_t Vel_Ref0x6004;/**< velocity cmd. */
int32_t Acc_Ref0x6005;/**< acceleration cmd. */
int32_t Jerk_Ref0x6006;/**< jerk cmd. */
int32_t Dec_Ref0x6007;/**< dec cmd. */
int32_t Vel_Fdk0x6008;/**< velocity feedback. */
int32_t Vel_err0x6009;/**< velocity error. */
int32_t Vel_IntrErr0x600a;/**< velocity error(counts). */
int32_t Vel_IntrFdk0x600b;/**< velocity feedback(counts). */
int32_t Vel_IntrCmd0x600c;/**< velocity cmd(counts). */
int32_t VelCnstTime0x600d;/**< velocity const time. */
}VelCfg_Obj;

/**
 * @brief     parameters of the current loop.
 */
typedef struct _tag_Current_Cfg_Obj{
float Kp0x5000_3;/**< proportional gain. */
float Ki0x5001_3;/**< integral gain. */
int32_t Current_ref0x5002;/**< current cmd. */
int32_t Current_Fdk0x5003;/**< current feedback. */
}CurrentCfg_Obj;

/**
 * @brief     parameters of motor .
 */
typedef struct _tag_MotorPara_Obj{
float Motor_RatedCurrent0x4000_3;/**< rated current. */
int32_t Mortor_RatedVlotage0x4001;/**< rated vlotage. */
float Mortor_R0x4002_3;/**< R. */
float Mortor_L0x4003_3;/**< L. */
int32_t Mortor_PoleNum0x4004;/**< pole num. */
int32_t Mortor_PolePicth0x4005;/**< pole picth. */
int32_t Motor_Type0x4006;/**< motor type. */
int32_t Motor_EncType0x4007;/**< encoder type. */
int32_t Motor_PlusCnt0x4008;/**< pulse cnts per rotation of the motor . */
}MotorPara_Obj;


/**
 * @brief     parameters of scope func .
 */
typedef struct _tag_Scope_Obj{
int32_t param_save0x8000;
int32_t paramOk_flag0x8001;
}ASSIST_Obj;

/**
 * @brief     parameters of the position loop .
 */
typedef struct _tag_PosCfg_Obj{
float Kp0x7000_3;/**< proportional gain. */
int32_t Pos_ref0x7001;/**< position cmd. */
int32_t Pos_Fdk0x7002;/**< position feedback. */
int32_t Pos_err0x7003;/**< position error. */
int32_t Pos_IntrErr0x7004;/**< position error(counts). */
int32_t Pos_IntrFdk0x7005;/**< position feedback(counts). */
int32_t Pos_IntrCmd0x7006;/**< position cmd(counts). */
int32_t CycleType0x7007;/**< motion type. */
int32_t CmdType0x7008;/**< cmd generation mode. */
int32_t DwellTime0x7009;/**< waiting time. */
int32_t CycleCount0x700a;/**< cycle time. */
}PosCfg_Obj;

/**
 * @brief     parameters of cmd .
 */
typedef struct _tag_Cmd_Obj{
int32_t ControlWord0x9000;
int32_t OpMode0x9001;
int32_t zero_ControlWord0x9002;
int32_t status0x9003;
}Cmd_Obj;


/**
 * @brief     Motor parameter structure
 *            ;电机参数结构体
 */
typedef struct {

 MotorPara_Obj    parmMotor;
 CurrentCfg_Obj   parmCurrent;
 VelCfg_Obj       parmVel;
 PosCfg_Obj       parmPos;
 ASSIST_Obj       parmAssist;
 Cmd_Obj          parmCmd;
}AXIS_PARAM;

extern AXIS_PARAM globalAxisParm[MAX_AXIS_NUM];

extern MOTOR_PARA motor0;
extern MOTOR_CONTROL_Global Motor_Control_Global;
extern pos_angle_CalObj pos_angleObj[MAX_AXIS_NUM];


/**
 * @brief     params update after power on
 *            ;上电后参数更新
 */
void parm_UpdatePowerOn(void);

/**
 * @brief     params manage
 *            ;电机轴参数管理
 */
void parm_GlobalManage(void);

/**
 * @brief     motor params init
 *            ;电机轴参数初始化
 */
void parm_GlobalParmStrInit(void);

/** @} */

#endif //__PARM_GLOBAL_H