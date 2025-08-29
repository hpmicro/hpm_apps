#ifndef _SPI_INIT_H
#define _SPI_INIT_H

/**
 * @brief SPI API ;SPI接口函数
 * @addtogroup PSI_API
 * @{
 *
 */

/**
 * @brief     data package
 *            ;数据打包
 */
void  spi_data_package(void);
/**
 * @brief     data trans
 *            ;数据传输
 */
void spi_trans_data(void);

/**
 * @brief     spi config
 *            ;spi从站中断模式配置
 */
int spi_salve_interrupt_config(void);

/** @} */

#endif