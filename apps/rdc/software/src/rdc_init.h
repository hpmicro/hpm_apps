#ifndef RDC_INIT_H_
#define RDC_INIT_H_

#include "board.h"
typedef struct _tag_speed_Cal_Obj{
      float speedtheta;
      float speedlasttheta ;
      float speedthetalastn ;
      int num ;
      float o_speedout_filter ;
      float o_speedout;
}SPEED_CAL;

extern SPEED_CAL g_speed_cal;
 
void Pwm_Duty_Init(void);
void Pwm_TrigFor_interrupt_Init(void);
void timer_init(void);
void rdc_init(void);
#endif

