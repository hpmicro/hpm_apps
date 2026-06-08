/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_YT8531C_H
#define HPM_YT8531C_H

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_tsw_phy.h"
#include "hpm_common.h"
#include "hpm_tsw_regs.h"
/*---------------------------------------------------------------------
 *  Macro Const Definitions
 *---------------------------------------------------------------------
 */
#ifndef YT8531C_P1_ADDR
#define YT8531C_P1_ADDR (1U)
#endif

#ifndef YT8531C_P2_ADDR
#define YT8531C_P2_ADDR (1U)
#endif

#define YT8531C_ID1  (0x4F51U)
#define YT8531C_ID2  (0x3AU)

/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
typedef struct {
    bool loopback;
    uint8_t speed;
    bool auto_negotiation;
    uint8_t duplex;
} yt8531c_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------
 */
void yt8531c_reset(TSW_Type *ptr, uint8_t port, uint8_t addr);
void yt8531c_basic_mode_default_config(TSW_Type *ptr, yt8531c_config_t *config);
bool yt8531c_basic_mode_init(TSW_Type *ptr, uint8_t port, uint8_t addr, yt8531c_config_t *config);
void yt8531c_get_phy_status(TSW_Type *ptr, uint8_t port, uint8_t addr, tsw_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_YT8531C_H */
