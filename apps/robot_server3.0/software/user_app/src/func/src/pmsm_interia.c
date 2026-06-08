/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pmsm_interia.h"

/**********************************************
 * *void interia_param_init(Para_Obj* para,INERTIA_CAL_Obj* INTERIA_CalHdl)
 * * 
 * *@Input: Motor structure
 * *#Output：None
 * ***************************************/
void interia_param_init(Para_Obj* para)
{
    para->FuncParaObj.inertia_fai.value = 14472;
    para->FuncParaObj.inertia_vel1.value = 350;
    para->FuncParaObj.inertia_cycle.value = 10;
    para->FuncParaObj.inertia_en.value = 0;
    para->FuncParaObj.inertia_k.value = 4;
    para->FuncParaObj.inertia_step.value = 500;
}






/**********************************************
 * *void motor_interiactrl(MOTOR_PARA* par)
 * * 
 * *@Input: Motor structure
 * *#Output：None
 * ***************************************/
void motor_interiactrl(MOTOR_PARA* par, uint8_t interia_ctrl, CMDGENE_PARA* par_cmd)
{
    double temp = fabs(4096 - par->foc_para.samplcurpar.adc_u_middle);
    INTERIA_CalHdl.INTERIA_InObj.iq = fabs((par->foc_para.currentqpipar.cur * 16.5) / temp);//iq计算
    INTERIA_CalHdl.INTERIA_InObj.speed_fdk = par->speedloop_para.cur;
    //motor_interia_cal_process(par, &INTERIA_CalHdl);
    interia_cal_process(&INTERIA_CalHdl);
    
    if(interia_ctrl == BOARD_PMSM0_INTERIA)
    {
        INTERIA_CalHdl.INTERIA_InObj.interia_start = 1;
        //if(INTERIA_CalHdl.INTERIA_OutObj.status==0)
        {
            ///辨识成功  
            //double inertia_J = INTERIA_CalHdl.INTERIA_OutObj.J;
        }
        par->speedloop_para.target = INTERIA_CalHdl.INTERIA_UserObj.Vel_out * 4000 * 1000; 
        par->speedloop_para.cur = par->foc_para.speedcalpar.o_speedout;
        par->speedloop_para.func_pid(&par->speedloop_para); /*速度控制函数*/
        par->foc_para.currentqpipar.target =  par->speedloop_para.outval;
        par->foc_para.currentdpipar.target =  0;    
     }     
}


