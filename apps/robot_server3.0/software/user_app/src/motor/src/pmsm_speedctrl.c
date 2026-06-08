/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pmsm_speedctrl.h"
#include "pmsm_init.h"
#include "cia402appl.h"
#include "pmsm_interia.h"
#include "bldc_foc_callback.h"

/**********************************************
 * *void speedloop_ctrl()
 * * speed loop PID
 * *@Input: None
 * *#Output：None
 * ***************************************/
void speedloop_ctrl(MOTOR_PARA *par, PWMV2_Type *ptr, MOTOR_CONTROL_Global * Motor_Control_Word, CMDGENE_PARA* CMDGENEObj )
{
    enable_all_pwm_output(ptr);//etehrcat csv模式的速度指令由主站下发
    
    if (BOARD_PMSM0_INTERIA && Motor_Control_Word->Motor_Interia == BOARD_PMSM0_INTERIA)
    {
#if defined  (CONFIG_USE_FUNCTION) && (CONFIG_USE_FUNCTION)
        motor_interiactrl(&motor, Motor_Control_Word->Motor_Interia, CMDGENEObj);
#endif

    }else
    {
#if  MOTORCONTROL_EC_OR_STUDIO   
        vel_cmd_gene(CMDGENEObj);
#endif
        par->speedloop_para.target = HPM_MOTOR_MATH_FL_MDF(CMDGENEObj->cmdgene_out_par.velcmd);
        par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout;
        par->speedloop_para.func_pid(&par->speedloop_para);
        par->foc_para.currentqpipar.target = par->speedloop_para.outval;
        par->foc_para.currentdpipar.target =  0; 
     }
}
/**********************************************
 * *void positionloop_ctrl()
 * * position loop PID
 * *@Input: None
 * *#Output：None
 * ***************************************/
void positionloop_ctrl(MOTOR_PARA *par, PWMV2_Type *ptr, MOTOR_CONTROL_Global * Motor_Control_Word, CMDGENE_PARA* CMDGENEObj,qei_CalObj* qei_CalHdl )
{

      enable_all_pwm_output(ptr); 
     
 #if  MOTORCONTROL_EC_OR_STUDIO     
      pos_cmd_gene(CMDGENEObj);
 #endif
#if !MOTORCONTROL_EC_OR_STUDIO
      par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout;
#endif
      par->position_para.target = HPM_MOTOR_MATH_FL_MDF(CMDGENEObj->cmdgene_out_par.poscmd);
      par->position_para.func_pid(&par->position_para);         
      par->speedloop_para.target =   HPM_MOTOR_MATH_MDF_FL(par->position_para.outval*1000) + CMDGENEObj->cmdgene_out_par.velcmd;
      par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout;
      par->speedloop_para.func_pid(&par->speedloop_para);
      par->foc_para.currentqpipar.target =  par->speedloop_para.outval;
      par->foc_para.currentdpipar.target =  0;
}
/**********************************************
 * *void motor_foc_angle_align()
 * * when ABZ encoder is used, prepositioning is must be applied in power on
 * *@Input: Motor structure, 1ms flag
 * *#Output：None
 * ***************************************/
void motor_foc_angle_align(MOTOR_PARA *motor_par,MOTOR_CONTROL_Global* motor_ctrl,uint32_t current_set)
{   
        if(motor_ctrl->zero_CW == motor_zero_control)
       {
           motor_par->adc_trig_event_callback = motor_angle_align_loop;   
           enable_all_pwm_output(BOARD_PMSM0PWM);
           motor_ctrl->zero_cnt++;   

           if((motor_ctrl->commu_IdRef < current_set)&&(motor_ctrl->zero_cnt<=5))
              {
                  motor_ctrl->commu_IdRef = 20 + motor_ctrl->commu_IdRef;
                  motor_ctrl->commu_theta = 90;                          
              }else{ 
              
                  if(motor_ctrl->commu_theta>0)
                  {
                        motor_ctrl->commu_theta =   motor_ctrl->commu_theta - 0.1 ;                   
                  }else{
        
                        if (motor_ctrl->commu_IdRef>0)
                        {
                               motor_ctrl->commu_IdRef = motor_ctrl->commu_IdRef - 10;                         
                        }else{

                               disable_all_pwm_output(BOARD_PMSM0PWM);     
                               motor_ctrl->motor_status = motor_zero_status; 
#if BOARD_PMSM0_ENCODER_TYPE_SET  
                               qeiv2_cfg_init();
#endif
                                motor_par->adc_trig_event_callback = motor_highspeed_loop;            
                        }
                  }
              }
      }
}
/**********************************************
 * *void param_update()
 * * when motor is disable, params is be inited
 * *@Input: intermediate variable
 * *#Output：None
 * ***************************************/
void param_update(Para_Obj* paraHdl, CMDGENE_PARA* CMDGENEHdl,qei_CalObj* qei_CalHdl)
{
    CMDGENEHdl->cmdgene_in_par.velcfgpar.q0 = 0;
    CMDGENEHdl->cmdgene_in_par.velcfgpar.Tv = paraHdl->VelObj.VelCnstTime.value/1000;
    CMDGENEHdl->cmdgene_in_par.velcfgpar.v0 = 0;
    CMDGENEHdl->cmdgene_in_par.velcfgpar.v1 = 0;
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
    {
        CMDGENEHdl->cmdgene_in_par.velcfgpar.vmax = paraHdl->VelObj.Vel_Ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.velcfgpar.amax = paraHdl->VelObj.Acc_Ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.velcfgpar.jmax = paraHdl->VelObj.Jerk_Ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
    }else{ 

        CMDGENEHdl->cmdgene_in_par.velcfgpar.vmax = paraHdl->VelObj.Vel_Ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.velcfgpar.amax = paraHdl->VelObj.Acc_Ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.velcfgpar.jmax = paraHdl->VelObj.Jerk_Ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
    }
    CMDGENEHdl->cmdgene_in_par.poscfgpar.q0 = qei_CalHdl->pos;
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)  
    {
        CMDGENEHdl->cmdgene_in_par.poscfgpar.q1 = paraHdl->PosObj.Pos_ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
    }else{

        CMDGENEHdl->cmdgene_in_par.poscfgpar.q1 = paraHdl->PosObj.Pos_ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
    }
    CMDGENEHdl->cmdgene_in_par.poscfgpar.v0 = 0;
    CMDGENEHdl->cmdgene_in_par.poscfgpar.v1 = 0;
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
    {
        CMDGENEHdl->cmdgene_in_par.poscfgpar.vmax = paraHdl->VelObj.Vel_Ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.poscfgpar.amax = paraHdl->VelObj.Acc_Ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.poscfgpar.jmax = paraHdl->VelObj.Jerk_Ref.value * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;

    }else{

        CMDGENEHdl->cmdgene_in_par.poscfgpar.vmax = paraHdl->VelObj.Vel_Ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.poscfgpar.amax = paraHdl->VelObj.Acc_Ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
        CMDGENEHdl->cmdgene_in_par.poscfgpar.jmax = paraHdl->VelObj.Jerk_Ref.value * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
    }
    CMDGENEHdl->cmdgene_in_par.poscfgpar.cyclecnt = paraHdl->PosObj.CycleCount.value;
    CMDGENEHdl->cmdgene_in_par.poscfgpar.cycletype = paraHdl->PosObj.CycleType.value;
    CMDGENEHdl->cmdgene_in_par.poscfgpar.dwelltime = paraHdl->PosObj.DwellTime.value;
    CMDGENEHdl->cmdgene_in_par.poscfgpar.isr_time_s = 0.001;
    CMDGENEHdl->cmdgene_in_par.velcfgpar.isr_time_s = 0.001;

}

/**********************************************
 * *void motor_control_parmas_update()
 * * pi control parmas changed by COE or init
 * *@Input: motor structure
 * *#Output：None
 * ***************************************/

#if BOARD_PMSM0_PICONTROL_PARMA_UPDATE 
TCiA402Axis *m_par_COE;
void motor_control_parmas_update(MOTOR_PARA *motor_par)
{
   static int32_t m_kp, m_ki, m_kd;
   static int32_t mk_old;
   //mk_old = m_kp;
   
   
   m_kp = s_P;   //user should know this is int type not float type. So it should be changed into float type pi parmas.
   m_ki = s_I;   //user should know this is int type not float type. So it should be changed into float type pi parmas.
   m_kd = s_D;   //user should know this is int type not float type. So it should be changed into float type pi parmas.
   

   if (mk_old != m_kp)
   {
      printf("mkp:%x\n",m_kp);
      mk_old = m_kp;
   }

   motor_par->foc_para.currentdpipar.i_kp = (float)m_kp;
   motor_par->foc_para.currentdpipar.i_ki = (float)m_ki;
   motor_par->foc_para.currentqpipar.i_kp = (float)m_kp;
   motor_par->foc_para.currentqpipar.i_ki = (float)m_ki;
  

}
#endif
/**********************************************
 * *void isr_speed_loop()
 * * 1ms ISR
 * *@Input: None
 * *#Output：None
 * ***************************************/
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_speed_loop)
void isr_speed_loop(void)
{
    volatile uint32_t s = BOARD_BLDC_TMR_1MS->SR;
    BOARD_BLDC_TMR_1MS->SR = s;
    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) 
    {
        Motor_Control_Global.flag_1ms = !Motor_Control_Global.flag_1ms; 
        if (Motor_Control_Global.Motor_Encoder_Type != BOARD_PMSM0_ENCODER_TYPE)  
        {
            Motor_Control_Global.zero_CW = 0x1210;
    
            if (Motor_Control_Global.motor_status == motor_zero_status)
            {
                Motor_Control_Global.zero_CW = 0;
            }
        }
        if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)  
        {
            Motor_Control_Global.motor_status = motor_zero_status; 
        }else{

            motor_foc_angle_align(&motor,&Motor_Control_Global,100);
        }

        if(Motor_Control_Global.motor_CW == 1)
        {                     
            if(Motor_Control_Global.OP_mode == POSITION_MODE)
            {   
                positionloop_ctrl(&motor, BOARD_PMSM0PWM, &Motor_Control_Global,&CMDGENEObj, &qeiCalObj );
            }else if(Motor_Control_Global.OP_mode == SPEED_MODE){
                speedloop_ctrl(&motor, BOARD_PMSM0PWM, &Motor_Control_Global,&CMDGENEObj );
                
            }
        }else{ 
            motor.speedloop_para.target = 0;
            motor.speedloop_para.cur = 0;
            motor.speedloop_para.mem = 0;
            motor.position_para.mem = 0;
            param_update(&paraObj[0], &CMDGENEObj, &qeiCalObj);
#if BOARD_PMSM0_PICONTROL_PARMA_UPDATE 
            motor_control_parmas_update(&motor);
#endif
            cmd_gene_disable(&CMDGENEObj.cmdgene_user_par);
        }
        motor.position_para.cur = HPM_MOTOR_MATH_FL_MDF(motor.foc_para.speedcalpar.speedtheta);
        motor_protect(&motor, &Motor_Control_Global);
        
#if MOTORCONTROL_EC_OR_STUDIO && (!BOARD_PMSM0_INTERIA)
        static uint16_t m_times_start;//CES展会定制，上电CSP模式，正反转
        if ((Motor_Control_Global.motor_status == motor_zero_status) && (m_times_start < 5000))
        {
            m_times_start++;  
        }
        if ((Motor_Control_Global.motor_status == motor_zero_status) && (m_times_start >= 5000))
        {
            Motor_Control_Global.motor_CW = 1;
        }
#endif
    }
}

mcl_user_value_t motor_speed;
mcl_user_value_t motor_position;
/**********************************************
 * *void motor_speed_loop_init()
 * * ethercat init
 * *@Input: None
 * *#Output：None
 * ***************************************/
void motor_speed_loop_init(void)
{
    if (motor_position.enable) {
        motor_position.enable = false;
    }
    motor_speed.enable = false;
    Motor_Control_Global.motor_CW = 0x00;
    Motor_Control_Global.OP_mode = SPEED_MODE;
    printf("motor speed mode!\n");
}
/**********************************************
 * *void motor_speed_loop_set()
 * * ethercat speed set
 * *@Input: None
 * *#Output：None
 * ***************************************/
void motor_speed_loop_set(int32_t target_speed)
{
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
    {
        motor_speed.value = (float)target_speed * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV / (float)9.2;
    }else{
        motor_speed.value = (float)target_speed * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV / (float)9.2;
    }    
    CMDGENEObj.cmdgene_out_par.velcmd = motor_speed.value;
    //printf("%x!\n",target_speed);
    
}
/**********************************************
 * *void motor_postion_loop_init()
 * * ethercat init
 * *@Input: None
 * *#Output：None
 * ***************************************/
void motor_postion_loop_init(void)
{
    if (motor_speed.enable) 
    {
        motor_speed.enable = false;
    }
    motor_position.enable = false;
    Motor_Control_Global.motor_CW = 0x00;
    Motor_Control_Global.OP_mode = POSITION_MODE;
    printf("motor position mode!\n");
}
/**********************************************
 * *void motor_position_loop_set()
 * * ethercat position set
 * *@Input: Target position
 * *#Output：None
 * ***************************************/
void motor_position_loop_set(int32_t target_position)
{
    motor_position.value = (float)target_position / CIA402_PSITION_COEFFICIENT * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
    {
        motor_position.value = (float)target_position / CIA402_PSITION_COEFFICIENT * BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
    }else{
        motor_position.value = (float)target_position / CIA402_PSITION_COEFFICIENT * BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV;
    } 
    CMDGENEObj.cmdgene_out_par.poscmd = motor_position.value;
}
/**********************************************
 * *void motor_get_actual_speed()
 * * ethercat speed feedback
 * *@Input: None
 * *#Output：None
 * ***************************************/
int32_t motor_get_actual_speed(void)
{
    int32_t m_speedbak;
    
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
    {
        m_speedbak = (int32_t)(motor.speedloop_para.cur * (float)9.2 / (float)BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV);
    }else{
        m_speedbak = (int32_t)(motor.speedloop_para.cur * (float)9.2 / (float)BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV);
    } 
    return m_speedbak;  
}
/**********************************************
 * *void motor_get_actual_position()
 * * ethercat position feedback
 * *@Input: None
 * *#Output：None
 * ***************************************/
int32_t motor_get_actual_position(void)
{
    if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
    {
        return (int32_t)(motor.position_para.cur / (float)BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV * (float)CIA402_PSITION_COEFFICIENT);
    }
    else
    {
        return (int32_t)(motor.position_para.cur / (float)BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV * (float)CIA402_PSITION_COEFFICIENT);
    } 
}
/**********************************************
 * *void motor_stop()
 * * ethercat motor stop
 * *@Input: None
 * *#Output：None
 * ***************************************/
void motor_stop(void)
{
    if (motor_speed.enable) {
        motor_speed.value = 0;
        CMDGENEObj.cmdgene_out_par.velcmd = motor_speed.value;
    } else if (motor_position.enable) {
        if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE) 
        {
            motor_position.value = motor.position_para.cur / (float)BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV * CIA402_PSITION_COEFFICIENT;
        }else{
            motor_position.value = motor.position_para.cur / (float)BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV * CIA402_PSITION_COEFFICIENT;
        }
        CMDGENEObj.cmdgene_out_par.poscmd = motor_position.value;
    }
    Motor_Control_Global.motor_CW = 0x0;
}
/**********************************************
 * *void motor_enable()
 * * ethercat motor enable
 * *@Input: None
 * *#Output：None
 * ***************************************/
void motor_enable(void)
{
    Motor_Control_Global.motor_CW = 0x01;
    printf("motor enable!\n");
}
/**********************************************
 * *void motor_disable()
 * * ethercat motor disable
 * *@Input: None
 * *#Output：None
 * ***************************************/
void motor_disable(void)
{
    Motor_Control_Global.motor_CW = 0x0;
    printf("motor disable!\n");
}


