/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_CAN_GW_CAN_THREAD_H_
#define APPLICATIONS_CAN_GW_CAN_THREAD_H_
#include "board.h"
#include "drv_log.h"
#include "drv_mcan.h"
#include "gw_mq.h"
#include "gw_mq_msg.h"

typedef struct
{
    struct {
        uint32_t tx_num: 16;
        uint32_t dlc: 4; 
        uint32_t rtr: 1;        
        uint32_t std_id: 11;
    };
    uint8_t data[8];
}gw_can_tx_std;

typedef gw_can_tx_std* gw_can_tx_std_t;

typedef struct
{
    struct {
        uint16_t dlc: 4; 
        uint16_t rtr: 1;        
        uint16_t std_id: 11;
    };
    uint8_t data[8];
}gw_can_rx_std;

typedef gw_can_rx_std* gw_can_rx_std_t;

typedef struct
{
    struct {
        uint64_t tx_num: 16;
        uint64_t dlc: 16; 
        uint64_t ext_id: 29;
        uint64_t res: 1;
        uint64_t rtr: 1; 
        uint64_t std_ext_bit: 1;
    };
    uint8_t data[8];
}__attribute__((packed)) gw_can_tx_ext;

typedef gw_can_tx_ext* gw_can_tx_ext_t;

typedef struct
{ 
    uint16_t dlc;
    struct {    
        uint32_t ext_id: 29;
        uint32_t res: 1;
        uint32_t rtr: 1; 
        uint32_t std_ext_bit: 1;
    };
    uint8_t data[8];
}__attribute__((packed)) gw_can_rx_ext;


typedef gw_can_rx_ext* gw_can_rx_ext_t;


extern volatile uint32_t can_std_cyc;
extern volatile uint32_t can_ext_cyc;
extern volatile uint32_t can_std_cyc_cnt;
extern volatile uint32_t can_ext_cyc_cnt;

void gw_can_thread(void *argument);
extern void *gateway_apps_get_txpdo_std(uint8_t *num);
extern void *gateway_apps_get_txpdo_ext(uint8_t *num);
extern void *gateway_apps_get_rxpdo_std(uint8_t *num);
extern void *gateway_apps_get_rxpdo_ext(uint8_t *num);

#endif /* APPLICATIONS_CAN_GW_CAN_THREAD_H_ */
