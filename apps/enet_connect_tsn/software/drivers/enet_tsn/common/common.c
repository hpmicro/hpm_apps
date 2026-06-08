/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
#include "hpm_common.h"
#include "hpm_otp_drv.h"
#include "ethernetif.h"
#include "netconf.h"
#include "drv_enet_tsn.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "osal.h"
#include "drv_enet_tsn.h"

static tsw_phy_status_t last_status_p1 = {.tsw_phy_link = tsw_phy_link_unknown};
static tsw_phy_status_t last_status_p2 = {.tsw_phy_link = tsw_phy_link_unknown};
static uint8_t dhcp_last_state[_ENET_PORT_COUNT] = {
    DHCP_STATE_OFF,
    #if _ENET_PORT_COUNT == 2
    DHCP_STATE_OFF,
    #endif
};

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag;
#endif

#if defined(NO_SYS) && !NO_SYS
extern osSemaphoreId_t s_xSemaphore;

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
void timer_callback(TimerHandle_t xTimer)
{
   (void)xTimer;

   enet_self_adaptive_port_speed();
}
#endif
#endif

#if defined(LWIP_DHCP) && LWIP_DHCP
void enet_update_dhcp_state(struct netif *netif)
{
    struct dhcp *dhcp = netif_dhcp_data(netif);
    char state_str[32] = {0};

    if (netif_is_link_up(netif) == false) {
        return;
    }

    int idx = netif_get_index(netif) - 1;
    if (idx < 0 || idx >= _ENET_PORT_COUNT) {
        return;
    }

    if (dhcp == NULL) {
        dhcp_last_state[idx] = DHCP_STATE_OFF;
    } else if (dhcp_last_state[idx] != dhcp->state) {
        dhcp_last_state[idx] = dhcp->state;

        switch (dhcp_last_state[idx]) {
        case DHCP_STATE_OFF:
            strcpy(state_str, "OFF");
            break;
        case DHCP_STATE_REQUESTING:
            strcpy(state_str, "REQUESTING");
            break;
        case DHCP_STATE_INIT:
            strcpy(state_str, "INIT");
            break;
        case DHCP_STATE_REBOOTING:
            strcpy(state_str, "REBOOTING");
            break;
        case DHCP_STATE_REBINDING:
            strcpy(state_str, "REBINDING");
            break;
        case DHCP_STATE_RENEWING:
            strcpy(state_str, "RENEWING");
            break;
        case DHCP_STATE_SELECTING:
            strcpy(state_str, "SELECTING");
            break;
        case DHCP_STATE_INFORMING:
            strcpy(state_str, "INFORMING");
            break;
        case DHCP_STATE_CHECKING:
            strcpy(state_str, "CHECKING");
            break;
        case DHCP_STATE_BOUND:
            strcpy(state_str, "BOUND");
            break;
        case DHCP_STATE_BACKING_OFF:
            strcpy(state_str, "BACKING_OFF");
            break;
        default:
            snprintf(state_str, sizeof(state_str), "%u", dhcp_last_state[idx]);
            break;
        }
        LOG_I("netif %d DHCP State: %s\r\n", idx, state_str);
        netif_user_notification(netif);
    }
}
#endif

ATTR_WEAK uint8_t enet_get_mac_address(uint8_t *mac, uint8_t idx)
{
    char *p = NULL;
    char *token;
    uint8_t i=0;

    if (mac == NULL) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    /* load MAC address from MACRO definitions */
    p = mac_init[idx].mac_addr;
    token = strtok(p, ":");
    mac[i] = strtol(token, NULL, 16);
    while (token != NULL && ++i < ENET_MAC_SIZE) {
        token = strtok(NULL, ":");
        mac[i] = strtol(token, NULL, 16);
    }

    if (i < ENET_MAC_SIZE) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    return ENET_MAC_ADDR_FROM_MACRO;
}

bool enet_get_dhcp_ready_status(uint8_t idx)
{
    return (dhcp_last_state[idx] == DHCP_STATE_BOUND) ? true : false;
}

void enet_self_adaptive_port_speed(void)
{
    tsw_phy_status_t status = {0};

    tsw_port_speed_t line_speed[] = {tsw_port_speed_10mbps, tsw_port_speed_100mbps, tsw_port_speed_1000mbps};
    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};

    yt8531c_get_phy_status(BOARD_TSN, BOARD_TSN_P1_PORT, YT8531C_P1_ADDR, &status);

    if (status.tsw_phy_link || (status.tsw_phy_link != last_status_p1.tsw_phy_link)) {
        if (memcmp(&last_status_p1, &status, sizeof(tsw_phy_status_t)) != 0) {
            memcpy(&last_status_p1, &status, sizeof(tsw_phy_status_t));
            if (status.tsw_phy_link) {
                LOG_I("P1 Link Status: Up\r\n");
                LOG_I("P1 Link Speed:  %s\r\n", speed_str[status.tsw_phy_speed]);
                LOG_I("P1 Link Duplex: %s\r\n", duplex_str[status.tsw_phy_duplex]);

                tsw_set_port_speed(BOARD_TSN, BOARD_TSN_P1_PORT, line_speed[status.tsw_phy_speed]);

                if (status.tsw_phy_speed != tsw_phy_port_speed_1000mbps) {
                    tsw_enable_store_forward_mode(BOARD_TSN, BOARD_TSN_P1_PORT);
                } else {
                    tsw_disable_store_forward_mode(BOARD_TSN, BOARD_TSN_P1_PORT);
                }

                if (!status.tsw_phy_duplex) {
                    LOG_E("Error: PHY is in half duplex now, but TSW MAC supports only full duplex mode!\r\n");
                    return;
                }
                netif_set_link_up(netif_get_by_index(1));
            } else {
                LOG_I("P1 Link Status: Down\r\n");
                netif_set_link_down(netif_get_by_index(1));
            }
        }
    }

    yt8531c_get_phy_status(BOARD_TSN, BOARD_TSN_P2_PORT, YT8531C_P2_ADDR, &status);
    if (status.tsw_phy_link || (status.tsw_phy_link != last_status_p2.tsw_phy_link)) {
        if (memcmp(&last_status_p2, &status, sizeof(tsw_phy_status_t)) != 0) {
            memcpy(&last_status_p2, &status, sizeof(tsw_phy_status_t));
            if (status.tsw_phy_link) {
                LOG_I("P2 Link Status: Up\r\n");
                LOG_I("P2 Link Speed:  %s\r\n", speed_str[status.tsw_phy_speed]);
                LOG_I("P2 Link Duplex: %s\r\n", duplex_str[status.tsw_phy_duplex]);

                tsw_set_port_speed(BOARD_TSN, BOARD_TSN_P2_PORT, line_speed[status.tsw_phy_speed]);

                if (status.tsw_phy_speed != tsw_phy_port_speed_1000mbps) {
                    tsw_enable_store_forward_mode(BOARD_TSN, BOARD_TSN_P2_PORT);
                } else {
                    tsw_disable_store_forward_mode(BOARD_TSN, BOARD_TSN_P2_PORT);
                }

                if (!status.tsw_phy_duplex) {
                    LOG_E("Error: PHY is in half duplex now, but TSW MAC supports only full duplex mode!\r\n");
                    return;
                }

                #if _ENET_PORT_COUNT == 2
                    netif_set_link_up(netif_get_by_index(2));
                #else
                    netif_set_link_up(netif_get_by_index(1));
                #endif

            } else {
                LOG_I("P2 Link Status: Down\r\n");
                #if _ENET_PORT_COUNT == 2
                    netif_set_link_down(netif_get_by_index(2));
                #else
                    netif_set_link_down(netif_get_by_index(1));
                #endif
            }
        }
    }
}

#if defined(NO_SYS) && NO_SYS

void enet_services(struct netif *netif)
{
    #if defined(LWIP_DHCP) && LWIP_DHCP
    dhcp_start(netif);
    #endif
}

void enet_common_handler(struct netif *netif)
{
    ethernetif_input(netif);

    /* Handle all system timeouts for all core protocols */
    #if defined(LWIP_TIMERS) && LWIP_TIMERS
    sys_check_timeouts();
    #endif

    /* update DHCP progress */
    #if defined(LWIP_DHCP) && LWIP_DHCP
    enet_update_dhcp_state(netif);
    #endif
}
#endif

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
void isr_enet(ENET_Type *ptr)
{
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
#endif
#endif

    uint32_t status;
    uint32_t rxgbfrmis;
    uint32_t intr_status;

    status = ptr->DMA_STATUS;
    rxgbfrmis = ptr->MMC_INTR_RX;
    intr_status = ptr->INTR_STATUS;

    if (ENET_DMA_STATUS_GLPII_GET(status)) {
        /* read LPI_CSR to clear interrupt status */
        ptr->LPI_CSR;
    }

    if (ENET_INTR_STATUS_RGSMIIIS_GET(intr_status)) {
        /* read XMII_CSR to clear interrupt status */
        ptr->XMII_CSR;
    }

    if (ENET_DMA_STATUS_RI_GET(status)) {
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
        /* Give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(s_xSemaphore, &xHigherPriorityTaskWoken);
        /* Switch tasks if necessary. */
        if (xHigherPriorityTaskWoken != pdFALSE) {
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
#endif
#else
        rx_flag = true;
#endif
        ptr->DMA_STATUS |= ENET_DMA_STATUS_RI_MASK;
    }

    if (ENET_MMC_INTR_RX_RXCTRLFIS_GET(rxgbfrmis)) {
        ptr->RXFRAMECOUNT_GB;
    }
}

#ifdef HPM_ENET0_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET0, isr_enet0)
void isr_enet0(void)
{
    isr_enet(BOARD_ENET);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
void isr_enet1(void)
{
    isr_enet(BOARD_ENET);
}
#endif

#endif
