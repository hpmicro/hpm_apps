/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * RTL8211F-CG 10/100/1000M Ethernet PHY driver
 * Compatible with HPM6750_COMBO RGMII design
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_enet_drv.h"
#include "hpm_rtl8211f_regs.h"
#include "hpm_rtl8211f.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool rtl8211f_check_id(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t id1, id2;

    id1 = enet_read_phy(ptr, phy_addr, RTL8211F_PHYID1);
    id2 = enet_read_phy(ptr, phy_addr, RTL8211F_PHYID2);

    if (RTL8211F_PHYID1_OUI_MSB_GET(id1) == RTL8211F_ID1 && id2 == RTL8211F_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
bool rtl8211f_reset(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t data;
    uint32_t retry_cnt = ENET_PHY_SW_RESET_RETRY_CNT;

    /* PHY reset */
    enet_write_phy(ptr, phy_addr, RTL8211F_BMCR, RTL8211F_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        data = enet_read_phy(ptr, phy_addr, RTL8211F_BMCR);
    } while (RTL8211F_BMCR_RESET_GET(data) && --retry_cnt);

    return retry_cnt > 0 ? true : false;
}

void rtl8211f_basic_mode_default_config(ENET_Type *ptr, rtl8211f_config_t *config)
{
    (void)ptr;

    config->loopback         = false;                        /* Disable PCS loopback mode */
    #if defined(__DISABLE_AUTO_NEGO) && (__DISABLE_AUTO_NEGO)
    config->auto_negotiation = false;                        /* Disable Auto-Negotiation */
    config->speed            = enet_phy_port_speed_1000mbps;
    config->duplex           = enet_phy_duplex_full;
    #else
    config->auto_negotiation = true;                         /* Enable Auto-Negotiation */
    #endif
}

bool rtl8211f_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, rtl8211f_config_t *config)
{
    uint16_t data = 0;

    data |= RTL8211F_BMCR_RESET_SET(0)                        /* Normal operation */
         |  RTL8211F_BMCR_LOOPBACK_SET(config->loopback)      /* configure PCS loopback mode */
         |  RTL8211F_BMCR_ANE_SET(config->auto_negotiation)   /* configure Auto-Negotiation */
         |  RTL8211F_BMCR_PWD_SET(0)                          /* Normal operation */
         |  RTL8211F_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  RTL8211F_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  RTL8211F_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == false) {
        /* Speed encoding: 00=10M, 01=100M, 10=1000M (bits 13, 6) */
        data |= RTL8211F_BMCR_SPEED0_SET((config->speed >> 1) & 0x1U);
        data |= RTL8211F_BMCR_SPEED1_SET(config->speed & 0x1U);
        data |= RTL8211F_BMCR_DUPLEX_SET(config->duplex);    /* Set duplex mode */
    }

    /* check the id of rtl8211f */
    if (rtl8211f_check_id(ptr, phy_addr) == false) {
        return false;
    }

    enet_write_phy(ptr, phy_addr, RTL8211F_BMCR, data);

    return true;
}

void rtl8211f_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status)
{
    uint16_t data;
    uint8_t speed_val;

    data = enet_read_phy(ptr, phy_addr, RTL8211F_PHYSR);
    status->enet_phy_link = RTL8211F_PHYSR_LINK_REAL_TIME_GET(data);
    status->enet_phy_duplex = RTL8211F_PHYSR_DUPLEX_GET(data);

    speed_val = RTL8211F_PHYSR_SPEED_GET(data);
    if (speed_val == 0) {
        status->enet_phy_speed = enet_phy_port_speed_10mbps;
    } else if (speed_val == 1) {
        status->enet_phy_speed = enet_phy_port_speed_100mbps;
    } else {
        status->enet_phy_speed = enet_phy_port_speed_1000mbps;
    }
}
