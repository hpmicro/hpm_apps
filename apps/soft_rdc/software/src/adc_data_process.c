/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "adc_data_process.h"
#include "adc_dma.h"
#include "math.h"
#include "SEGGER_RTT.h"
#include "hpm_gptmr_drv.h"
static float Sin_Max_Value;
static float Cos_Max_Value;
static float Sin_Min_Value;
static float Cos_Min_Value;
float value1,value2;
MOTOR_CONTROL_Global Motor_Control_Global;
uint16_t adc_data[3][ADC_DMA_BUFF_LEN_IN_BYTES];

#define PI                        3.141592  
float OSin_Integral_Actual;
float OCos_Integral_Actual;
speed_Cal_Obj speed_CalObj;
void RT_Sin_Cos_MAXMIN(uint8_t Sample_Cnt)
{          
    if (Sample_Cnt == 0)
    {
      Sin_Max_Value = Motor_Control_Global.Sample_Sin_Out;
      Sin_Min_Value = Motor_Control_Global.Sample_Sin_Out;
      Cos_Max_Value = Motor_Control_Global.Sample_Cos_Out;
      Cos_Min_Value = Motor_Control_Global.Sample_Cos_Out;
      return;
    }

    if (Motor_Control_Global.Sample_Sin_Out > Sin_Max_Value)
    {
        Sin_Max_Value = Motor_Control_Global.Sample_Sin_Out;
    }
    else if (Motor_Control_Global.Sample_Sin_Out < Sin_Min_Value)
    {
        Sin_Min_Value = Motor_Control_Global.Sample_Sin_Out;
    }

    if (Motor_Control_Global.Sample_Cos_Out > Cos_Max_Value)
    {
        Cos_Max_Value = Motor_Control_Global.Sample_Cos_Out;
    }
    else if (Motor_Control_Global.Sample_Cos_Out < Cos_Min_Value)
    {
        Cos_Min_Value = Motor_Control_Global.Sample_Cos_Out;
    }             
             
}

void rdc_cal_pro(void)
{
    static float OExc_Integral_Last_MAX;
     static float OExc_Integral_MAX;
     static float OExc_Integral_MAX_Point;
     static float OExc_Integral_Last_MIN;
     static float OExc_Integral_MIN;
     static float OExc_Integral_MIN_Point;
     static uint8_t MAX_MIN_Switch;
     static float Sin_Maximu_Value;
     static float Cos_Maximu_Value;
     static float Elec_Angle;
     if ((Motor_Control_Global.Sample_Exc_Out_Last < 0) && (Motor_Control_Global.Sample_Exc_Out >= 0))
      {
            OSin_Integral_Actual = -Sin_Maximu_Value;
            OCos_Integral_Actual = Cos_Maximu_Value;
            MAX_MIN_Switch = 1;
            OExc_Integral_Last_MAX = 0;
            OExc_Integral_Last_MIN = 0;

      }
      else if ((Motor_Control_Global.Sample_Exc_Out_Last > 0) && (Motor_Control_Global.Sample_Exc_Out <= 0))
      {
            OSin_Integral_Actual = Sin_Maximu_Value;
            OCos_Integral_Actual = -Cos_Maximu_Value;
            MAX_MIN_Switch = 2;
            OExc_Integral_Last_MAX = 0;
            OExc_Integral_Last_MIN = 0;

      }
      if (MAX_MIN_Switch == 1)
      {///find oexc_max
          if (Motor_Control_Global.Sample_Exc_Out > OExc_Integral_Last_MAX)
          {
              OExc_Integral_MAX = Motor_Control_Global.Sample_Exc_Out;
              OExc_Integral_Last_MAX = OExc_Integral_MAX;        
              Sin_Maximu_Value = Motor_Control_Global.Sample_Sin_Out;
              Cos_Maximu_Value = Motor_Control_Global.Sample_Cos_Out;
           }
      }
      else if (MAX_MIN_Switch == 2)
      {///find oexc_min
            if (Motor_Control_Global.Sample_Exc_Out < OExc_Integral_Last_MIN)
            {
                OExc_Integral_MIN = Motor_Control_Global.Sample_Exc_Out;
                OExc_Integral_Last_MIN = OExc_Integral_MIN;
                Sin_Maximu_Value = Motor_Control_Global.Sample_Sin_Out;
                Cos_Maximu_Value = Motor_Control_Global.Sample_Cos_Out;
            }             
      }  
            Motor_Control_Global.Sample_Sin_Out_Last = Motor_Control_Global.Sample_Sin_Out;
            Motor_Control_Global.Sample_Cos_Out_Last = Motor_Control_Global.Sample_Cos_Out;//filter
            Motor_Control_Global.Sample_Exc_Out_Last = Motor_Control_Global.Sample_Exc_Out;//            
}
void adc_data_calculat(void)
{
    for (int i = 0;i < ADC_DMA_BUFF_LEN_IN_BYTES;i++)
    {
      Motor_Control_Global.Sample_OSin[i] = adc_data[0][i] >> 4;
      Motor_Control_Global.Sample_OCos[i] = adc_data[1][i] >> 4;
      Motor_Control_Global.Sample_OExc[i] = adc_data[2][i] >> 4;
      Motor_Control_Global.Sample_Sin[i] = (float)Motor_Control_Global.Sample_OSin[i]-2056;//直流偏置
      Motor_Control_Global.Sample_Exc[i] = (float)Motor_Control_Global.Sample_OExc[i]-2500;
      Motor_Control_Global.Sample_Cos[i] = (float)Motor_Control_Global.Sample_OCos[i]-2053;  
      Motor_Control_Global.Sample_Sin_Out = Motor_Control_Global.Sample_Sin_Out_Last + (float)SIN_COS_COF * (float)(Motor_Control_Global.Sample_Sin[i] - Motor_Control_Global.Sample_Sin_Out_Last);
      Motor_Control_Global.Sample_Cos_Out = Motor_Control_Global.Sample_Cos_Out_Last + (float)SIN_COS_COF * (float)(Motor_Control_Global.Sample_Cos[i] - Motor_Control_Global.Sample_Cos_Out_Last);   
      Motor_Control_Global.Sample_Exc_Out = Motor_Control_Global.Sample_Exc_Out_Last + (float) SIN_COS_COF *(float)(Motor_Control_Global.Sample_Exc[i] - Motor_Control_Global.Sample_Exc_Out_Last);
      int32_t osin =  Motor_Control_Global.Sample_Sin_Out;
      int32_t sin = OSin_Integral_Actual;
      rdc_cal_pro();
    } 
    #if sin_cos_rtt_print
    ///////////////包络线//////////////////////////////////////
        int32_t osin =  Motor_Control_Global.Sample_Sin_Out;
        int32_t ocos =  Motor_Control_Global.Sample_Cos_Out;
        int32_t oexc =  Motor_Control_Global.Sample_Exc_Out;
        int32_t  sin = OSin_Integral_Actual;//包络线
        int32_t  cos = OCos_Integral_Actual;//包络线
        SEGGER_RTT_printf(0, "%d,%d\r\n",sin,cos); 
    //////////////////////////////////////////////////////////
    #endif
        float theta = (atan2f((float)(OSin_Integral_Actual),(float)(OCos_Integral_Actual))*180/PI);
        if(theta < 0)
        {
           theta = 360+ theta;
        }   
        speed_CalObj.speedtheta = theta;
}

void adc_data_process(void)
{
   if (adc_done[0] == 1)
   {
     /*you can process the adc data here*/
      adc_done[0] = 0;
      memcpy(&adc_data[0][0], &adc_buff_u[0], ADC_DMA_BUFF_LEN_IN_BYTES * sizeof(uint16_t));
      memcpy(&adc_data[1][0], &adc_buff_v[0], ADC_DMA_BUFF_LEN_IN_BYTES * sizeof(uint16_t));
      memcpy(&adc_data[2][0], &adc_buff_w[0], ADC_DMA_BUFF_LEN_IN_BYTES * sizeof(uint16_t));
      adc_data_calculat();
   }
   if (adc_done[1] == 1)
   {
      adc_done[1] = 0;
      /*you can process the adc data here*/
      memcpy(&adc_data[0][0], &adc_buff_u[ADC_DMA_BUFF_LEN_IN_BYTES], ADC_DMA_BUFF_LEN_IN_BYTES * sizeof(uint16_t));
      memcpy(&adc_data[1][0], &adc_buff_v[ADC_DMA_BUFF_LEN_IN_BYTES], ADC_DMA_BUFF_LEN_IN_BYTES * sizeof(uint16_t));
      memcpy(&adc_data[2][0], &adc_buff_w[ADC_DMA_BUFF_LEN_IN_BYTES], ADC_DMA_BUFF_LEN_IN_BYTES * sizeof(uint16_t));
      adc_data_calculat();
   }
}


void speed_cal(speed_Cal_Obj* speed_CalHdl )
{
    float delta = 0;    
    delta = speed_CalHdl->speedtheta - speed_CalHdl->speedlasttheta; 
    if (delta > (180)) {/*-speed*/
        delta = -(360) +speed_CalHdl->speedtheta -speed_CalHdl->speedlasttheta;
    } else if (delta < (-180)) {/*+speed*/
        delta = ((360) - speed_CalHdl->speedlasttheta + speed_CalHdl->speedtheta ) ;
    }
  
    speed_CalHdl->speedthetalastn += delta;
    speed_CalHdl->speedlasttheta = speed_CalHdl->speedtheta;
    speed_CalHdl->num++;
    
    if (10 == speed_CalHdl->num) {

        speed_CalHdl->num = 0;
       
        speed_CalHdl->o_speedout = speed_CalHdl->speedthetalastn/
            (0.001* 1* 360);
         speed_CalHdl->o_speedout_filter = speed_CalHdl->o_speedout_filter + 0.02*
            (speed_CalHdl->o_speedout - speed_CalHdl->o_speedout_filter);
        
         speed_CalHdl->speedthetalastn = 0;
       
    }
}





void timer_init(void)
{
    gptmr_channel_config_t config;
    clock_add_to_group(BOARD_BLDC_TMR_CLOCK, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.cmp[0] = BOARD_BLDC_1MS_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_1MS_RELOAD+1;

    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);

}

void isr_gptmr(void)
{
  
    volatile uint32_t s = BOARD_BLDC_TMR_1MS->SR;
    BOARD_BLDC_TMR_1MS->SR = s;

    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) 
    {
        speed_cal(&speed_CalObj);
        int32_t theta = speed_CalObj.speedtheta*100;
        int32_t speed = speed_CalObj.o_speedout_filter*100;
        SEGGER_RTT_printf(0, "%d,%d\r\n",theta,speed); 
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
