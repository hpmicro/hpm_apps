/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "netconf.h"

#if defined(LWIP_DHCP) && LWIP_DHCP
#include "lwip/dhcp.h"
#endif

#include "lwip/netifapi.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "common.h"

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

/* Public variables ---------------------------------------------------------*/
ip_init_t ip_init[_ENET_PORT_COUNT] = {
    {HPM_STRINGIFY(IP0_CONFIG), HPM_STRINGIFY(NETMASK0_CONFIG), HPM_STRINGIFY(GW0_CONFIG)},
    #if _ENET_PORT_COUNT == 2
    {HPM_STRINGIFY(IP1_CONFIG), HPM_STRINGIFY(NETMASK1_CONFIG), HPM_STRINGIFY(GW1_CONFIG)}
    #endif
};

mac_init_t mac_init[_ENET_PORT_COUNT] = {
    {HPM_STRINGIFY(MAC0_CONFIG)},
    #if _ENET_PORT_COUNT == 2
    {HPM_STRINGIFY(MAC1_CONFIG)}
    #endif
};

#if defined(LWIP_DHCP) && LWIP_DHCP
/**
* @brief  LwIP_DHCP_Process_Handle
* @param  parameter the parameter of thread enter function
* @retval None
*/
void LwIP_DHCP_task(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;

    err_t err = dhcp_start(netif);
    if(err == ERR_OK) {
        LOG_I("DHCP Acquiring IP address ...\r\n");
    } else {
        LOG_I("Failed to start DHCP\r\n");
    }

    for (;;) {
        enet_update_dhcp_state(netif);
        vTaskDelay(500);
    }
}
#endif

static void netif_update_status(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
#if defined(LWIP_DHCP) && !LWIP_DHCP
        netif_user_notification(netif);
#endif
    }
}

void netif_config(struct netif *netif, uint8_t idx)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#if defined(LWIP_DHCP) && LWIP_DHCP
    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);
#else
    ip4addr_aton(ip_init[idx].ip_addr, &ipaddr);
    ip4addr_aton(ip_init[idx].netmask, &netmask);
    ip4addr_aton(ip_init[idx].gw, &gw);
#endif

#if defined(NO_SYS) && NO_SYS
    netif_add(netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
    netif_set_up(netif);
    netif_set_default(netif);
#else
    netifapi_netif_add(netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netifapi_netif_set_up(netif);
    netifapi_netif_set_default(netif);
#endif
    netif_set_link_callback(netif, netif_update_status);
}


void netif_user_notification(struct netif *netif)
{
    if (netif_is_up(netif)) {
        LOG_I("IPv4 Address: %s\r\n", ipaddr_ntoa(&netif->ip_addr));
        LOG_I("IPv4 Netmask: %s\r\n", ipaddr_ntoa(&netif->netmask));
        LOG_I("IPv4 Gateway: %s\r\n", ipaddr_ntoa(&netif->gw));
    }
}
