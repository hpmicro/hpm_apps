#ifndef _INERTIA_CAL_H
#define _INERTIA_CAL_H
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stdint.h"
#include "Parm_Global.h"
#include "data_trans.h"



typedef struct  _tag_INERTIA_IN_Obj
{
  double    vel1;
  double    iq;
  double    flux;
  uint8_t   interia_start;
  uint16_t  step;  //加速时间，ms
  uint8_t   kk;
  uint8_t   cycle;
  uint8_t   poles;
  int16_t   currentsample_middle;
  double    currentsample_actual;
  float     speed_fdk;
}INERTIA_IN_Obj;

typedef struct  _tag_INERTIA_USER_Obj
{
double vel1;
double vel2;
double Ts;
double Vel_out;//unit r/ms
double Te1[10];
double Te2[10];
double Te3[10];
double Te4[10];
uint16_t cnt;
uint8_t process;
//double k;
double delta_vel;
double b1;
double b2;
double b0;
uint32_t temp;
uint32_t cycle;
uint32_t step;
double vel_acc1[10];
double vel_acc2[10];
double vel_dec1[10];
double vel_dec2[10];
double vel_old;
uint8_t ii;

}INERTIA_USER_Obj;


typedef struct  _tag_INERTIA_OUT_Obj
{

double interia_acc[10];
double interia_dec[10];
double J;
uint8_t status;

}INERTIA_OUT_Obj;


typedef struct  _tag_INERTIA_CAL_Obj
{
INERTIA_IN_Obj  INTERIA_InObj;
INERTIA_USER_Obj  INTERIA_UserObj;
INERTIA_OUT_Obj   INTERIA_OutObj;
}INERTIA_CAL_Obj;


typedef enum _tag_interia_process
{
   Acc1_proc,
   Acc2_proc,
   Dec1_proc,
   Dec2_proc,
   Interica_Cal,
}INTERIA_PROC;
extern INERTIA_CAL_Obj INERTIA_CalObj ;
void interia_cal_process(INERTIA_CAL_Obj* INTERIA_CalHdl);
void interia_init(Para_Obj* para,INERTIA_CAL_Obj* INTERIA_CalHdl);
void interia_disable(INERTIA_CAL_Obj* INTERIA_CalHdl);

#endif






