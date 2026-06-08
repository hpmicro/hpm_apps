///*
// * Copyright (c) 2022 hpmicro
// *
// * SPDX-License-Identifier: BSD-3-Clause
// *
// */
//#ifndef HPM_STEPPER_MOTOR_H
//#define HPM_STEPPER_MOTOR_H

//#if defined(__cplusplus)
//extern "C" {
//#endif /* __cplusplus */

//#include "hpm_motor_math.h"

//#define STEPPER_MOTOR0_INDEX                (1)
//#define STEPPER_MOTOR1_INDEX                (2)
//#define STEPPER_MOTOR2_INDEX                (3)
//#define STEPPER_MOTOR3_INDEX                (4)

//#define STEPPER_MOTOR0_PWM_T                (20)
//#define STEPPER_MOTOR1_PWM_T                (20)
//#define STEPPER_MOTOR2_PWM_T                (20)
//#define STEPPER_MOTOR3_PWM_T                (20)
//extern void stepper_nullcallback_func(void);
///*电机相关结构体定义,请遵守命名规则，物理性性质描述定义：输入（I）/输出（O）_物理量描述_物理量单位*/
//typedef struct stepper_motor_par{
//    /*输入参数*/
//    float  I_Rstator_ohm; 	        /* 例：输入：定子电阻 (单位ohm)（下同省略） */
//    float  I_Poles_n;               /*极对数*/
//    float  I_MaxSpeed_rs;           /*最大转速  r/s*/
//    float  I_Lstator_h;				      /* 定子电感 */	  			      
//    float  I_PhaseCur_a; 			      /* 额定电流 */
//    float  I_PhaseVol_v;			      /* 额定电压 */
//    float  I_SamplingPer_s;			    /* 电流采样周期*/
//    float  I_step_angle_phys;                /* step angle physics*/
//    float  I_step_angle_subdiv;             /* Angle after subdivision of physical step angle */
//    uint16_t I_subdivision;               /* subdivision */
//    uint32_t I_step_totalnum;                  /*Subdivision of steps in the latter round*/

//}STEPPER_MOTOR_PARA;

//#define STEPPER_MOTOR_PARA_DEFAULTS {0, 0, 0,\
//                                        0, 0, 0,\
//                                        0, 0, 0, 0, 0}

///*速度计算参数*/
//typedef struct  stepper_contrl_spd_par{
//    float target_speed;                           /* Target speed step/s */
//    uint32_t interrupt_freqc;                     /* Hz */
//    int64_t angle_int;                          /* Used to perform modulo operations */
//    float angle_float;
//    float step_total;                               /* Number of running steps */
//    float step_total_mid;                               /* Number of middle running steps */
//    HPM_MOTOR_MATH_TYPE angle_rad;                    /* output angle rad*/
//    HPM_MOTOR_MATH_TYPE angle_deg;                    /* output angle degree*/
//    HPM_MOTOR_MATH_TYPE angle_current;                    /* output angle degree*/
//    uint8_t model;      /** speed model, position model */

//    void    (*func_setangle)();                     /* angle arithmetic function */
//}STEPPER_CONTRL_ANGLE_PARA;

//#define STEPPER_CONTRL_ANGLE_PARA_DEFAULTS {0,0,0,0,0,0, 0, 0, 0, 0,\
//                                        &stepper_nullcallback_func}
///*针对所有pid控制*/
//typedef struct stepper_contrl_pid_par{
//    HPM_MOTOR_MATH_TYPE       I_kp;                   /*Kp*/
//    HPM_MOTOR_MATH_TYPE       I_ki;                   /*Ki*/
//    HPM_MOTOR_MATH_TYPE       I_kd;                   /*Kd*/
//    HPM_MOTOR_MATH_TYPE       I_kc;                   /*Kc*/
//    HPM_MOTOR_MATH_TYPE       SatErr;
//    HPM_MOTOR_MATH_TYPE       I_switch_pi_p;          /* pi-controlled and p-controlled switching nodes */
//    HPM_MOTOR_MATH_TYPE       I_max;                  /*最大值  默认min = -max*/
//    HPM_MOTOR_MATH_TYPE       target;                 /*目标*/
//    HPM_MOTOR_MATH_TYPE       mem;                    /*内部运算使用*/
//    HPM_MOTOR_MATH_TYPE       cur;                    /*采集*/
//    HPM_MOTOR_MATH_TYPE       outval;                 /*输出*/
//    void (*func_pid)();                         /*Pid控制程序*/
//}STEPPER_CONTRL_PID_PARA;
//#define STEPPER_CONTRL_PID_PARA_DEFAULTS {0,0,0,0,0,0,\
//                                        0,0,0,0,0,\
//                                       &stepper_nullcallback_func}
///*电流采样参数*/
//typedef struct stepper_contrl_current_par{
//    uint16_t            adc_a; 
//    uint16_t            adc_b;
//    uint16_t            adc_a_middle; 
//    uint16_t            adc_b_middle;
//    int16_t       cal_a;
//    int16_t       cal_b;
//    void*               userdata;
//    void (*func_sampl)(); 
//}STEPPER_CONTROL_CURRENT_PARA;
//#define STEPPER_CONTROL_CURRENT_PARA_DEFAULTS {0,0,0,\
//                                            0,0,0,\
//                                            NULL,&stepper_nullcallback_func}
///*pwm*/
//typedef struct stepper_control_pwmout_par{
//    uint8_t             I_motor_id;                 /*电机id*/
//    uint8_t             I_sync_id;            /*同步id*/
//    uint32_t            pwm_a0;
//    uint32_t            pwm_a1;
//    uint32_t            pwm_b0;
//    uint32_t            pwm_b1;
//    uint32_t            I_pwm_reload;
//    void (*func_set_pwm)();                 /*指向输出pwm函数*/
//}STEPPER_CONTROL_PWMOUT_PARA;
//#define STEPPER_CONTROL_PWMOUT_PARA_DEFAULTS {0,0,0,\
//                                            0,0,0,\
//                                            0,\
//                                            &stepper_nullcallback_func}
//typedef struct stepper_control_pwm_par{
//    HPM_MOTOR_MATH_TYPE       target_alpha;
//    HPM_MOTOR_MATH_TYPE       target_beta;
//    uint32_t            I_pwm_reload_max;       /*最大占空比*/
//    int16_t     sector; /** sector num */
//    STEPPER_CONTROL_PWMOUT_PARA    pwmout;
//    void (*func_spwm)();                    /*指向处理函数*/
//}STEPPER_CONTROL_PWM_PARA;
//#define STEPPER_CONTROL_PWM_PARA_DEFAULTS {0,0,0,0,\
//                                           STEPPER_CONTROL_PWMOUT_PARA_DEFAULTS,\
//                                            &stepper_nullcallback_func}

///**
// * @brief Low-pass filter parameters
// * 
// */
//typedef struct stepper_control_lpf_par{
//    float filter_cycle;                     /** Filtering cycle, Units: s */
//    float cur_off_freqc;    /** Cut-off frequency hz*/
//    float k;                /** Filter factor, which need to be calculated by calling the initialization */
//    int32_t input_current;     /** Input current */
//    int32_t output_current;   /** Filtered current, this variable needs to be used iteratively */
//    void (*func_init)();    /** init para */
//    void (*func_lpf)();     /** Low-pass filter function */
//}STEPPER_CONTROL_LPF_PARA;
//void hpm_stepper_lpf_init(STEPPER_CONTROL_LPF_PARA * lpf);
//#define STEPPER_CONTROL_LPF_PARA_DEFAULTS {0,0,0,0,0,\
//                                            &hpm_stepper_lpf_init,\
//                                            &stepper_nullcallback_func}
///**
// * @brief sigmoid speed control
// * 
// */
//typedef struct stepper_control_sigmoid_par {
//    float Ka_max;   /** Acceleration max*/
//    float Ka_T;     /** Average acceleration, which determines the acceleration time t = (v1 - v0)/a*/
//    float mid;      /** s-curve midpoint, default is 0 */
//    float v1;       /** end speed */
//    float vmax;     /** max speed, position control use */
//    float v0;       /** start speed */
//    float v_mid;    /** mid speed */
//    float output_v;      /** target speed */
//    float err_v;    /** Permissible speed error */
//    uint8_t v_zero_flag; /** Speed over zero flag bit 1: over zero, 0: but zero */
//    uint32_t freqc; /** Operating cycle */
//    int32_t t_total;        /** Total number of period */
//    int32_t t_current;     /** current number of period */
//    float t_step_time;  /** unit : s */
//    uint8_t working;    /** work in progress , 1: working 0: idle*/
//    void (*func_curve)();
//}STEPPER_CONTROL_SIGMOID_PAR;
//#define STEPPER_CONTROL_SIGMOID_PARA_DEFAULTS {\
//    0, 0, 0, 0, 0, 0, 0,\
//    0, 0, 0, 0, 0, 0, 0, 0,\
//    &stepper_nullcallback_func \
//}

///**
// * @brief Step count
// *
// */
//typedef struct stepper_func_cal{
//    void *par;
//    void (*func)();
//} STEPPER_FUNC_CAL;
//#define STEPPER_FUNC_CAL_DEFAULTS {NULL, &stepper_nullcallback_func}

//typedef struct current_tracking_unit {
//    float speed;
//    HPM_MOTOR_MATH_TYPE current;
//} CURRENT_TRACKING_UNIT;

//typedef struct stepper_current_tracking_par {
//    CURRENT_TRACKING_UNIT * curves;
//    HPM_MOTOR_MATH_TYPE current_speed;
//    uint8_t curve_length; /** length >= 3 */
//    uint16_t pos; /* Current location */
//    uint16_t given; /* Given torque */
//    void (*func)();
//} STEPPER_CURRENT_TRACKING_PAR;
//#define STEPPER_CURRENT_TRACKING_PARA_DEFAULTS {\
//    NULL,\
//    0, 0, 0, 0, \
//    &stepper_nullcallback_func \
//}

///*foc控制*/
//typedef struct stepper_contrl_foc_par{
//    STEPPER_CONTRL_PID_PARA        currentdpipar;             /*D轴电流*/
//    STEPPER_CONTRL_PID_PARA        currentqpipar;             /*Q轴电流*/
//    STEPPER_CONTRL_ANGLE_PARA        anglecalpar;                  /*速度参数*/
//    HPM_MOTOR_MATH_TYPE            electric_angle;             /*当前电角度*/
//    STEPPER_CONTROL_CURRENT_PARA   samplcurpar;                    /*采样电流*/
//    STEPPER_MOTOR_PARA             motorpar;                   /*电机参数*/
//    STEPPER_CONTROL_PWM_PARA       pwmpar;                     /*pwm参数*/
//    HPM_MOTOR_MATH_TYPE               u_alpha;
//    HPM_MOTOR_MATH_TYPE               u_beta;
//    HPM_MOTOR_MATH_TYPE               i_alpha;
//    HPM_MOTOR_MATH_TYPE               i_beta;
//    STEPPER_CONTROL_LPF_PARA       lpfa;                        /** low-pass filter */
//    STEPPER_CONTROL_LPF_PARA       lpfb;                        /** low-pass filter */
//    STEPPER_CONTROL_SIGMOID_PAR     s_curve;                      /** S acceleration/deceleration curve profile */
//    STEPPER_CURRENT_TRACKING_PAR    v_d_curve;                  /** Current velocity curve */
//    uint8_t                        motor_status;                /** motor status 0: running, 1: stop*/
//    void (*func_loop)();
//}STEPPER_CONTROL_FOC_PARA;
//#define STEPPER_CONTROL_FOC_PARA_DEFAULTS {STEPPER_CONTRL_PID_PARA_DEFAULTS,STEPPER_CONTRL_PID_PARA_DEFAULTS,\
//                                        STEPPER_CONTRL_ANGLE_PARA_DEFAULTS,0,\
//                                        STEPPER_CONTROL_CURRENT_PARA_DEFAULTS,STEPPER_MOTOR_PARA_DEFAULTS,\
//                                        STEPPER_CONTROL_PWM_PARA_DEFAULTS,\
//                                        0,0,0,0,\
//                                        STEPPER_CONTROL_LPF_PARA_DEFAULTS, \
//                                        STEPPER_CONTROL_LPF_PARA_DEFAULTS, \
//                                        STEPPER_CONTROL_SIGMOID_PARA_DEFAULTS, \
//                                        STEPPER_CURRENT_TRACKING_PARA_DEFAULTS, \
//                                        0,\
//                                        &stepper_nullcallback_func}

//void hpm_stepper_angle_generator_by_speed(STEPPER_CONTROL_FOC_PARA *stepper);
//void hpm_stepper_pi_controller(STEPPER_CONTRL_PID_PARA * pi);
//void hpm_stepper_svpwm(STEPPER_CONTROL_PWM_PARA *par, bool dir);
//void hpm_stepper_pwmset(STEPPER_CONTROL_PWMOUT_PARA *par);
//void hpm_stepper_loop(STEPPER_CONTROL_FOC_PARA * par);
//void hpm_stepper_lpf(STEPPER_CONTROL_LPF_PARA * lpf);
//void hpm_current_cal(STEPPER_CONTROL_CURRENT_PARA * par);
//void hpm_stepper_angle_generator_by_step(STEPPER_CONTROL_FOC_PARA *stepper);

//float hpm_stepper_get_speed(STEPPER_CONTROL_FOC_PARA *stepper, float speed);
//float hpm_stepper_get_position(STEPPER_CONTROL_FOC_PARA *stepper, float pos);
//void hpm_stepper_sigmoid(STEPPER_CONTROL_SIGMOID_PAR * par);
//void hpm_stepper_current_tracking (STEPPER_CURRENT_TRACKING_PAR *par);
//void hpm_stepper_step_speed_cal(STEPPER_CONTROL_SIGMOID_PAR *par, float s, float *v1, float *t1);
//void hpm_stepper_pulse_time_of_step_speed_cal(STEPPER_CONTROL_SIGMOID_PAR *par, int32_t pluse, int32_t t_ms, 
//                                              float *step, float *v, float *t1);


//#if defined(__cplusplus)
//}
//#endif /* __cplusplus */

//#endif
