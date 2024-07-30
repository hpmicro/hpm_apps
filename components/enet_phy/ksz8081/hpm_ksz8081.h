/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ksz8081_H
#define HPM_ksz8081_H

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "stdint.h"

/*---------------------------------------------------------------------*
 *  Macro Const Definitions
 *---------------------------------------------------------------------*/
#define PHY_ADDR (2U)
#define PHY_ID1  (0x22U)
#define PHY_ID2  (0x5U)

/*---------------------------------------------------------------------*
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------*/
typedef struct {
    bool loopback;
    uint8_t speed;
    bool auto_negotiation;
    uint8_t duplex_mode;
} ksz8081_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------*
 * Exported Functions
 *---------------------------------------------------------------------*/
uint16_t ksz8081_check(ENET_Type *ptr, uint32_t addr);
void ksz8081_reset(ENET_Type *ptr);
void ksz8081_basic_mode_default_config(ENET_Type *ptr, ksz8081_config_t *config);
bool ksz8081_basic_mode_init(ENET_Type *ptr, ksz8081_config_t *config);
void ksz8081_read_status(ENET_Type *ptr);
void ksz8081_control_config(ENET_Type *ptr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_ksz8081_H */