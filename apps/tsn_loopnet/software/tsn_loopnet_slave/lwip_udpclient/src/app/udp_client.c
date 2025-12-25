/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "udp_client.h"
#include "lwip/udp.h"
#include "netconf.h"

struct udp_pcb *upcb = NULL;

static void udp_client_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    (void)arg;

    static int rc = 0;
    rc++;
    if(rc>=1000) {
        rc = 0;
        board_led_toggle();
    }

    if (p != NULL) {
        // printf("客户端收到来自 %s:%d 的回复：%s\n", ip_ntoa(addr), port, (char*)p->payload);
        app_lwip_timer_stop();
        pbuf_free(p);
    }
}

void udp_client_init(void)
{
    struct udp_pcb *pcb = NULL;

    pcb = udp_new();
    udp_bind(pcb, IP_ADDR_ANY, UDP_LOCAL_PORT);
    udp_recv(pcb, udp_client_recv, NULL);
    upcb=pcb;
}
// 发送数据到服务器
void udp_client_send(const char *data, u16_t len) {
    struct udp_pcb *udp_pcb;
    ip_addr_t server_ip;
    struct pbuf *p;
    err_t err;

    ip4addr_aton(HPM_STRINGIFY(REMOTE_IP_CONFIG), &server_ip);

    p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL) {
        printf("pbuf_alloc failed\n");
        udp_remove(upcb);
        return;
    }
    memcpy(p->payload, data, len);


    err = udp_sendto(upcb, p, &server_ip, 5000);
    if (err != ERR_OK) {
        printf("send err: %d\n", err);
    }

    pbuf_free(p);
}
