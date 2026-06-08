/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef DRV_ENET_TSN_H_
#define DRV_ENET_TSN_H_
#include "board.h"
#include "hpm_tsw_drv.h"
#include "hpm_enet_drv.h"
#include "hpm_l1c_drv.h"
#include "drv_log.h"
#include "drv_log.h"

#define ENET_TX_BUFF_COUNT  (20U)
#define ENET_RX_BUFF_COUNT  (30U)
#define ENET_TX_BUFF_SIZE   (1536U)
#define ENET_RX_BUFF_SIZE   (1536U)

extern enet_desc_t desc;
extern uint8_t mac[_ENET_PORT_COUNT][ENET_MAC_SIZE];

void drv_enet_tsn_init(void);

#endif /* DRV_ENET_TSN_H_ */
