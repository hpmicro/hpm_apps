/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __DRV_MCAN_H__
#define __DRV_MCAN_H__
#include "board.h"
#include "hpm_mcan_drv.h"
#include "drv_log.h"

typedef struct {
    MCAN_Type *can_base;
    uint32_t clock_freq;
    uint32_t irq_num;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    uint32_t ram_base;
    uint32_t ram_size;
#endif
} drv_mcan_info;

typedef drv_mcan_info *drv_mcan_info_t;    

typedef void (*drv_mcan_rx_cb_t)(volatile mcan_rx_message_t *rx_msg);
typedef void (*drv_mcan_tx_cb_t)(void);

void drv_mcan_init(uint32_t baudrate, drv_mcan_rx_cb_t rx_cb, drv_mcan_tx_cb_t tx_cb);
void drv_mcan_send_std(uint8_t rtr, uint8_t dlc, uint32_t id, uint8_t *data);
void drv_mcan_send_ext(uint8_t rtr, uint8_t dlc, uint32_t id, uint8_t *data);
#endif

