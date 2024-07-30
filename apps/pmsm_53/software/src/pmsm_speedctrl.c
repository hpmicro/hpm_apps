#include "pmsm_speedctrl.h"

CMDGENE_PARA cmdpar[MAX_AXIS_NUM] = {0};

void motor_speed_ctrl(MOTOR_PARA *par, PWM_Type *ptr)
{
    int32_t pos_fdk[MAX_AXIS_NUM] = {0};
    uint8_t motor_id = par->foc_para.pwmpar.pwmout.i_motor_id-1;
    pos_fdk[motor_id] = pos_angleObj[motor_id].pos;
    uint32_t MAX_cnt = pos_angleObj[motor_id].maxph;
    uint8_t global_cw = globalAxisParm[motor_id].parmCmd.ControlWord0x9000;
    uint8_t op_mode = globalAxisParm[motor_id].parmCmd.OpMode0x9001;
    if (0 == global_cw)
    {
        par->foc_para.pos_estimator_par.func = NULL;

    }
    if (op_mode == SPEED_MODE) 
    {           
       if (1 == global_cw)
       {
          enable_all_pwm_output(ptr);
          vel_cmd_gene(&cmdpar[motor_id]);
          par->speedloop_para.target = HPM_MOTOR_MATH_FL_MDF(cmdpar[motor_id].cmdgene_out_par.velcmd);   
          par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout*MAX_cnt;
          par->speedloop_para.func_pid(&par->speedloop_para); 
          par->foc_para.currentqpipar.target =  par->speedloop_para.outval;
          par->foc_para.currentdpipar.target =  0;         
       }
       else
       {
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.q0 = pos_fdk[motor_id];
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.Tv = CONSTANT_SPEED_TIME_s;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.v0 = par->speedloop_para.cur;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.v1 = 0;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.vmax =MOVE_SPEED*MAX_cnt;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.amax = MOVE_ACC*MAX_cnt;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.jmax = MOVE_JERK*MAX_cnt;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.isr_time_s = ISR_TIME_s;
          cmd_gene_disable(&cmdpar[motor_id].cmdgene_user_par);
          
       }

    } 
    else                                      
    {        
        if(op_mode == POSITION_MODE)
        {
            if (1 == global_cw)
            {          
                enable_all_pwm_output(ptr);
                pos_cmd_gene(&cmdpar[motor_id]);
                par->position_para.cur = HPM_MOTOR_MATH_FL_MDF(pos_fdk[motor_id]);
                par->position_para.target = HPM_MOTOR_MATH_FL_MDF(cmdpar[motor_id].cmdgene_out_par.poscmd);
                par->position_para.func_pid(&par->position_para);
                par->speedloop_para.target = par->position_para.outval*1000;
                par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout*MAX_cnt;
                par->speedloop_para.func_pid(&par->speedloop_para); 
                par->foc_para.currentqpipar.target =  par->speedloop_para.outval;
                par->foc_para.currentdpipar.target =  0;         
        
            }
            else
            {
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.q0 = pos_fdk[motor_id];
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.q1 = MOVE_ABSOLUTE_POS_r*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.v0 = par->speedloop_para.cur;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.v1 = 0;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.vmax = MOVE_SPEED*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.amax = MOVE_ACC*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.jmax = MOVE_JERK*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.cyclecnt = CYCLE_CNT;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.cycletype = CONTINOUS_RONUD_MOVE_TYPE;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.dwelltime = DWELLTIME_ms;
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.isr_time_s = ISR_TIME_s;

                cmdpar[motor_id].cmdgene_in_par.velcfgpar.q0 = pos_fdk[motor_id];
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.Tv = CONSTANT_SPEED_TIME_s;
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.v0 = par->speedloop_para.cur;
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.v1 = 0;
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.vmax =MOVE_SPEED*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.amax = MOVE_ACC*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.jmax = MOVE_JERK*MAX_cnt;
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.isr_time_s = ISR_TIME_s;

                cmd_gene_disable(&cmdpar[motor_id].cmdgene_user_par);
                
            }

        }
        
    }

     globalAxisParm[motor_id].parmVel.Vel_IntrCmd0x600c = par->speedloop_para.target;

     globalAxisParm[motor_id].parmPos.Pos_IntrCmd0x7006 = par->position_para.target;
     globalAxisParm[motor_id].parmPos.Pos_IntrFdk0x7005 = pos_fdk[motor_id];
     globalAxisParm[motor_id].parmVel.Vel_IntrFdk0x600b = par->speedloop_para.cur;

}



void isr_gptmr(void)
{
  
    volatile uint32_t s = BOARD_BLDC_TMR_1MS->SR;
    BOARD_BLDC_TMR_1MS->SR = s;

    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) 
    {
        Motor_Control_Global.flag_1ms = !Motor_Control_Global.flag_1ms;
        motor_foc_angle_align(BLDC_MOTOR0_INDEX);
        motor_speed_ctrl(&motor0, BOARD_PMSM0PWM);
       
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)