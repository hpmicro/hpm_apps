/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "pmsm_currentctrl.h"

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_PMSM0_ADC_SEQ_DMA_SIZE_IN_4BYTES];

void motor0_angle_align_loop(void)
{
    motor0.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_theta);
    motor0.foc_para.currentdpipar.target = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_IdRef);
    motor0.foc_para.currentqpipar.target = HPM_MOTOR_MATH_FL_MDF(0);
    motor0.foc_para.func_dqsvpwm(&motor0.foc_para,0,0,0);
    motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);
}

void motor0_highspeed_loop(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;
    uint32_t  pos;
    float fre_get_angle = 0.0;
    qei_phase_config(BOARD_PMSM0_QEI_BASE, BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV, qei_z_count_inc_on_phase_count_max, true);
    pos = qei_get_current_count(BOARD_PMSM0_QEI_BASE, qei_counter_type_phase)&0x1fffff;
    uint32_t enc_cnt = BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV/motor0.foc_para.motorpar.i_poles_n;
    fre_get_angle = 360-(((pos%enc_cnt)*360)/enc_cnt);
 

    motor0.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(fre_get_angle);
    if (1 == Motor_Control_Global.ControlWord[0])
    { 
      motor0.foc_para.func_dqsvpwm(&motor0.foc_para,0,0,0);
      motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);
    }
    else
    {

       motor0.foc_para.currentdpipar.target = 0;
       motor0.foc_para.currentqpipar.target = 0;
       motor0.foc_para.currentdpipar.mem = 0;
       motor0.foc_para.currentqpipar.mem = 0;
       motor0.foc_para.currentdpipar.cur = 0;
       motor0.foc_para.currentqpipar.cur = 0;
       disable_all_pwm_output(BOARD_PMSM0PWM);
    }
    motor0.foc_para.speedcalpar.speedtheta = motor0.foc_para.electric_angle;
    motor0.foc_para.speedcalpar.func_getspd(&motor0.foc_para.speedcalpar);
}


void motor0_isr_adc(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;

    status = pwm_get_status(BOARD_PMSM0PWM);
    if (PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP) == (status & PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP))) 
    {
        pwm_clear_status(BOARD_PMSM0PWM, status);
        motor0.adc_trig_event_callback();
    }

}
SDK_DECLARE_EXT_ISR_M(BOARD_PMSM0APP_PWM_IRQ, motor0_isr_adc)

void motor1_angle_align_loop(void)
{
    motor1.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM1_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor1.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM1_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor1.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM1_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor1.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_theta);
    motor1.foc_para.currentdpipar.target = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_IdRef);
    motor1.foc_para.currentqpipar.target = HPM_MOTOR_MATH_FL_MDF(0);
    motor1.foc_para.func_dqsvpwm(&motor1.foc_para,0,0,0);
    motor1.foc_para.pwmpar.pwmout.func_set_pwm(&motor1.foc_para.pwmpar.pwmout);
}

void motor1_highspeed_loop(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;
    uint32_t  pos;
    float fre_get_angle = 0.0;
    qei_phase_config(BOARD_PMSM1_QEI_BASE, BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV, qei_z_count_inc_on_phase_count_max, true);
    pos = qei_get_current_count(BOARD_PMSM1_QEI_BASE, qei_counter_type_phase)&0x1fffff;
    uint32_t enc_cnt = BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV/motor1.foc_para.motorpar.i_poles_n;
    fre_get_angle = 360-(((pos%enc_cnt)*360)/enc_cnt);
    motor1.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM1_ADC_TRG*4]&0xffff)>>4);
    motor1.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM1_ADC_TRG*4]&0xffff)>>4);
    motor1.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM1_ADC_TRG*4]&0xffff)>>4);
    motor1.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(fre_get_angle);
    if (1 == Motor_Control_Global.ControlWord[1])
    {
      motor1.foc_para.func_dqsvpwm(&motor1.foc_para,0,0,0);
      motor1.foc_para.pwmpar.pwmout.func_set_pwm(&motor1.foc_para.pwmpar.pwmout);
    }
    else
    {
       motor1.foc_para.currentqpipar.target = 0;
       motor1.foc_para.currentdpipar.mem = 0;
       motor1.foc_para.currentqpipar.mem = 0;
       motor1.foc_para.currentdpipar.cur = 0;
       motor1.foc_para.currentqpipar.cur = 0;
       disable_all_pwm_output(BOARD_PMSM1PWM);
    }
    motor1.foc_para.speedcalpar.speedtheta = motor1.foc_para.electric_angle;
    motor1.foc_para.speedcalpar.func_getspd(&motor1.foc_para.speedcalpar);
}

void motor1_isr_adc(void)
{
    uint32_t status;
    float user_give_angle = 0;
    status = pwm_get_status(BOARD_PMSM1PWM);
    if (PWM_IRQ_CMP(BOARD_PMSM1_PWM_TRIG_CMP_INDEX_CURRENTLOOP) == (status & PWM_IRQ_CMP(BOARD_PMSM1_PWM_TRIG_CMP_INDEX_CURRENTLOOP))) 
    {
        pwm_clear_status(BOARD_PMSM1PWM, status);
        motor1.adc_trig_event_callback();
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_PMSM1APP_PWM_IRQ, motor1_isr_adc)


void motor2_angle_align_loop(void)
{
    motor2.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM2_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor2.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM2_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor2.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM2_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor2.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_theta);
    motor2.foc_para.currentdpipar.target = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_IdRef);
    motor2.foc_para.currentqpipar.target = HPM_MOTOR_MATH_FL_MDF(0);
    motor2.foc_para.func_dqsvpwm(&motor2.foc_para,0,0,0);
    motor2.foc_para.pwmpar.pwmout.func_set_pwm(&motor2.foc_para.pwmpar.pwmout);
}

void motor2_highspeed_loop(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;
    uint32_t  pos;
    float fre_get_angle = 0.0;
    qei_phase_config(BOARD_PMSM2_QEI_BASE, BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV, qei_z_count_inc_on_phase_count_max, true);
    pos = qei_get_current_count(BOARD_PMSM2_QEI_BASE, qei_counter_type_phase)&0x1fffff;
    uint32_t enc_cnt = BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV/motor2.foc_para.motorpar.i_poles_n;
    fre_get_angle = 360-(((pos%enc_cnt)*360)/enc_cnt);

    motor2.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM2_ADC_TRG*4]&0xffff)>>4);
    motor2.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM2_ADC_TRG*4]&0xffff)>>4);
    motor2.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM2_ADC_TRG*4]&0xffff)>>4);
    motor2.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(fre_get_angle);
    if (1 == Motor_Control_Global.ControlWord[2])
    {
      motor2.foc_para.func_dqsvpwm(&motor2.foc_para,0,0,0);
      motor2.foc_para.pwmpar.pwmout.func_set_pwm(&motor2.foc_para.pwmpar.pwmout);
    }
    else
    {
       motor2.foc_para.currentdpipar.target = 0;
       motor2.foc_para.currentqpipar.target = 0;
       motor2.foc_para.currentdpipar.mem = 0;
       motor2.foc_para.currentqpipar.mem = 0;
       motor2.foc_para.currentdpipar.cur = 0;
       motor2.foc_para.currentqpipar.cur = 0;
       disable_all_pwm_output(BOARD_PMSM2PWM);
    }
    motor2.foc_para.speedcalpar.speedtheta = motor2.foc_para.electric_angle;
    motor2.foc_para.speedcalpar.func_getspd(&motor2.foc_para.speedcalpar);
}

void motor2_isr_adc(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;
    status = pwm_get_status(BOARD_PMSM2PWM);
    if (PWM_IRQ_CMP(BOARD_PMSM2_PWM_TRIG_CMP_INDEX_CURRENTLOOP) == (status & PWM_IRQ_CMP(BOARD_PMSM2_PWM_TRIG_CMP_INDEX_CURRENTLOOP))) 
    {
        pwm_clear_status(BOARD_PMSM2PWM, status);
        motor2.adc_trig_event_callback();
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_PMSM2APP_PWM_IRQ, motor2_isr_adc)

void motor3_angle_align_loop(void)
{
    motor3.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM3_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor3.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM3_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor3.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM3_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor3.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_theta);
    motor3.foc_para.currentdpipar.target = HPM_MOTOR_MATH_FL_MDF(Motor_Control_Global.commu_IdRef);
    motor3.foc_para.currentqpipar.target = HPM_MOTOR_MATH_FL_MDF(0);
    motor3.foc_para.func_dqsvpwm(&motor3.foc_para,0,0,0);
    motor3.foc_para.pwmpar.pwmout.func_set_pwm(&motor3.foc_para.pwmpar.pwmout);
}

void motor3_highspeed_loop(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;
    uint32_t  pos;
    float fre_get_angle = 0.0;
    qei_phase_config(BOARD_PMSM3_QEI_BASE, BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV, qei_z_count_inc_on_phase_count_max, true);
    pos = qei_get_current_count(BOARD_PMSM3_QEI_BASE, qei_counter_type_phase)&0x1fffff;
    uint32_t enc_cnt = BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV/motor3.foc_para.motorpar.i_poles_n;
    fre_get_angle = 360-(((pos%enc_cnt)*360)/enc_cnt);

    motor3.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM3_ADC_TRG*4]&0xffff)>>4);
    motor3.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM3_ADC_TRG*4]&0xffff)>>4);
    motor3.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM3_ADC_TRG*4]&0xffff)>>4);
    motor3.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(fre_get_angle);
    if (1 == Motor_Control_Global.ControlWord[3])
    {
      motor3.foc_para.func_dqsvpwm(&motor3.foc_para,0,0,0);
      motor3.foc_para.pwmpar.pwmout.func_set_pwm(&motor3.foc_para.pwmpar.pwmout);
    }
    else
    {
       motor3.foc_para.currentdpipar.target = 0;
       motor3.foc_para.currentqpipar.target = 0;
       motor3.foc_para.currentdpipar.mem = 0;
       motor3.foc_para.currentqpipar.mem = 0;
       motor3.foc_para.currentdpipar.cur = 0;
       motor3.foc_para.currentqpipar.cur = 0;
       disable_all_pwm_output(BOARD_PMSM3PWM);
    }

    motor3.foc_para.speedcalpar.speedtheta = motor3.foc_para.electric_angle;
    motor3.foc_para.speedcalpar.func_getspd(&motor3.foc_para.speedcalpar);

}
void motor3_isr_adc(void)
{
    uint32_t status, status1, status1cnt;
    float user_give_angle = 0;

    status = pwm_get_status(BOARD_PMSM3PWM);
    if (PWM_IRQ_CMP(BOARD_PMSM3_PWM_TRIG_CMP_INDEX_CURRENTLOOP) == (status & PWM_IRQ_CMP(BOARD_PMSM3_PWM_TRIG_CMP_INDEX_CURRENTLOOP))) 
    {
        pwm_clear_status(BOARD_PMSM3PWM, status);
        motor3.adc_trig_event_callback();
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_PMSM3APP_PWM_IRQ, motor3_isr_adc)






