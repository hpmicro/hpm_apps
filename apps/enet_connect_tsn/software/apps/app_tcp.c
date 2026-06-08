/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include "app_tcp.h"

static void app_tcp_echo_thread_0(void *arg)
{
    struct netconn *conn, *newconn;
    struct netbuf *buf;
    void *data;
    u16_t len;
    err_t err;

    struct netif *netif = (struct netif *)arg;

#if LWIP_IPV6
    conn = netconn_new(NETCONN_TCP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, APP_TCP_LOCAL_PORT0);
#else /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, APP_TCP_LOCAL_PORT0);
#endif /* LWIP_IPV6 */
    LWIP_ASSERT("tcpecho: invalid conn", (conn != NULL));

    netconn_listen(conn);
    netconn_bind_if(conn, netif_get_index(netif));

    while (1) {

        if (netconn_accept(conn, &newconn) == ERR_OK) {
            while (netconn_recv(newconn, &buf) == ERR_OK) {
                do {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_COPY);
                    if (err != ERR_OK) {
                        LOG_E("tcp_echo: netconn_write: error %d\n", err);
                    }
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

static void app_tcp_echo_thread_1(void *arg)
{
    struct netconn *conn, *newconn;
    struct netbuf *buf;
    void *data;
    u16_t len;
    err_t err;

    struct netif *netif = (struct netif *)arg;

#if LWIP_IPV6
    conn = netconn_new(NETCONN_TCP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, APP_TCP_LOCAL_PORT1);
#else /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, APP_TCP_LOCAL_PORT1);
#endif /* LWIP_IPV6 */
    LWIP_ASSERT("tcpecho: invalid conn", (conn != NULL));

    netconn_listen(conn);
    netconn_bind_if(conn, netif_get_index(netif));

    while (1) {
        if (netconn_accept(conn, &newconn) == ERR_OK) {
            while (netconn_recv(newconn, &buf) == ERR_OK) {
                do {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_COPY);
                    if (err != ERR_OK) {
                       LOG_E("tcp_echo: netconn_write: error %d\n", err);
                    }
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

void app_tcp_echo_init(struct netif *netif)
{
    char thread_name[20] = {0};
    lwip_thread_fn thread[] = {app_tcp_echo_thread_0, app_tcp_echo_thread_1};

    sprintf(thread_name, "tcp_echo_thread_%d", netif->num);

    sys_thread_new(thread_name, thread[netif->num], (void *)netif, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

}