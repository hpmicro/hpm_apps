/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @FilePath: udp_echo.c
 * @Version: 2.0
 * @Author: zhaoshuai (shuai.zhao@hpmicro.com)
 * @Date: 2025-11-10 14:20:00
 * @LastEditTime: 2025-11-11 09:52:21
 */
/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "udp_echo.h"
#include "lwip/udp.h"

static struct udp_pcb *upcb = NULL;
static ip_addr_t client_ip[16];       // 客户端IP
static uint16_t client_port[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};     // 客户端端口（0表示未连接）
static uint8_t client_count = 0; // 已连接客户端数量

static void udpecho_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    (void)arg;

    if (p != NULL) {
        // printf("sever recv %s:%d %s\n", ip_ntoa(addr), port, (char*)p->payload);
        if(client_count < 16) {
            // 查找是否已存在相同的客户端
            uint8_t found = 0;
            for(uint8_t i = 0; i < client_count; i++) {
                if(ip_addr_cmp(addr, &client_ip[i]) && port == client_port[i]) {
                    found = 1;
                    break;
                }
            }
            if(!found) {
                // 新客户端，添加到列表
                ip_addr_copy(client_ip[client_count], *addr);
                client_port[client_count] = port;
                client_count++;
            }
        }
        pbuf_free(p);
    }
}

void udp_echo_init(void)
{
    struct udp_pcb *pcb = NULL;

    pcb = udp_new();
    udp_bind(pcb, IP_ADDR_ANY, UDP_LOCAL_PORT);
    udp_recv(pcb, udpecho_recv, NULL);
    upcb = pcb;
}

void udp_echo_send_test(void)
{
    struct pbuf *p;
    err_t err = ERR_OK;

    if (client_count == 0 || upcb == NULL) {
        return;
    }

    p = pbuf_alloc(PBUF_TRANSPORT, 12, PBUF_RAM);
    if (p == NULL) {
        printf("pbuf_alloc failed\n");
        return ERR_MEM;
    }

    memcpy(p->payload, "Hello World", p->len);
    static uint8_t i = 0; 
    err = udp_sendto(upcb, p, &client_ip[i], client_port[i]);
    if (err == ERR_OK) {
    } else {
        printf("err%d\n", err);
    }
    i++;
    if(i >= client_count) {
        i = 0;
    }

    pbuf_free(p);
}

