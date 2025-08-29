#ifndef PLL_INIT_H
#define PLL_INIT_H
#include "board.h"

/**
 * @brief PLL API ;pll算法接口函数
 * @addtogroup PLL_API
 * @{
 *
 */

/**
 * @brief     speed calculation
 *            ;速度计算结构体
 */
typedef struct _tag_speed_Cal_Obj{
      float speedtheta;
      float speedlasttheta ;
      float speedthetalastn ;
      int num ;
      float o_speedout_filter ;
      float o_speedout;
}speed_Cal_Obj;

/**
 * @brief     pll temp variable value
 *            ;pll算法临时变量
 */
typedef struct {
    float       theta_last; 
    float       err;      
    float       mem;    
}pll_user_t;

/**
 * @brief     pll configure variable value
 *            ;pll算法配置值
 */
typedef struct {
float       sin_val;
float       cos_val;
float       kp; /**< pid kp */
float       ki; /**< pid ki */
float       max_i;    /**< max integral */
float       min_i;    /**< min integral */
float       max_o;    /**< max output */
float       min_o;  /**< min output */
float       theta0; /**< initial angle */
float       loop_in_sec; /**< cycle time in s */
}pll_cfg_t;

/**
 * @brief     pll output value
 *            ;pll算法输出值
 */
typedef struct {
float       speedout;   /**< speed resulting from phase locked loop processing */
float       speedout_filter; 
float       theta;  /**< electrical angle */
}pll_outout_t;

/**
 * @brief     pll struct
 *            ;pll结构体
 */
typedef struct  hpm_smc_pll_para {
pll_cfg_t cfg;
pll_user_t user;
pll_outout_t out;
} pll_para_t;



/**
 * @brief  configuration of pll type ii filter
 *         ;二阶PLL观测器配置参数
 */
typedef struct {
    float k1; /**< adc gain */
    float a;  /**< Filter Zero Coefficient */
    float b;  /**< Filter pole coefficient */
    float c;  /**< Integrator gain factor */
    float sin;
    float cos;
} pll_ii_cfg_t;

/**
 * @brief   Running data of pll type ii filter
 *          ;二阶PLL观测器中间数据
 */
typedef struct {
    float x0;   /**< x stands for the intermediate variable, which the user does not need to care about */
    float x1;
    float x2;
    float x3;
} pll_ii_user_t;


/**
 * @brief   Phase Locked Loop Filter Output Data
 *          ;二阶PLL观测器输出值
 */
typedef struct {
    float theta;
    float speed;
    float speed_filter;
} pll_ii_output_t;

/**
 * @brief   Phase Locked Loop Filter struct
 *          ;二阶PLL结构体
 */
typedef struct{
pll_ii_cfg_t cfg;
pll_ii_user_t user;
pll_ii_output_t out;
}pll_ii_t;


/**
 * @brief     pll params init
 *            ;pll参数初始化
 */
void pll_para_init(pll_para_t *pll);
/**
 * @brief     pll calculate theta/speed
 *            ;pll算法计算角度/速度
 */
void pll_pro(pll_para_t *pll);
/**
 * @brief     pll_ii struct parameter  config
 *            ;二阶PLL参数配置
 */
void pll_type_ii_cfg(pll_ii_t *pll);
/**
 * @brief     speed calculate
 *            ;速度计算
 */
void speed_cal(speed_Cal_Obj* speed_CalHdl );

/**
 * @brief     pll type ii filter function
 *            ;二阶PLL运算
 */
void pll_type_ii_pro(pll_ii_t *pll_ii);

/** @} */

#endif /* PLL_INIT_H */