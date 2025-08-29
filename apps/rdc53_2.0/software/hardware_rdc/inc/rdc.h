#ifndef _HPM_RDC_H
#define _HPM_RDCT_H
#include "board.h"

/**
 * @brief RDC API ;RDC接口函数
 * @addtogroup RDC_API
 * @{
 *
 */

/**
 * @brief     rdc struct
 *            ;RDC结构体
 */
typedef struct _rdc_dac_obj{
int32_t sin_i;
int32_t cos_q;
float rdc_theta;
float sei_theta;
float rdc_speed;
float rdc_speed_filter;
uint8_t acc_cnt_i;
uint8_t acc_cnt_q;
uint8_t rdc_config_ok;
uint8_t flag_once;
float pll_vel;
uint8_t rdc_satus;
}rdc_obj;

/**
 * @brief     RDC error struct
 *            ;rdc异常状态
 */
typedef enum _tag_rdc_fault{
rdc_ok,
rdc_error,
exc_break,
sin_break,
cos_break,
}rdc_status;



/*RDC*/
#define BOARD_RDC_BASE            HPM_RDC
#define BOARD_RDC_TRGM            HPM_TRGM0
#define BOARD_RDC_TRGIGMUX_IN_NUM HPM_TRGM0_INPUT_SRC_RDC_TRGO_0
#define BOARD_RDC_TRG_NUM         HPM_TRGM0_OUTPUT_SRC_MOT_GPIO0

#define BOARD_RDC_TRG_ADC_NUM     TRGM_TRGOCFG_ADCX_PTRGI0A  
#define BOARD_RDC_ADC_I_BASE      HPM_ADC0
#define BOARD_RDC_ADC_Q_BASE      HPM_ADC1
#define BOARD_RDC_ADC_I_CHANNEL       5
#define BOARD_RDC_ADC_Q_CHANNEL       6
#define BOARD_RDC_ADC_IRQn        IRQn_ADC0
#define BOARD_RDC_ADC_TRIG_FLAG   adc16_event_trig_complete
#define BOARD_RDC_ADC_TRG         ADC16_CONFIG_TRG0A

/* DAC section */
#define BOARD_RDC_DAC_I_BASE     HPM_DAC0
#define BOARD_DAC_IRQn           IRQn_DAC0
#define BOARD_APP_DAC_CLOCK_NAME clock_dac0

#define BOARD_RDC_DAC_Q_BASE      HPM_DAC1
#define BOARD_DAC_IRQn1           IRQn_DAC1
#define BOARD_APP_DAC_CLOCK_NAME1 clock_dac1

#define I_DELAY_CONSTANT          5430
#define Q_DELAY_CONSTANT          5430

#define PI2                       6.283184
#define PI                        3.141592  
#define ANGLE_TO_RADIAN           57.2958          

extern  rdc_obj  rdcObj;

/**
 * @brief     RDC Configuration function
 *            ;RDC配置函数
 */
void rdc_cfg(RDC_Type *rdc);

/**
 * @brief     RDC initialize function
 *            ;RDC初始化函数
 */
void rdc_init(void);


/** @} */

#endif /* RDC_H */