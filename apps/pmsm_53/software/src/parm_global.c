#include "parm_global.h"


AXIS_PARAM globalAxisParm[MAX_AXIS_NUM];

void parm_SetToDefault(void)
{
    uint32_t i;
    for(i=0;i<MAX_AXIS_NUM;i++)
    {
      globalAxisParm[i].parmMotor.Motor_RatedCurrent0x4000_3 = 0.125;
      globalAxisParm[i].parmMotor.Mortor_RatedVlotage0x4001 = 24;
      globalAxisParm[i].parmMotor.Mortor_R0x4002_3= 1.1;
      globalAxisParm[i].parmMotor.Mortor_L0x4003_3 = 0.003;
      globalAxisParm[i].parmMotor.Mortor_PoleNum0x4004= 2;
      globalAxisParm[i].parmMotor.Mortor_PolePicth0x4005 = 30000;
      globalAxisParm[i].parmMotor.Motor_Type0x4006 = 0;  
      globalAxisParm[i].parmMotor.Motor_EncType0x4007 = 0;
      globalAxisParm[i].parmMotor.Motor_PlusCnt0x4008 = 4000;  
       
      globalAxisParm[i].parmCurrent.Kp0x5000_3 = 0.5;
      globalAxisParm[i].parmCurrent.Ki0x5001_3 = 0.02;
      globalAxisParm[i].parmCurrent.Current_ref0x5002 = 0;
      globalAxisParm[i].parmCurrent.Current_Fdk0x5003 = 0;
   
      globalAxisParm[i].parmVel.Kp0x6000 = 60;
      globalAxisParm[i].parmVel.Ki0x6001_3 = 0.01;
      globalAxisParm[i].parmVel.Vel_ForWard_Gain0x6002 = 0;
      globalAxisParm[i].parmVel.LowPassWn0x6003_3 =  0.1;
      globalAxisParm[i].parmVel.Vel_Ref0x6004 = 10;
      globalAxisParm[i].parmVel.Acc_Ref0x6005 = 100;
      globalAxisParm[i].parmVel.Jerk_Ref0x6006= 1000;    
      globalAxisParm[i].parmVel.Dec_Ref0x6007 = 0;
      globalAxisParm[i].parmVel.Vel_Fdk0x6008 = 0;
      globalAxisParm[i].parmVel.Vel_err0x6009 = 0;
      globalAxisParm[i].parmVel.Vel_IntrErr0x600a = 0;
      globalAxisParm[i].parmVel.Vel_IntrFdk0x600b = 0;
      globalAxisParm[i].parmVel.Vel_IntrCmd0x600c = 0;
      globalAxisParm[i].parmVel.VelCnstTime0x600d = 0;

      globalAxisParm[i].parmPos.Kp0x7000_3 = 0.1;
      globalAxisParm[i].parmPos.Pos_ref0x7001 = 20;
      globalAxisParm[i].parmPos.Pos_Fdk0x7002 = 0;
      globalAxisParm[i].parmPos.Pos_err0x7003 = 0;
      globalAxisParm[i].parmPos.Pos_IntrErr0x7004 =0;
      globalAxisParm[i].parmPos.Pos_IntrFdk0x7005 = 0;
      globalAxisParm[i].parmPos.Pos_IntrCmd0x7006 = 0;
      globalAxisParm[i].parmPos.CycleType0x7007 = 0;
      globalAxisParm[i].parmPos.CmdType0x7008 = 0;
      globalAxisParm[i].parmPos.DwellTime0x7009 = 500;
      globalAxisParm[i].parmPos.CycleCount0x700a = 1;

      globalAxisParm[i].parmAssist.param_save0x8000 = 0;
      globalAxisParm[i].parmAssist.paramOk_flag0x8001 = 0;
   
      globalAxisParm[i].parmCmd.ControlWord0x9000 =0;
      globalAxisParm[i].parmCmd.OpMode0x9001 = 0;
      globalAxisParm[i].parmCmd.zero_ControlWord0x9002 = 0;
      globalAxisParm[i].parmCmd.status0x9003 = 0;
    }
}


void parm_UpdatePowerOn(void)
{
      iic_pin_cfg();
      parm_SetToDefault();
      parmReadAll(&globalAxisParm[0].parmMotor);
      if(globalAxisParm[0].parmAssist.paramOk_flag0x8001!=1)
      {
          parm_SetToDefault();
      }
}



void parm_GlobalManage(void)
{
    AXIS_PARAM *axis;
    axis = &globalAxisParm[0];
    if(axis->parmAssist.param_save0x8000 == 0) return;
    if(axis->parmAssist.param_save0x8000 == 1)
    {
	parm_SetToDefault();  
    }
    else
    {	
        axis->parmAssist.param_save0x8000 = 0 ;	
        globalAxisParm[0].parmAssist.paramOk_flag0x8001 = 1;
	parmStoreAll();
        
    }
    
}


