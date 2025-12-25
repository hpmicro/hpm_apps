/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "hpm_gpio_drv.h"
#include "dhserver.h"
#include "dnserver.h"
#include "netif/etharp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#include "httpd.h"
#include "usbd_core.h"
#include "usbd_rndis.h"
#include "cdc_rndis_device.h"
#include "button.h"

/* Network configuration */
#define NUM_DHCP_ENTRY 3
#define PADDR(ptr)     ((ip_addr_t *)ptr)

static uint8_t hwaddr[6] = { 0x20, 0x89, 0x84, 0x6A, 0x96, 0x00 };
static uint8_t ipaddr[4] = { 192, 168, 7, 1 };
static uint8_t netmask[4] = { 255, 255, 255, 0 };
static uint8_t gateway[4] = { 0, 0, 0, 0 };

static dhcp_entry_t entries[NUM_DHCP_ENTRY] = { { { 0 }, { 192, 168, 7, 2 }, { 255, 255, 255, 0 }, 24 * 60 * 60 },
                                                { { 0 }, { 192, 168, 7, 3 }, { 255, 255, 255, 0 }, 24 * 60 * 60 },
                                                { { 0 }, { 192, 168, 7, 4 }, { 255, 255, 255, 0 }, 24 * 60 * 60 } };

static dhcp_config_t dhcp_config = {
    { 192, 168, 7, 1 }, /* server address */
    67,                 /* port */
    { 192, 168, 7, 1 }, /* dns server */
    "hpm",              /* dns suffix */
    NUM_DHCP_ENTRY,     /* num entry */
    entries             /* entries */
};

/* Network interface (exported for use by tasks.c) */
struct netif netif_data;

/* Forward declarations for internal callbacks */
static err_t netif_init_cb(struct netif *netif);
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p);
static bool dns_query_proc(const char *name, ip_addr_t *addr);

void user_init_lwip(void)
{
    struct netif *netif = &netif_data;

    /* Initialize LwIP TCP/IP stack */
    tcpip_init(NULL, NULL);
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, hwaddr, 6);

    netif = netif_add(netif, PADDR(ipaddr), PADDR(netmask), PADDR(gateway), NULL, netif_init_cb, netif_input);
    netif_set_default(netif);

    /* Wait for network interface to be up */
    while (!netif_is_up(&netif_data)) {
        ;
    }

    /* Initialize DHCP server */
    while (dhserv_init(&dhcp_config) != ERR_OK) {
        ;
    }

    /* Initialize DNS server */
    while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK) {
        ;
    }

    /* Initialize HTTP server */
    httpd_init();
}

/* Internal callback: netif initialization */
static err_t netif_init_cb(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output = etharp_output;
    return ERR_OK;
}

/* Internal callback: link output via USB RNDIS */
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    extern usb_osal_sem_t sema_rndis_send_done;
    (void)netif;
    int ret;

    ret = usbd_rndis_eth_tx(p);

    if (0 != ret) {
        ret = ERR_BUF;
    } else {
        if (usb_osal_sem_take(sema_rndis_send_done, portMAX_DELAY) != 0) {
            ret = ERR_BUF;
        }
    }
    return ret;
}

/* Internal callback: DNS query handler */
static bool dns_query_proc(const char *name, ip_addr_t *addr)
{
    if (strcmp(name, "rndis.hpm") == 0 || strcmp(name, "www.rndis.hpm") == 0) {
        addr->addr = *(uint32_t *)ipaddr;
        return true;
    }
    return false;
}

/**
 * @brief Main application entry point
 * 
 * Initializes hardware, network stack, and creates all system tasks
 * before starting the FreeRTOS scheduler.
 */
int main(void)
{
    /* Initialize board peripherals */
    board_init();

    /* Initialize EUI (segment LED and keys) */
    init_eui_pins(BOARD_EUI);
    init_eui_config();
    seg_led_disp_config();

    /* Initialize USB */
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);
    cdc_rndis_init(0, CONFIG_HPM_USBD_BASE);

    /* Initialize network stack (LwIP, DHCP, DNS, HTTP) */
    user_init_lwip();

    /* Create all application tasks (USB, Mongoose, GPIO) */
    create_system_tasks();

    /* Start FreeRTOS scheduler - never returns */
    vTaskStartScheduler();

    /* Should never reach here */
    for (;;) {
        ;
    }

    return 0;
}