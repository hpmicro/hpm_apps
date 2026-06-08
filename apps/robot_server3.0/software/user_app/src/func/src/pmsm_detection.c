/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pmsm_detection.h"
#include "bldc_foc_callback.h"

void motor_param_detection_cfg(BLDC_CONTROL_DETECTION_PARA* dection_para,MOTOR_PARA *par)
{
    dection_para->detection_t.cfg.detection_loop_ts = par->foc_para.motorpar.i_samplingper_s;//电流环运行时间
    dection_para->detection_t.cfg.inductor_detection_times = 20;
    dection_para->detection_t.cfg.delay_times = 200 ;
    dection_para->detection_t.cfg.vbus = par->foc_para.motorpar.i_phasevol_v;
    dection_para->detection_t.cfg.Ts_RLD = PWM_RELOAD;
}

void  motor_param_detection_loop(BLDC_CONTROL_DETECTION_PARA* dection_para,MOTOR_PARA *par)
{
    HPM_MOTOR_MATH_TYPE  sin_angle = 0;
    HPM_MOTOR_MATH_TYPE  cos_angle = 0;
    HPM_MOTOR_MATH_TYPE  alpha = 0;
    HPM_MOTOR_MATH_TYPE  beta = 0;
    HPM_MOTOR_MATH_TYPE  cal_u,cal_v,cal_w = 0;
    HPM_MOTOR_MATH_TYPE  elec_angle, ud_ref,uq_ref;
    par->foc_para.samplcurpar.func_sampl(&par->foc_para.samplcurpar);
    cal_u= par->foc_para.samplcurpar.cal_u * ADC_REF_VOL / (SAMPLE_PRECISION * OPAMP_GAIN * MOTOR_Rs);//转化为模拟量 放大倍数10，电阻1mΩ
    cal_v = par->foc_para.samplcurpar.cal_v * ADC_REF_VOL / (SAMPLE_PRECISION * OPAMP_GAIN * MOTOR_Rs);
    cal_w = par->foc_para.samplcurpar.cal_w * ADC_REF_VOL / (SAMPLE_PRECISION * OPAMP_GAIN * MOTOR_Rs);
   ////CLARK变换
   alpha = cal_u;
   beta = (SQRT3_DIV3 * HPM_MOTOR_MATH_MDF_FL(cal_u)) + (SQRT3_DIV3 * 2 * HPM_MOTOR_MATH_MDF_FL(cal_v));


   elec_angle = 0;
   sin_angle = sinf(elec_angle);
   cos_angle = cosf(elec_angle);
   dection_para->detection_t.common_use.i_alpha = alpha;
   dection_para->detection_t.common_use.i_beta = beta;
    
   dection_para->detection_t.common_use.ud_ref = 0;
   dection_para->detection_t.common_use.uq_ref = 0;

    switch(dection_para->detection_t.mode)
    {
        case offline_param_detection_mode_init:
              detection_init(&dection_para->detection_t);
              dection_para->detection_t.mode = offline_param_detection_mode_wait;
              dection_para->detection_t.last_mode = offline_param_detection_mode_init;

        break;
        case offline_param_detection_mode_wait:
               dection_para->detection_t.status = 0;
               dection_para->detection_t.tick_count++;
                  if(dection_para->detection_t.tick_count > dection_para->detection_t.cfg.delay_times)
                  {
                       dection_para->detection_t.tick_count = 0;
                       switch(dection_para->detection_t.last_mode)
                       {
                            case offline_param_detection_mode_init:
                                  dection_para->detection_t.mode = offline_param_detection_mode_rs;
                                  dection_para->detection_t.last_mode = offline_param_detection_mode_wait;
                            break;
                            case offline_param_detection_mode_rs:
                                  dection_para->detection_t.mode = offline_param_detection_mode_ld;
                                  dection_para->detection_t.last_mode = offline_param_detection_mode_wait;
                            break;
                            case offline_param_detection_mode_ld:
                                  dection_para->detection_t.mode = offline_param_detection_mode_lq;
                                  dection_para->detection_t.last_mode = offline_param_detection_mode_wait;
                            break;
                            case offline_param_detection_mode_lq:
                                  dection_para->detection_t.mode = offline_param_detection_mode_success;
                                  dection_para->detection_t.last_mode = offline_param_detection_mode_wait;
                            break;
               
                          default:
                                  dection_para->detection_t.mode = offline_param_detection_mode_init;
                                  dection_para->detection_t.last_mode = offline_param_detection_mode_wait;
                            break;
                       }
                }
                 break;
        case offline_param_detection_mode_rs:
            if(dection_para->detection_t.status == success)            
              {
                  disable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.last_mode = offline_param_detection_mode_rs;
                  dection_para->detection_t.mode = offline_param_detection_mode_wait;
                     
                  
              }
              else if(dection_para->detection_t.status == fail)
              {
                  disable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.mode = offline_param_detection_mode_error;
                  
              }
              else
              {   
                  enable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.status = Rs_detection_pro(&dection_para->detection_t);
              }
              
        break;
        case offline_param_detection_mode_ld:
              if(dection_para->detection_t.status == success)            
              {
                  disable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.last_mode = offline_param_detection_mode_ld;
                  dection_para->detection_t.mode = offline_param_detection_mode_wait; 

              }
              else if(dection_para->detection_t.status == fail)
              {
                  disable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.mode = offline_param_detection_mode_error;
                  
              }
              else
              {   
                  enable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.status = Ld_detection_pro(&dection_para->detection_t);
              }
              
        break;
        case offline_param_detection_mode_lq:
              if(dection_para->detection_t.status == success)            
              {
                  disable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.last_mode = offline_param_detection_mode_lq;
                  dection_para->detection_t.mode = offline_param_detection_mode_wait; 
              }
              else if(dection_para->detection_t.status == fail)
              {
                  disable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.mode = offline_param_detection_mode_error;              
              }
              else
              {   
                  enable_all_pwm_output(BOARD_PMSM0PWM);
                  dection_para->detection_t.status = Lq_detection_pro(&dection_para->detection_t);
              }
        break;
        case offline_param_detection_mode_error:
        break;
        case offline_param_detection_mode_success:////参数自整定
        pi_params_get(dection_para);
        break;
        default:
             dection_para->detection_t.mode = offline_param_detection_mode_init;
        break;
    } 
      ud_ref = dection_para->detection_t.common_use.ud_ref * dection_para->detection_t.cfg.Ts_RLD * SQRT3 / dection_para->detection_t.cfg.vbus;
      uq_ref = dection_para->detection_t.common_use.uq_ref * dection_para->detection_t.cfg.Ts_RLD * SQRT3 / dection_para->detection_t.cfg.vbus;
      bldc_foc_inv_park(ud_ref, uq_ref, &alpha, &beta, sin_angle, cos_angle);
      par->foc_para.pwmpar.target_alpha = alpha;
      par->foc_para.pwmpar.target_beta = beta;
      bldc_foc_svpwm(&par->foc_para.pwmpar);
      bldc_foc_pwmset(&par->foc_para.pwmpar.pwmout);     
}


void pi_params_get(BLDC_CONTROL_DETECTION_PARA* dection_para)
{
    uint32_t bandwith = CURRENLOOP_BANDWITH;//带宽2k
    dection_para->detection_t.pi_par.kp_q = dection_para->detection_t.result.lq * bandwith;
    dection_para->detection_t.pi_par.kp_d = dection_para->detection_t.result.ld * bandwith;
    float Ts = dection_para->detection_t.cfg.detection_loop_ts;//电流环运行时间
    dection_para->detection_t.pi_par.ki_q = dection_para->detection_t.result.rs * Ts / dection_para->detection_t.result.lq;
    dection_para->detection_t.pi_par.ki_d = dection_para->detection_t.result.rs * Ts / dection_para->detection_t.result.ld;
    dection_para->detection_t.pi_par.i_max = dection_para->detection_t.cfg.Ts_RLD;
} 
