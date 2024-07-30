#include "libcmdtest.h"

//#define POS_MODE 1
CMDGENE_PARA cmdpar;
double poscmd[5000]={0};
double velcmd[5000]={0};
uint32_t ii = 0;

/**
 * @brief            timer init
 *                  ;定时器初始化
 */
void timer_init(void)
{
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(BOARD_BLDC_TMR_MS, &config);
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD+1;

    gptmr_enable_irq(BOARD_BLDC_TMR_MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 2);

}

/**
 * @brief            config trajectory planning parameters
 *                  ;配置轨迹规划参数
 */
void lib_params_cfg(void)
{
    cmdpar.cmdgene_in_par.poscfgpar.q0 = 0;
    cmdpar.cmdgene_in_par.poscfgpar.q1 = MOVE_BASOLUTE_POS_R;
    cmdpar.cmdgene_in_par.poscfgpar.v0 = 0;
    cmdpar.cmdgene_in_par.poscfgpar.v1 = 0;
    cmdpar.cmdgene_in_par.poscfgpar.vmax = MOVE_SPEED;
    cmdpar.cmdgene_in_par.poscfgpar.amax = MOVE_ACC;
    cmdpar.cmdgene_in_par.poscfgpar.jmax = MOVE_JERK;
    cmdpar.cmdgene_in_par.poscfgpar.cyclecnt = CYCLE_CNT;
    cmdpar.cmdgene_in_par.poscfgpar.cycletype = MULTIPLE_MOVE_TYPE;
    cmdpar.cmdgene_in_par.poscfgpar.dwelltime = DWELLTIME_MS;
    cmdpar.cmdgene_in_par.poscfgpar.isr_time_s = ISR_TIME_S;

    cmdpar.cmdgene_in_par.velcfgpar.q0 = 0;
    cmdpar.cmdgene_in_par.velcfgpar.Tv = CONSTANT_SPEED_TIME_S;
    cmdpar.cmdgene_in_par.velcfgpar.v0 = 0;
    cmdpar.cmdgene_in_par.velcfgpar.v1 = 0;
    cmdpar.cmdgene_in_par.velcfgpar.vmax =MOVE_SPEED;
    cmdpar.cmdgene_in_par.velcfgpar.amax = MOVE_ACC;
    cmdpar.cmdgene_in_par.velcfgpar.jmax = MOVE_JERK;
    cmdpar.cmdgene_in_par.velcfgpar.isr_time_s = ISR_TIME_S;

}

/**
 * @brief            trajectory planning function call
 *                  ;轨迹规划调用函数
 */
void libcall(void)
{
#ifdef POS_MODE
//位置模式下，轨迹规划函数生成位置/速度指令
   pos_cmd_gene(&cmdpar);
#else 
//速度模式下，轨迹规划函数生成速度指令
   vel_cmd_gene(&cmdpar);
#endif
}

/**
 * @brief            ISR 
 *                  ;中断函数内调用轨迹规划函数
 */
void isr_gptmr(void)
{
  
    volatile uint32_t s = BOARD_BLDC_TMR_MS->SR;
    BOARD_BLDC_TMR_MS->SR = s;

    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) 
    {
       libcall();
       if(ii==5000)
       {
             return;
       }     
       poscmd[ii] = cmdpar.cmdgene_out_par.poscmd;
       velcmd[ii] = cmdpar.cmdgene_out_par.velcmd;
       ii++;
    
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)

int main (void)
{

   board_init();
   lib_params_cfg();
   timer_init();
   
}