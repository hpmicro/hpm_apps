/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "board.h"
#include "pmsm_currentctrl_svpwm.h"
#include "hpm_pwmv2_drv.h"
#include "bldc_foc_cfg.h"
#include "pmsm_define.h"
#include "bldc_foc_callback.h"

const uint8_t pwm_uvw_conversion_tbl[4][6] ={
  {
    BOARD_BLDC_UH_PWM_OUTPIN,
    BOARD_BLDC_UL_PWM_OUTPIN,
    BOARD_BLDC_VH_PWM_OUTPIN,
    BOARD_BLDC_VL_PWM_OUTPIN,
    BOARD_BLDC_WH_PWM_OUTPIN,
    BOARD_BLDC_WL_PWM_OUTPIN 
  },
  {
    BOARD_BLDC_UH_PWM_OUTPIN,
    BOARD_BLDC_UL_PWM_OUTPIN,
    BOARD_BLDC_VH_PWM_OUTPIN,
    BOARD_BLDC_VL_PWM_OUTPIN,
    BOARD_BLDC_WH_PWM_OUTPIN,
    BOARD_BLDC_WL_PWM_OUTPIN 
  }
};

PWMV2_Type* motor_pwm_tbl[4] ={
  BOARD_STEPPER0PWM,
};


/**********************************************
 * *void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par)
 * * reflesh pwm cmp value
 * *@Input: Motor pwm structure
 * *#Output：None
 * ***************************************/
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
      
        pwmv2_shadow_register_unlock(BOARD_PMSM0PWM);
        pwmv2_set_shadow_val(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_0 + 1, (pwm_reload - pwm_u_half), 0, false);
        pwmv2_set_shadow_val(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_1 + 1, (pwm_reload + pwm_u_half), 0, false);
        pwmv2_set_shadow_val(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_2 + 1, (pwm_reload - pwm_v_half), 0, false);
        pwmv2_set_shadow_val(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_3 + 1, (pwm_reload + pwm_v_half), 0, false);
        pwmv2_set_shadow_val(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_4 + 1, (pwm_reload - pwm_w_half), 0, false);
        pwmv2_set_shadow_val(BOARD_PMSM0PWM, BOARD_PMSM0PWM_CMP_INDEX_5 + 1, (pwm_reload + pwm_w_half), 0, false);
        pwmv2_shadow_register_lock(BOARD_PMSM0PWM);
        
    break;

  default:
    break;
  }
}
/**********************************************
 * *void disable_all_pwm_output(PWMV2_Type *ptr)
 * * disable pwm output
 * *@Input:  pwmv2 structure
 * *#Output：None
 * ***************************************/
void disable_all_pwm_output(PWMV2_Type *ptr)
{
    pwmv2_set_force_update_time(BOARD_PMSM0PWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(BOARD_PMSM0PWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(BOARD_PMSM0PWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(BOARD_PMSM0PWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(BOARD_PMSM0PWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(BOARD_PMSM0PWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_immediately);

    pwmv2_force_update_time_by_shadow(BOARD_PMSM0PWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(BOARD_PMSM0PWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(BOARD_PMSM0PWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(BOARD_PMSM0PWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(BOARD_PMSM0PWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(BOARD_PMSM0PWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_update_shadow_immediately);

    pwmv2_enable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_enable_software_force(BOARD_PMSM0PWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_enable_software_force(BOARD_PMSM0PWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_enable_software_force(BOARD_PMSM0PWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_software_force(BOARD_PMSM0PWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_software_force(BOARD_PMSM0PWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_software_force(BOARD_PMSM0PWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_shadow_register_unlock(BOARD_PMSM0PWM);
    pwmv2_force_output(BOARD_PMSM0PWM, BOARD_BLDC_UH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(BOARD_PMSM0PWM, BOARD_BLDC_UL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(BOARD_PMSM0PWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(BOARD_PMSM0PWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(BOARD_PMSM0PWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(BOARD_PMSM0PWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_shadow_register_lock(BOARD_PMSM0PWM);
}
/**********************************************
 * *void enable_all_pwm_output(PWMV2_Type *ptr)
 * * enable pwm output
 * *@Input: pwmv2 structure
 * *#Output：None
 * ***************************************/
void enable_all_pwm_output(PWMV2_Type *ptr)
{

    pwmv2_disable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwmv2_disable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwmv2_disable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_disable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_disable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_disable_force_by_software(BOARD_PMSM0PWM, BOARD_BLDC_WL_PWM_OUTPIN);
}
