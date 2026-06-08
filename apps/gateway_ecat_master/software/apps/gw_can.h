/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_CAN_GW_CAN_THREAD_H_
#define APPLICATIONS_CAN_GW_CAN_THREAD_H_
#include "board.h"

typedef struct
{
    struct {
        uint32_t tx_num: 16;
        uint32_t dlc: 4; 
        uint32_t rtr: 1;        
        uint32_t id: 11;
    };
    uint8_t data[8];
}__attribute__((packed)) gw_can_tx_std;

typedef gw_can_tx_std* gw_can_tx_std_t;

typedef struct
{
    struct {
        uint16_t dlc: 4; 
        uint16_t rtr: 1;        
        uint16_t id: 11;
    };
    uint8_t data[8];
}__attribute__((packed)) gw_can_rx_std;

typedef gw_can_rx_std* gw_can_rx_std_t;

typedef struct
{
    struct {
        uint64_t tx_num: 16;
        uint64_t dlc: 16; 
        uint64_t id: 29;
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
        uint32_t id: 29;
        uint32_t res: 1;
        uint32_t rtr: 1; 
        uint32_t std_ext_bit: 1;
    };
    uint8_t data[8];
}__attribute__((packed)) gw_can_rx_ext;


typedef gw_can_rx_ext* gw_can_rx_ext_t;


#endif /* APPLICATIONS_CAN_GW_CAN_THREAD_H_ */
