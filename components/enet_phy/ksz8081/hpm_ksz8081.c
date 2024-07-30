/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "hpm_enet_drv.h"
#include "hpm_ksz8081_regs.h"
#include "hpm_ksz8081.h"
#include "board.h"

/*---------------------------------------------------------------------*
 * Internal API
 *---------------------------------------------------------------------*/
static bool ksz8081_id_check(ENET_Type *ptr)
{
    uint16_t id1, id2;

    id1 = enet_read_phy(ptr, PHY_ADDR, KSZ8081_PHY_ID1_REG);
    id2 = enet_read_phy(ptr, PHY_ADDR, KSZ8081_PHY_ID2_REG);

    if (KSZ8081_PHYID1_OUI_MSB_GET(id1) == PHY_ID1 && KSZ8081_PHYID2_OUI_MSB_GET(id2) == PHY_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------*
 * API
 *---------------------------------------------------------------------*/
uint16_t ksz8081_register_check(ENET_Type *ptr, uint32_t addr)
{
   return enet_read_phy(ptr, PHY_ADDR, addr);
}

void ksz8081_reset(ENET_Type *ptr)
{
    uint16_t data;

    /* PHY reset */
    enet_write_phy(ptr, PHY_ADDR, KSZ8081_PHY_BASICCONTROL_REG, KSZ8081_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        data = enet_read_phy(ptr, PHY_ADDR, KSZ8081_PHY_BASICCONTROL_REG);
    } while (KSZ8081_BMCR_RESET_GET(data));
}

void ksz8081_basic_mode_default_config(ENET_Type *ptr, ksz8081_config_t *config)
{
    config->loopback         = 1; /* Enable PCS loopback mode */
    config->speed            = 1; /* reserved:3/2; 100mbps: 1; 10mbps: 0 */
    config->auto_negotiation = 1; /* Enable Auto-Negotiation */
    config->duplex_mode      = 1; /* Full duplex mode */
}

bool ksz8081_basic_mode_init(ENET_Type *ptr, ksz8081_config_t *config)
{
    uint16_t data = 0;
    uint16_t bssReg = 0xFF, ctlReg = 0xFF;
    /* check the id of ksz8081 */
    if (ksz8081_id_check(ptr) == false) {
        return false;
    }

    ksz8081_reset(ptr);

    //50M mode
    data = enet_read_phy(ptr, PHY_ADDR, KSZ8081_PHY_CONTROL2_REG);
    enet_write_phy(ptr, PHY_ADDR, KSZ8081_PHY_CONTROL2_REG, (data | KSZ8081_PHY_CTL2_REFCLK_SELECT_MASK));

    // auto mode
    data = (KSZ8081_PHY_100BASETX_FULLDUPLEX_MASK | KSZ8081_PHY_100BASETX_HALFDUPLEX_MASK |
        KSZ8081_PHY_10BASETX_FULLDUPLEX_MASK | KSZ8081_PHY_10BASETX_HALFDUPLEX_MASK | 0x1U);
    enet_write_phy(ptr, PHY_ADDR, KSZ8081_PHY_AUTONEG_ADVERTISE_REG, data);

    enet_write_phy(ptr, PHY_ADDR, KSZ8081_PHY_BASICCONTROL_REG, (KSZ8081_PHY_BCTL_AUTONEG_MASK | KSZ8081_PHY_BCTL_RESTART_AUTONEG_MASK));

    while (1)
    {
        bssReg = enet_read_phy(ptr, PHY_ADDR, KSZ8081_PHY_BASICSTATUS_REG);
        ctlReg = enet_read_phy(ptr, PHY_ADDR, KSZ8081_PHY_CONTROL1_REG);
        if (((bssReg & KSZ8081_PHY_BSTATUS_AUTONEGCOMP_MASK) == 0U) && ((ctlReg & KSZ8081_PHY_LINK_READY_MASK) == 0U))
            break;
    }
    printf("phy init success!\n");
    return true;
}


void ksz8081_init_auto_negotiation(void)
{
}
