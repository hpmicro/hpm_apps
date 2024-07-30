/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "bldc_foc_callback.h"
 
void pwm_cmp_force_value_step(PWM_Type *pwm_x, uint8_t index, uint32_t cmp)
{
    pwm_x->CMP[index] = PWM_CMP_CMP_SET(cmp);
}
void pwm_update_raw_cmp_central_aligned_step(PWM_Type *pwm_x, uint8_t cmp1_index,
                                       uint8_t cmp2_index, uint32_t target_cmp1, uint32_t target_cmp2)
{
    uint32_t reload = PWM_RLD_RLD_GET(pwm_x->RLD);
    if (!target_cmp1) {
        target_cmp1 = reload + 1;
    }
    if (!target_cmp2) {
        target_cmp2 = reload + 1;
    }

    pwm_cmp_force_value_step(pwm_x, cmp1_index, target_cmp1);
    pwm_cmp_force_value_step(pwm_x, cmp2_index, target_cmp2);
   
}

void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par)
{
  uint32_t pwm_reload;
  uint32_t pwm_u_half, pwm_v_half, pwm_w_half;

  pwm_reload = par->i_pwm_reload >> 1;
  switch (par->i_motor_id){
    case BLDC_MOTOR0_INDEX:
        pwm_u_half =  par->pwm_u >> 1;
        pwm_v_half =  par->pwm_v >> 1;
        pwm_w_half =  par->pwm_w >> 1;

       
        pwm_cmp_force_value(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload + pwm_u_half)));
        pwm_cmp_force_value(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload - pwm_u_half)));
        pwm_cmp_force_value(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_2, PWM_CMP_CMP_SET((pwm_reload + pwm_v_half)));
        pwm_cmp_force_value(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_3, PWM_CMP_CMP_SET((pwm_reload - pwm_v_half)));
        pwm_cmp_force_value(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_4, PWM_CMP_CMP_SET((pwm_reload + pwm_w_half)));
        pwm_cmp_force_value(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_5, PWM_CMP_CMP_SET((pwm_reload - pwm_w_half)));
        pwm_issue_shadow_register_lock_event(BOARD_PMSM0PWM);
    break;
   //case BLDC_MOTOR1_INDEX:
   //     pwm_u_half =  par->pwm_u >> 1;
   //     pwm_v_half =  par->pwm_v >> 1;
   //     pwm_w_half =  par->pwm_w >> 1;
   //     pwm_cmp_force_value(BOARD_PMSM1PWM, BOARD_PMSM1PWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload + pwm_u_half)));
   //     pwm_cmp_force_value(BOARD_PMSM1PWM, BOARD_PMSM1PWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload - pwm_u_half)));
   //     pwm_cmp_force_value(BOARD_PMSM1PWM, BOARD_PMSM1PWM_CMP_INDEX_2, PWM_CMP_CMP_SET((pwm_reload + pwm_v_half)));
   //     pwm_cmp_force_value(BOARD_PMSM1PWM, BOARD_PMSM1PWM_CMP_INDEX_3, PWM_CMP_CMP_SET((pwm_reload - pwm_v_half)));
   //     pwm_cmp_force_value(BOARD_PMSM1PWM, BOARD_PMSM1PWM_CMP_INDEX_4, PWM_CMP_CMP_SET((pwm_reload + pwm_w_half)));
   //     pwm_cmp_force_value(BOARD_PMSM1PWM, BOARD_PMSM1PWM_CMP_INDEX_5, PWM_CMP_CMP_SET((pwm_reload - pwm_w_half)));
   //     pwm_issue_shadow_register_lock_event(BOARD_PMSM1PWM);
   // break;
    //case BLDC_MOTOR2_INDEX:
    //    pwm_u_half =  par->pwm_u >> 1;
    //    pwm_v_half =  par->pwm_v >> 1;
    //    pwm_w_half =  par->pwm_w >> 1;
    //    pwm_cmp_force_value(BOARD_PMSM2PWM, BOARD_PMSM2PWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload + pwm_u_half)));
    //    pwm_cmp_force_value(BOARD_PMSM2PWM, BOARD_PMSM2PWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload - pwm_u_half)));
    //    pwm_cmp_force_value(BOARD_PMSM2PWM, BOARD_PMSM2PWM_CMP_INDEX_2, PWM_CMP_CMP_SET((pwm_reload + pwm_v_half)));
    //    pwm_cmp_force_value(BOARD_PMSM2PWM, BOARD_PMSM2PWM_CMP_INDEX_3, PWM_CMP_CMP_SET((pwm_reload - pwm_v_half)));
    //    pwm_cmp_force_value(BOARD_PMSM2PWM, BOARD_PMSM2PWM_CMP_INDEX_4, PWM_CMP_CMP_SET((pwm_reload + pwm_w_half)));
    //    pwm_cmp_force_value(BOARD_PMSM2PWM, BOARD_PMSM2PWM_CMP_INDEX_5, PWM_CMP_CMP_SET((pwm_reload - pwm_w_half)));
    //    pwm_issue_shadow_register_lock_event(BOARD_PMSM2PWM);
    //break;
    //case BLDC_MOTOR3_INDEX:
    //    pwm_u_half =  par->pwm_u >> 1;
    //    pwm_v_half =  par->pwm_v >> 1;
    //    pwm_w_half =  par->pwm_w >> 1;
    //    pwm_cmp_force_value(BOARD_PMSM3PWM, BOARD_PMSM3PWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload + pwm_u_half)));
    //    pwm_cmp_force_value(BOARD_PMSM3PWM, BOARD_PMSM3PWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload - pwm_u_half)));
    //    pwm_cmp_force_value(BOARD_PMSM3PWM, BOARD_PMSM3PWM_CMP_INDEX_2, PWM_CMP_CMP_SET((pwm_reload + pwm_v_half)));
    //    pwm_cmp_force_value(BOARD_PMSM3PWM, BOARD_PMSM3PWM_CMP_INDEX_3, PWM_CMP_CMP_SET((pwm_reload - pwm_v_half)));
    //    pwm_cmp_force_value(BOARD_PMSM3PWM, BOARD_PMSM3PWM_CMP_INDEX_4, PWM_CMP_CMP_SET((pwm_reload + pwm_w_half)));
    //    pwm_cmp_force_value(BOARD_PMSM3PWM, BOARD_PMSM3PWM_CMP_INDEX_5, PWM_CMP_CMP_SET((pwm_reload - pwm_w_half)));
    //    pwm_issue_shadow_register_lock_event(BOARD_PMSM3PWM);
    //break;
  default:
    break;
  }
}
void disable_all_pwm_output(PWM_Type *ptr)
{
   /*force pwm*/
    if ((ptr == HPM_PWM0))
    {
        pwm_config_force_cmd_timing(BOARD_PMSM0PWM, pwm_force_immediately);
        pwm_enable_pwm_sw_force_output(BOARD_PMSM0PWM, BOARD_PMSM0_UH_PWM_OUTPIN);
        pwm_enable_pwm_sw_force_output(BOARD_PMSM0PWM, BOARD_PMSM0_UL_PWM_OUTPIN);
        pwm_enable_pwm_sw_force_output(BOARD_PMSM0PWM, BOARD_PMSM0_VH_PWM_OUTPIN);
        pwm_enable_pwm_sw_force_output(BOARD_PMSM0PWM, BOARD_PMSM0_VL_PWM_OUTPIN);
        pwm_enable_pwm_sw_force_output(BOARD_PMSM0PWM, BOARD_PMSM0_WH_PWM_OUTPIN);
        pwm_enable_pwm_sw_force_output(BOARD_PMSM0PWM, BOARD_PMSM0_WL_PWM_OUTPIN);
        pwm_set_force_output(BOARD_PMSM0PWM,
                            PWM_FORCE_OUTPUT(BOARD_PMSM0_UH_PWM_OUTPIN, pwm_output_0)
                            | PWM_FORCE_OUTPUT(BOARD_PMSM0_UL_PWM_OUTPIN, pwm_output_0)
                            | PWM_FORCE_OUTPUT(BOARD_PMSM0_VH_PWM_OUTPIN, pwm_output_0)
                            | PWM_FORCE_OUTPUT(BOARD_PMSM0_VL_PWM_OUTPIN, pwm_output_0)
                            | PWM_FORCE_OUTPUT(BOARD_PMSM0_WH_PWM_OUTPIN, pwm_output_0)
                            | PWM_FORCE_OUTPUT(BOARD_PMSM0_WL_PWM_OUTPIN, pwm_output_0));
        pwm_enable_sw_force(BOARD_PMSM0PWM);
    }
    //else if ((ptr == HPM_PWM1))
    //{
    //    pwm_config_force_cmd_timing(BOARD_PMSM1PWM, pwm_force_immediately);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM1PWM, BOARD_PMSM1_UH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM1PWM, BOARD_PMSM1_UL_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM1PWM, BOARD_PMSM1_VH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM1PWM, BOARD_PMSM1_VL_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM1PWM, BOARD_PMSM1_WH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM1PWM, BOARD_PMSM1_WL_PWM_OUTPIN);
    //    pwm_set_force_output(BOARD_PMSM1PWM,
    //                        PWM_FORCE_OUTPUT(BOARD_PMSM1_UH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM1_UL_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM1_VH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM1_VL_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM1_WH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM1_WL_PWM_OUTPIN, pwm_output_0));

    //    pwm_enable_sw_force(BOARD_PMSM1PWM);
    //}
    //    else if ((ptr == HPM_PWM2))
    //{
    //    pwm_config_force_cmd_timing(BOARD_PMSM2PWM, pwm_force_immediately);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM2PWM, BOARD_PMSM2_UH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM2PWM, BOARD_PMSM2_UL_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM2PWM, BOARD_PMSM2_VH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM2PWM, BOARD_PMSM2_VL_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM2PWM, BOARD_PMSM2_WH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM2PWM, BOARD_PMSM2_WL_PWM_OUTPIN);
    //    pwm_set_force_output(BOARD_PMSM2PWM,
    //                        PWM_FORCE_OUTPUT(BOARD_PMSM2_UH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM2_UL_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM2_VH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM2_VL_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM2_WH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM2_WL_PWM_OUTPIN, pwm_output_0));
    //    pwm_enable_sw_force(BOARD_PMSM2PWM);
    //}
    //else
    //{
    //    pwm_config_force_cmd_timing(BOARD_PMSM3PWM, pwm_force_immediately);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM3PWM, BOARD_PMSM3_UH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM3PWM, BOARD_PMSM3_UL_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM3PWM, BOARD_PMSM3_VH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM3PWM, BOARD_PMSM3_VL_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM3PWM, BOARD_PMSM3_WH_PWM_OUTPIN);
    //    pwm_enable_pwm_sw_force_output(BOARD_PMSM3PWM, BOARD_PMSM3_WL_PWM_OUTPIN);
    //    pwm_set_force_output(BOARD_PMSM3PWM,
    //                        PWM_FORCE_OUTPUT(BOARD_PMSM3_UH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM3_UL_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM3_VH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM3_VL_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM3_WH_PWM_OUTPIN, pwm_output_0)
    //                        | PWM_FORCE_OUTPUT(BOARD_PMSM3_WL_PWM_OUTPIN, pwm_output_0));
    //    pwm_enable_sw_force(BOARD_PMSM3PWM);
    //}
}
void enable_all_pwm_output(PWM_Type *ptr)
{
   /*force pwm*/
    if ((ptr == HPM_PWM0))
    {
        pwm_disable_sw_force(BOARD_PMSM0PWM);
    }
    //else if ((ptr == HPM_PWM1))
    //{
    //    pwm_disable_sw_force(BOARD_PMSM1PWM);
    //}
    //    else if ((ptr == HPM_PWM2))
    //{
    //    pwm_disable_sw_force(BOARD_PMSM2PWM);
    //}
    //else
    //{
    //    pwm_disable_sw_force(BOARD_PMSM3PWM);
    //}
}