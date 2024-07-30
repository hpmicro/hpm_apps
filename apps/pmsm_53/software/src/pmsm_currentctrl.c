
#include "pmsm_currentctrl.h"

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_SEQ_DMA_SIZE_IN_4BYTES];


void electric_angle_cal(pos_angle_CalObj* pos_angleCalHdl)
{
    if(pos_angleCalHdl->encoder_type == ABZ)
    {
        pos_angleCalHdl->z =qeiv2_get_current_count(BOARD_PMSM0_QEI_BASE, qeiv2_counter_type_z)&0x1fffff;
        pos_angleCalHdl->ph = qeiv2_get_current_count(BOARD_PMSM0_QEI_BASE, qeiv2_counter_type_phase)&0x1fffff;
        if(pos_angleCalHdl->z >= (0x200000 >> 1))
        {
           pos_angleCalHdl->pos = -(((pos_angleCalHdl->z - 0x200000)*pos_angleCalHdl->maxph)+pos_angleCalHdl->ph);
        }
        else
        {
           pos_angleCalHdl->pos =  -((pos_angleCalHdl->z*pos_angleCalHdl->maxph)+pos_angleCalHdl->ph);
        }
        uint32_t enc_cnt = pos_angleCalHdl->maxph/pos_angleCalHdl->motor_pole;
 
        pos_angleCalHdl->elec_angle = 360- (pos_angleCalHdl->ph%enc_cnt)*360.0/enc_cnt;
    
     
    }
    else if(pos_angleCalHdl->encoder_type == ABS)
    {
     pos_angleCalHdl->z =sei_get_data_value(HPM_SEI, SEI_DAT_7);
     pos_angleCalHdl->ph = sei_get_data_value(HPM_SEI, SEI_DAT_5);
     pos_angleCalHdl->pos = pos_angleCalHdl->ph +  pos_angleCalHdl->z*pos_angleCalHdl->maxph; 
     uint32_t enc_cnt = pos_angleCalHdl->maxph/pos_angleCalHdl->motor_pole;
     int32_t delta_pos = pos_angleCalHdl->pos- pos_angleCalHdl->pos0;
     if(delta_pos>=0)
     {
        pos_angleCalHdl->elec_angle = (delta_pos%enc_cnt)*360.0/enc_cnt;
     }
     else
     {
        pos_angleCalHdl->elec_angle =  360-((abs(delta_pos)%enc_cnt)*360)/enc_cnt ;
     }
    
    }

}


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
    
    motor0.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_PMSM0_ADC_TRG*4]&0xffff)>>4)&0xfff;
    electric_angle_cal(&pos_angleObj[0]);
    motor0.foc_para.electric_angle = pos_angleObj[0].elec_angle;
    if (1 == globalAxisParm[0].parmCmd.ControlWord0x9000)
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
    uint32_t status;

    status = pwm_get_status(BOARD_PMSM0PWM);
    if (PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP) == (status & PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP))) 
    {
        pwm_clear_status(BOARD_PMSM0PWM, status);
        motor0.adc_trig_event_callback();
    }

}
SDK_DECLARE_EXT_ISR_M(BOARD_PMSM0APP_PWM_IRQ, motor0_isr_adc)
