/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __DRV_OLED_H__
#define __DRV_OLED_H__
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_debug_console.h"
#include "hpm_spi_drv.h"
#include "hpm_gpio_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "drv_log.h"
#if USE_U8G2
#include "u8g2.h"
#endif

#define DRV_OLED_RES_RESET()  gpio_write_pin(BOARD_OLED_RESET_CTRL, BOARD_OLED_RESET_INDEX, BOARD_OLED_RESET_PIN, false)
#define DRV_OLED_RES_SET()  gpio_write_pin(BOARD_OLED_RESET_CTRL, BOARD_OLED_RESET_INDEX, BOARD_OLED_RESET_PIN, true)

#define DRV_OLED_DC_RESET()  gpio_write_pin(BOARD_OLED_D_C_CTRL, BOARD_OLED_D_C_INDEX, BOARD_OLED_D_C_PIN, false)
#define DRV_OLED_DC_SET()  gpio_write_pin(BOARD_OLED_D_C_CTRL, BOARD_OLED_D_C_INDEX, BOARD_OLED_D_C_PIN, true)
 		     
#define DRV_OLED_BS0_RESET()  gpio_write_pin(BOARD_OLED_BS0_CTRL, BOARD_OLED_BS0_INDEX, BOARD_OLED_BS0_PIN, false)
#define DRV_OLED_BS0_SET()  gpio_write_pin(BOARD_OLED_BS0_CTRL, BOARD_OLED_BS0_INDEX, BOARD_OLED_BS0_PIN, true)

#define DRV_OLED_BS1_RESET()  gpio_write_pin(BOARD_OLED_BS1_CTRL, BOARD_OLED_BS1_INDEX, BOARD_OLED_BS1_PIN, false)
#define DRV_OLED_BS1_SET()  gpio_write_pin(BOARD_OLED_BS1_CTRL, BOARD_OLED_BS1_INDEX, BOARD_OLED_BS1_PIN, true)
 		     

typedef enum
{
    DRV_OLED_CMD = 0,
    DRV_OLED_DATA
}drv_oled_ctrl_e;

#if USE_U8G2
extern u8g2_t u8g2;
#endif

void drv_oled_show_count(uint32_t count);
void drv_oled_init(void);

#endif /* __DRV_OLED_H__ */
