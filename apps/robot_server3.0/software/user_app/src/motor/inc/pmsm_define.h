/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_BLDC_DEFINE_H
#define HPM_BLDC_DEFINE_H

#ifdef CONFIG_BLDC_HAS_EXTRA_CONFIG
#include CONFIG_BLDC_HAS_EXTRA_CONFIG
#endif

#include "pmsm_math.h"
 
/**
 * @brief PMSM definition;PMSM宏定义
 * @addtogroup PMSM_DEFINITION
 * @{
 *
 */

/**
 * @brief bldc motor rotation direction;电机旋转方向
 *
 */
#define BLDC_MOTOR_DIR_FORWARD 0
#define BLDC_MOTOR_DIR_REVERSE 1

/**
 * @brief Motor pin definition U, V, W three pairs;UVW三相频pin引脚
 *
 */
#define BLDC_PWM_PIN_UH     0
#define BLDC_PWM_PIN_UL     1
#define BLDC_PWM_PIN_VH     2
#define BLDC_PWM_PIN_VL     3
#define BLDC_PWM_PIN_WH     4
#define BLDC_PWM_PIN_WL     5

/**
 * @brief Motor serial number, listing four motors;电机线序
 *
 */
#define BLDC_MOTOR0_INDEX                (1)
#define BLDC_MOTOR1_INDEX                (2)
#define BLDC_MOTOR2_INDEX                (3)
#define BLDC_MOTOR3_INDEX                (4)

/**
 * @brief Serial number of the current sampling array;采样地址
 *
 */
#define ADCU_INDEX                 (0)
#define ADCV_INDEX                 (1)
#define ADCW_INDEX                 (2)

/**
 * @brief PWM output channel definition, used for internal calculations；pwm输出引脚
 *
 */
#define BLDC_PWM_U          0
#define BLDC_PWM_V          1
#define BLDC_PWM_W          2

/**
 * @brief Get ADC data with 12bit valid bits;取adc12位有效值
 *
 */
#define GET_ADC_12BIT_VALID_DATA(x)             ((x & 0xffff) >> 4)


/**
 * @brief Motor related structure definition;电机参数结构体
 *
 */
typedef struct hpm_motor_par {
    float  i_rstator_ohm;           /**< Stator resistance (in ohm) */
    float  i_poles_n;               /**< polar logarithm */
    float  i_maxspeed_rs;           /**< Maximum speed r/s */
    float  i_lstator_h;             /**< Stator inductor */
    float  i_phasecur_a;            /**< Rated current */
    float  i_phasevol_v;            /**< Rated voltage */
    float  i_samplingper_s;         /**< Current sampling period */
    HPM_MOTOR_MATH_TYPE  o_smc_f;   /**< Sliding mode control factor1 */
    HPM_MOTOR_MATH_TYPE  o_smc_g;   /**< Sliding mode control factor2 */
    void   (*func_smc_const)(void *str);     /**< Calculate the function of the sliding mode control coefficient */
} hpm_motor_para_t;

#define BLDC_MOTOR_PARA_DEFAULTS {0, 0, 0,\
                                        0, 0, 0,\
                                        0, 0, 0,\
                                        NULL}

/**
 * @brief Speed calculation parameters;速度环计算结构体
 *
 */
typedef struct  bldc_contrl_spd_par {
    uint16_t            i_speedacq;               /**< Update velocity data once after collecting n times of angle data */
    uint16_t            num;                    /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       i_speedlooptime_s;        /**< Time for n cycles. Unit s */
    HPM_MOTOR_MATH_TYPE       speedtheta;             /**< Current motor angle */
    HPM_MOTOR_MATH_TYPE       speedlasttheta;          /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       speedthetalastn;        /**< Internal Data, Initialization Clear */
    HPM_MOTOR_MATH_TYPE       i_speedfilter;            /**< Low-pass filter coefficient */
    HPM_MOTOR_MATH_TYPE       o_speedout_filter;        /**< Speed after filter */
    HPM_MOTOR_MATH_TYPE       o_speedout;               /**< Speed before filter */
    hpm_motor_para_t     *i_motorpar;                /**< Motor operating parameters */
    HPM_MOTOR_MATH_TYPE      pos;
    HPM_MOTOR_MATH_TYPE      poslast;
    HPM_MOTOR_MATH_TYPE      poslastn;  
    void    (*func_getspd)(void *str);                     /**< Speed calculation function */
} BLDC_CONTRL_SPD_PARA;

#define BLDC_CONTRL_SPD_PARA_DEFAULTS {0, 0, 0, 0, 0,\
                                        0, 0, 0, 0, NULL,\
                                        NULL}

/**
 * @brief pid control parameters;pi控制结构体
 *
 */
typedef struct bldc_contrl_pid_par {
    HPM_MOTOR_MATH_TYPE       i_kp;                   /**< Kp */
    HPM_MOTOR_MATH_TYPE       i_ki;                   /**< Ki */
    HPM_MOTOR_MATH_TYPE       i_kd;                   /**< Kd */
    HPM_MOTOR_MATH_TYPE       i_max;                  /**< Output max,  min = -max */
    HPM_MOTOR_MATH_TYPE       target;                 /**< Target parameters */
    HPM_MOTOR_MATH_TYPE       mem;                    /**< Intenal Data */
    HPM_MOTOR_MATH_TYPE       cur;                    /**< Sampling data  */
    HPM_MOTOR_MATH_TYPE       outval;                 /**< Output Data */
    void (*func_pid)(void *str);                               /**< Pid function */
} BLDC_CONTRL_PID_PARA;
#define BLDC_CONTRL_PID_PARA_DEFAULTS {0, 0, 0, 0,\
                                        0, 0, 0, 0,\
                                       NULL}

/**
 * @brief Current sampling parameters;电流环计算结构体
 *
 */
typedef struct bldc_contrl_current_par {
    uint16_t            adc_u;                  /**< u Phase current AD sampling value */
    uint16_t            adc_v;                  /**< v Phase current AD sampling value */
    uint16_t            adc_w;                  /**< W Phase current AD sampling value */
    uint16_t            adc_u_middle;           /**< u Phase current midpoint AD sampling value */
    uint16_t            adc_v_middle;           /**< v Phase current midpoint AD sampling value */
    uint16_t            adc_w_middle;           /**< w Phase current midpoint AD sampling value */
    HPM_MOTOR_MATH_TYPE       cal_u;            /**< Calculated U-phase current */
    HPM_MOTOR_MATH_TYPE       cal_v;            /**< Calculated V-phase current */
    HPM_MOTOR_MATH_TYPE       cal_w;            /**< Calculated W-phase current */
    void *userdata;                             /**< user data */
    void (*func_sampl)(void *str);                       /**< current samples */
} BLDC_CONTROL_CURRENT_PARA;
#define BLDC_CONTROL_CURRENT_PARA_DEFAULTS {0, 0, 0,\
                                            0, 0, 0,\
                                            0, 0, 0,\
                                            NULL, NULL}

/**
 * @brief PWM output parameters;pwm结构体
 *
 */
typedef struct bldc_control_pwmout_par {
    uint8_t             i_motor_id;           /**< Motor id @ref BLDC_MOTOR0_INDEX ... BLDC_MOTOR3_INDEX */
    uint8_t             i_sync_id;            /**< Synchronization id */
    uint32_t            pwm_u;                /**< u pwm duty cycle */
    uint32_t            pwm_v;                /**< v pwm duty cycle */
    uint32_t            pwm_w;                /**< w pwm duty cycle */
    uint32_t            i_pwm_reload;         /**< pwm reload value, pwm configuration related */
    void (*func_set_pwm)(void *str);                   /**< pwm output function */
} BLDC_CONTROL_PWMOUT_PARA;
#define BLDC_CONTROL_PWMOUT_PARA_DEFAULTS {0, 0, 0,\
                                            0, 0, 0,\
                                            NULL}
/**
 * @brief svpwm parameters;svpwm结构体
 *
 */
typedef struct bldc_control_pwm_par {
    HPM_MOTOR_MATH_TYPE       target_alpha; /**< alpha voltage */
    HPM_MOTOR_MATH_TYPE       target_beta;  /**< beta voltage */
    int8_t             sector;             /**< Sector Number */
    uint32_t            i_pwm_reload_max;       /**< Maximum duty cycle the pwm module can output */
    BLDC_CONTROL_PWMOUT_PARA    pwmout; /**< @ref BLDC_CONTROL_PWMOUT_PARA */
    void (*func_spwm)(void *str);                    /**< svpwm function */
} BLDC_CONTROL_PWM_PARA;
#define BLDC_CONTROL_PWM_PARA_DEFAULTS {0, 0, 0, 0,\
                                           BLDC_CONTROL_PWMOUT_PARA_DEFAULTS,\
                                            NULL}
/**
 * @brief Location estimation function;位置估算函数
 *
 */
typedef struct bldc_func_cal {
    void *par;
    void (*func)(void *str);
} BLDC_FUNC_CAL;

#define BLDC_FUNC_CAL_DEFAULTS {NULL, NULL}

/**
 * @brief foc control;FOC控制结构体
 *
 */
typedef struct bldc_contrl_foc_par {
    BLDC_CONTRL_PID_PARA        currentdpipar;             /**< D-axis current pi parameters */
    BLDC_CONTRL_PID_PARA        currentqpipar;             /**< Q-axis current pi parameters */
    BLDC_CONTRL_SPD_PARA        speedcalpar;               /**< Speed calculation parameters */
    HPM_MOTOR_MATH_TYPE         electric_angle;            /**< Electric angle */
    BLDC_CONTROL_CURRENT_PARA   samplcurpar;               /**< Sampling current */
    hpm_motor_para_t             motorpar;                  /**< Motor parameters */
    BLDC_CONTROL_PWM_PARA       pwmpar;                    /**< PWM parameters */
    BLDC_FUNC_CAL               pos_estimator_par;         /**< Null pointers do not run the position estimation algorithm, pointers are assigned for position estimation */
    HPM_MOTOR_MATH_TYPE               ualpha;             /**< alpha voltage */
    HPM_MOTOR_MATH_TYPE               ubeta;              /**< beta voltage */
    HPM_MOTOR_MATH_TYPE               ialpha;             /**< alpha current */
    HPM_MOTOR_MATH_TYPE               ibeta;              /**< beta current */
    void (*func_dqsvpwm)(void *str/*, void *str1, void *str2, void *data8*/);                               /**< dq axis current to svpwm function */
} BLDC_CONTROL_FOC_PARA;
#define BLDC_CONTROL_FOC_PARA_DEFAULTS {BLDC_CONTRL_PID_PARA_DEFAULTS, BLDC_CONTRL_PID_PARA_DEFAULTS,\
                                        BLDC_CONTRL_SPD_PARA_DEFAULTS, 0,\
                                        BLDC_CONTROL_CURRENT_PARA_DEFAULTS, BLDC_MOTOR_PARA_DEFAULTS,\
                                        BLDC_CONTROL_PWM_PARA_DEFAULTS,\
                                        BLDC_FUNC_CAL_DEFAULTS,\
                                        0, 0, 0, 0, NULL}


/** @} */
#endif
