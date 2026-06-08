/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "app_udp.h"

static void app_udp_echo_thread_0(void *arg)
{
    struct netconn *conn;
    struct netbuf *inbuf;
    struct netbuf *outbuf;
    void *data;
    err_t err;

    struct netif *netif = (struct netif *)arg;

#if LWIP_IPV6
    conn = netconn_new(NETCONN_UDP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, APP_UDP_LOCAL_PORT0);
#else  /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP_ADDR_ANY, APP_UDP_LOCAL_PORT0);
#endif /* LWIP_IPV6 */
    LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

    netconn_bind_if(conn, netif_get_index(netif));

    while (1) {
        err = netconn_recv(conn, &inbuf);
        if (err == ERR_OK) {
            /* create a new netbuf */
            outbuf = netbuf_new();
            if (outbuf != NULL) {
                /* allocate memory for the new netbuf */
                data = netbuf_alloc(outbuf, netbuf_len(inbuf));
                if (data != NULL) {
                    /* copy payload from inbuf to outbuf */
                    if (netbuf_copy(inbuf, data, netbuf_len(inbuf)) == netbuf_len(inbuf)) {
                        /* send using the new netbuf with source address and port */
                        err = netconn_sendto(conn, outbuf, netbuf_fromaddr(inbuf), netbuf_fromport(inbuf));
                        if (err != ERR_OK) {
                            LOG_E("netconn_sendto failed: %d\n", err);
                        }
                    } else {
                        LOG_E("netbuf_copy failed\n");
                    }
                }
                netbuf_delete(outbuf);
            }
            netbuf_delete(inbuf);
        }
    }
}

static void app_udp_echo_thread_1(void *arg)
{
    struct netconn *conn;
    struct netbuf *inbuf;
    struct netbuf *outbuf;
    void *data;
    err_t err;

    struct netif *netif = (struct netif *)arg;

#if LWIP_IPV6
    conn = netconn_new(NETCONN_UDP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, APP_UDP_LOCAL_PORT1);
#else  /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP_ADDR_ANY, APP_UDP_LOCAL_PORT1);
#endif /* LWIP_IPV6 */
    LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

    netconn_bind_if(conn, netif_get_index(netif));

    while (1) {
        err = netconn_recv(conn, &inbuf);
        if (err == ERR_OK) {
            /* create a new netbuf */
            outbuf = netbuf_new();
            if (outbuf != NULL) {
                /* allocate memory for the new netbuf */
                data = netbuf_alloc(outbuf, netbuf_len(inbuf));
                if (data != NULL) {
                    /* copy payload from inbuf to outbuf */
                    if (netbuf_copy(inbuf, data, netbuf_len(inbuf)) == netbuf_len(inbuf)) {
                        /* send using the new netbuf with source address and port */
                        err = netconn_sendto(conn, outbuf, netbuf_fromaddr(inbuf), netbuf_fromport(inbuf));
                        if (err != ERR_OK) {
                            LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_sendto failed: %d\n", (int)err));
                        }
                    } else {
                        LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
                    }
                }
                netbuf_delete(outbuf);
            }
            netbuf_delete(inbuf);
        }
    }
}

void app_udp_echo_init(struct netif *netif)
{
    char thread_name[20] = {0};
    lwip_thread_fn thread[] = {app_udp_echo_thread_0, app_udp_echo_thread_1};

    sprintf(thread_name, "udp_echo_thread_%d", netif->num);
    
    sys_thread_new(thread_name, thread[netif->num], (void *)netif, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
