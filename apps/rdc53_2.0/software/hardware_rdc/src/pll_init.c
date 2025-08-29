#include "pll_init.h"
#include "rdc.h"
#include "math.h"
#include "hpm_gptmr_drv.h"
#include "rdc_cfg.h"
#include "hpm_gpio_drv.h"

void pll_para_init(pll_para_t *pll)
{
    pll->cfg.kp =0.00005;
    pll->cfg.ki = 0.000005;
    pll->cfg.loop_in_sec = 0.0001;
    pll->cfg.theta0 = 0;
    pll->cfg.max_i = 500;
    pll->cfg.min_i = -500;
    pll->cfg.max_o = 500;
    pll->cfg.min_o = -500;
}

void pll_pro(pll_para_t *pll)
{
    float portion_asp = 0;
    float portion_asi = 0;

    pll->user.err = pll->cfg.sin_val* cosf(pll->user.theta_last) -
             pll->cfg.cos_val* sinf(pll->user.theta_last);
    portion_asp = pll->cfg.kp * pll->user.err;
    portion_asi = pll->cfg.ki * pll->user.err;
    portion_asi += pll->user.mem;

    if (portion_asi > pll->cfg.max_i) {
        portion_asi = pll->cfg.max_i;
    } else if (portion_asi < pll->cfg.min_i) {
        portion_asi = pll->cfg.min_i;
    }
    pll->user.mem = portion_asi;
    pll->out.speedout = (portion_asi + portion_asp);
    pll->out.speedout_filter = pll->out.speedout_filter + 0.03*
            (pll->out.speedout - pll->out.speedout_filter);
    if (pll->out.speedout > pll->cfg.max_o) {
        pll->out.speedout = pll->cfg.max_o;
    } else if (pll->out.speedout < pll->cfg.min_o) {
        pll->out.speedout = pll->cfg.min_o;
    }

    pll->user.theta_last += pll->out.speedout * pll->cfg.loop_in_sec;
 
    pll->user.theta_last = fmodf(pll->user.theta_last, PI*2);

    pll->out.theta =  fmodf(pll->user.theta_last * 57.29578 , 360);
   
    if(pll->out.theta<0)
    {
      pll->out.theta = pll->out.theta+360;
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
         //0.04
         speed_CalHdl->o_speedout_filter = speed_CalHdl->o_speedout_filter + 0.03*
            (speed_CalHdl->o_speedout - speed_CalHdl->o_speedout_filter);
        
         speed_CalHdl->speedthetalastn = 0;
       
    }
}


void pll_type_ii_cfg(pll_ii_t *pll)
{
   pll->cfg.k1 = 0.8;
   pll->cfg.a = -0.999969482421875;
   pll->cfg.b = -0.999664306640625;
   pll->cfg.c = 0.0001;  
}

void pll_type_ii_pro(pll_ii_t *pll_ii)
{
    /* Define intermediate variables a0 to a4 for temporary storage during calculations. */
    float a0, a1, a2, a3, a4;

    /* Calculate a0 based on the current phase estimate and the sine and cosine components of the input signal. */
    a0 = cosf(pll_ii->user.x3) * pll_ii->cfg.k1 * pll_ii->cfg.sin - sinf(pll_ii->user.x3) * pll_ii->cfg.k1 * pll_ii->cfg.cos;
    
   
    pll_ii->user.x0 += a0;
    a1 = pll_ii->user.x0 * pll_ii->cfg.c;

    a2 = a1 - pll_ii->user.x1 * pll_ii->cfg.b;
    a3 = pll_ii->user.x1 * pll_ii->cfg.a + a2; 

    /* Update the state variable x2 by integrating a3. */
    pll_ii->user.x2 += a3;
    a4 = pll_ii->user.x2 * pll_ii->cfg.c;
    
    /*  Update the state variables x1 and x3 for the next iteration. */
    pll_ii->user.x1 = a2;
    pll_ii->user.x3 = fmodf(a4, PI*2);

    /* Store the computed phase and speed in the output structure. */

    pll_ii->out.theta  =  fmodf(pll_ii->user.x3 * 57.29578 , 360);  
    if(pll_ii->out.theta <0)
    {
      pll_ii->out.theta  = pll_ii->out.theta+360;
    }
    pll_ii->out.speed = a3;

    pll_ii->out.speed_filter = pll_ii->out.speed_filter + 0.015*
            (pll_ii->out.speed - pll_ii->out.speed_filter);
}




