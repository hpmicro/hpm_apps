/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __DRV_SEGMENT_H__
#define __DRV_SEGMENT_H__
#include "board.h"
#include "hpm_eui_drv.h"
#include "hpm_interrupt.h"
#include "hpm_clock_drv.h"

typedef enum
{
    DRV_SEGMENT_CTRL_GE = 0,
    DRV_SEGMENT_CTRL_SHI,
    DRV_SEGMENT_CTRL_BAI,
    DRV_SEGMENT_CTRL_QIAN,
    DRV_SEGMENT_CTRL_WAN,
}drv_segment_ctrl_e;

/* irq config */
typedef void (*drv_irq_cb_func)(void);

void drv_segment_init(drv_irq_cb_func cb);
void drv_segment_ctrl(uint8_t ge, uint8_t shi, uint8_t bai, uint8_t qian, uint8_t wan);

#endif /* __DRV_SEGMENT_H__ */