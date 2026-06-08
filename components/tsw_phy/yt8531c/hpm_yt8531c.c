/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_tsw_drv.h"
#include "hpm_yt8531c_regs.h"
#include "hpm_yt8531c.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool yt8531c_check_id(TSW_Type *ptr, uint8_t port, uint8_t addr)
{
    uint16_t id1, id2;

    tsw_ep_mdio_read(ptr, port, addr, YT8531C_PHYID1, &id1);
    tsw_ep_mdio_read(ptr, port, addr, YT8531C_PHYID2, &id2);

    if (YT8531C_PHYID1_OUI_MSB_GET(id1) == YT8531C_ID1 && YT8531C_PHYID2_OUI_LSB_GET(id2) == YT8531C_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
void yt8531c_reset(TSW_Type *ptr, uint8_t port, uint8_t addr)
{
    uint16_t data;

    /* PHY reset */
    tsw_ep_mdio_write(ptr, port, addr, YT8531C_BMCR, YT8531C_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        tsw_ep_mdio_read(ptr, port, addr, YT8531C_BMCR, &data);
    } while (YT8531C_BMCR_RESET_GET(data));
}

void yt8531c_basic_mode_default_config(TSW_Type *ptr, yt8531c_config_t *config)
{
    (void)ptr;

    config->loopback         = false;                        /* Disable PCS loopback mode */
    #if defined(__DISABLE_AUTO_NEGO) && (__DISABLE_AUTO_NEGO)
    config->auto_negotiation = false;                        /* Disable Auto-Negotiation */
    config->speed            = tsw_phy_port_speed_100mbps;
    config->duplex           = tsw_phy_duplex_full;
    #else
    config->auto_negotiation = true;                         /* Enable Auto-Negotiation */
    #endif
}

bool yt8531c_basic_mode_init(TSW_Type *ptr, uint8_t port, uint8_t addr, yt8531c_config_t *config)
{
    uint16_t data = 0;

    data |= YT8531C_BMCR_RESET_SET(0)                        /* Normal operation */
         |  YT8531C_BMCR_LOOPBACK_SET(config->loopback)      /* configure PCS loopback mode */
         |  YT8531C_BMCR_ANE_SET(config->auto_negotiation)   /* configure Auto-Negotiation */
         |  YT8531C_BMCR_PWD_SET(0)                          /* Normal operation */
         |  YT8531C_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  YT8531C_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  YT8531C_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == 0) {
        data |= YT8531C_BMCR_SPEED0_SET(config->speed);      /* Set port speed */
        data |= YT8531C_BMCR_DUPLEX_SET(config->duplex);     /* Set duplex mode */
    }

    /* check the id of yt8531c */
    if (yt8531c_check_id(ptr, port, addr) == false) {
        return false;
    }

    tsw_ep_mdio_write(ptr, port, addr, YT8531C_BMCR, data);

    return true;
}

void yt8531c_get_phy_status(TSW_Type *ptr, uint8_t port, uint8_t addr, tsw_phy_status_t *status)
{
    uint16_t data;

    tsw_ep_mdio_read(ptr, port, addr, YT8531C_SR, &data);
    status->tsw_phy_link = YT8531C_SR_LINK_STATUS_GET(data);

    if (YT8531C_SR_SPEED_MODE_GET(data) == tsw_phy_port_speed_1000mbps) {
        if (YT8531C_SR_DUPLEX_STATUS_GET(data)) {
            status->tsw_phy_speed = tsw_phy_port_speed_1000mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_full;
        } else {
            status->tsw_phy_speed = tsw_phy_port_speed_1000mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_half;
        }
    } else if (YT8531C_SR_SPEED_MODE_GET(data) == tsw_phy_port_speed_100mbps) {
        if (YT8531C_SR_DUPLEX_STATUS_GET(data)) {
            status->tsw_phy_speed = tsw_phy_port_speed_100mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_full;
        } else {
            status->tsw_phy_speed = tsw_phy_port_speed_100mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_half;
        }
    } else if (YT8531C_SR_SPEED_MODE_GET(data) == tsw_phy_port_speed_10mbps) {
        if (YT8531C_SR_DUPLEX_STATUS_GET(data)) {
            status->tsw_phy_speed = tsw_phy_port_speed_10mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_full;
        } else {
            status->tsw_phy_speed = tsw_phy_port_speed_10mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_half;
        }
    } else {
        
    }
}
