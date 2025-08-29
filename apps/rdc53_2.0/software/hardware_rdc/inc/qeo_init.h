#ifndef _HPM_QEO_CONFIG_H
#define _HPM_QEO_CONFIG_H
#include "board.h"

/**
 * @brief QEO API ;QEO接口函数
 * @addtogroup QEO_API
 * @{
 *
 */

/**
 * @brief     abz lines for one cycle
 *            一圈对应ABZ线数
 */
#define TEST_QEO_ABZ_LINES    (1024U)

/**
 * @brief     QEO max freq
 *            QEO输出最大频率
 */
#define TEST_QEO_ABZ_MAX_FREQ   10000//(20000000U)

/**
 * @brief     max position
 *            最大位置
 */
#define QEO_POSITION_MAX_VAL  (0x100000000UL)

/**
 * @brief     position update frequency
 *            QEO输出最大频率
 */
#define POS_UPDATE_FREQ        (10000)

/**
 * @brief     QEO struct
 *            ;QEO转ABZ输出结构体
 */
typedef struct _qeo_abz_Obj{
    int64_t previous_pos;
    int64_t current_pos;
    float vel;
    uint32_t qeo_lines ;
    uint16_t sample_freq;
}qeo_abz_Obj;

/**
 * @brief     position abz mode config
 *            ;ABZ模式输出位置配置
 */
void qeo_gen_abz_signal_hardware(void);
/**
 * @brief      abz mode output 
 *            ;ABZ模式输出位置配置
 */
void qeo_abz_output(qeo_abz_Obj* qeo_abzHdl);

/** @} */

#endif
