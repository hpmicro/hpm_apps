/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _BOARD_SPI_OPS_H
#define _BOARD_SPI_OPS_H

#include "hpm_serial_nor_host.h"

/**
 *
 * @brief spi nor flash board APIs; spi nor flash board 接口
 * @defgroup spi nor flash board APIs
 * @{
 */

/**
 * @brief config spi nor flash board;配置板级 spi nor flash 默认配置
 *
 * @param host Pointer to spi nor flash host struct;指向 spi nor flash host 结构体
 *
 * @return return status_success if success
 */
hpm_stat_t serial_nor_get_board_host(hpm_serial_nor_host_t *host);

/**
 * @brief config spi pins;配置 spi 引脚
 *
 * @param spi Pointer to spi struct;指向 spi 结构体
 *
 *
 */
void serial_nor_spi_pins_init(SPI_Type *spi);

/**
 * @}
 */

#endif
