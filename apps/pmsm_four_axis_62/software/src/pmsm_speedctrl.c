/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "pmsm_speedctrl.h"


CMDGENE_PARA cmdpar[4] = {0};


int32_t pmsm_foc_get_pos(QEI_Type *ptr_qei, int32_t MAX_cnt)
{
    int32_t ph,z;
    ph = qei_get_current_count(ptr_qei, qei_counter_type_phase)&0x1fffff;
    z = qei_get_current_count(ptr_qei, qei_counter_type_z)&0x1fffff;

    if(z >= (0x200000 >> 1)){
        return -(((z - 0x200000)*MAX_cnt)+ph);
    }
    else{
        return -((z*MAX_cnt)+ph);
    }
}

void motor_speed_ctrl(MOTOR_PARA *par, PWM_Type *ptr, MOTOR_CONTROL_Global* global_control, QEI_Type *ptr_qei, uint32_t MAX_cnt)
{
    int32_t pos_fdk[4] = {0};
    uint8_t motor_id = par->foc_para.pwmpar.pwmout.i_motor_id-1;
    pos_fdk[motor_id] = pmsm_foc_get_pos(ptr_qei, MAX_cnt);
    uint8_t global_cw = global_control->ControlWord[motor_id];
    uint8_t op_mode = global_control->OpMode[motor_id];
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
          par->speedloop_para.target = HPM_MOTOR_MATH_FL_MDF(cmdpar[motor_id].cmdgene_out_par.velcmd/MAX_cnt);   
          par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout;
          par->speedloop_para.func_pid(&par->speedloop_para); 
          par->foc_para.currentqpipar.target =  par->speedloop_para.outval;
          par->foc_para.currentdpipar.target =  0;         
       }
       else
       {
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.q0 = pos_fdk[motor_id];
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.Tv = CONSTANT_SPEED_TIME_s;//uint s
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.v0 = par->speedloop_para.cur;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.v1 = 0;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.vmax =MOVE_SPEED*MAX_cnt;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.amax = MOVE_ACC*MAX_cnt;
          cmdpar[motor_id].cmdgene_in_par.velcfgpar.jmax = MOVE_JERK*MAX_cnt;
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
                par->speedloop_para.target = cmdpar[motor_id].cmdgene_out_par.velcmd/MAX_cnt ;
                par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout;
                par->speedloop_para.func_pid(&par->speedloop_para); 
                par->foc_para.currentqpipar.target =  par->speedloop_para.outval;
                par->foc_para.currentdpipar.target =  0;         
        
            }
            else
            {
                cmdpar[motor_id].cmdgene_in_par.poscfgpar.q0 = pos_fdk[motor_id];///count
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
                cmdpar[motor_id].cmdgene_in_par.velcfgpar.Tv = CONSTANT_SPEED_TIME_s;//uint s
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

}



void isr_gptmr(void)
{
  
    volatile uint32_t s = BOARD_BLDC_TMR_1MS->SR;
    BOARD_BLDC_TMR_1MS->SR = s;

    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) 
    {
        Motor_Control_Global.flag_1ms = !Motor_Control_Global.flag_1ms;
        motor_speed_ctrl(&motor0, BOARD_PMSM0PWM, &Motor_Control_Global, BOARD_PMSM0_QEI_BASE, BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV);
        motor_speed_ctrl(&motor1, BOARD_PMSM1PWM, &Motor_Control_Global, BOARD_PMSM1_QEI_BASE, BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV);
        motor_speed_ctrl(&motor2, BOARD_PMSM2PWM, &Motor_Control_Global, BOARD_PMSM2_QEI_BASE, BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV);
        motor_speed_ctrl(&motor3, BOARD_PMSM3PWM, &Motor_Control_Global, BOARD_PMSM3_QEI_BASE, BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV);  
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)



