#ifndef _HPM_SEI_SLAVE_TAMAGAWA_H_
#define _HPM_SEI_SLAVE_TAMAGAWA_H_

/**
 * @brief TAMAGAWA_SLAVE_API ;多摩川从模式函数
 * @addtogroup TAMAGAWA_SLAVE_API
 * @{
 *
 */

/**
 * @brief      get pos in TAMAGAWA slave mode  
 *            ;多摩川从模式下SEI更新的角度信息
 */
void sei_slave_tamagawa_pos_get(void);
/**
 * @brief      initialize TAMAGAWA slave mode  
 *            ;初始化配置多摩川从模式
 */
void sei_slave_tamagawa_init(void);

/** @} */

#endif