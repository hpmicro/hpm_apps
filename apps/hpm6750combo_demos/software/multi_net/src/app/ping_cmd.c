/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/mem.h>
#include <lwip/icmp.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/inet.h>
#include <lwip/inet_chksum.h>
#include <lwip/ip.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping receive timeout - in milliseconds */
#define PING_RCV_TIMEO (2000 * portTICK_PERIOD_MS)
/** ping delay - in milliseconds */
#define PING_DELAY     (1000 * portTICK_PERIOD_MS)

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/* ping variables */
static u16_t ping_seq_num;

struct _ip_addr {
    uint8_t addr0, addr1, addr2, addr3;
};

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *) iecho)[sizeof(struct icmp_echo_hdr) + i] = (char) i;
    }

#if defined(CHECKSUM_GEN_ICMP) && CHECKSUM_GEN_ICMP
    iecho->chksum = inet_chksum(iecho, len);
#else
    iecho->chksum = 0;
#endif
}

/* Ping using the socket ip */
err_t lwip_ping_send(int s, ip_addr_t *addr, int size)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    int ping_size = sizeof(struct icmp_echo_hdr) + size;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = pvPortMalloc(ping_size);
    if (iecho == NULL) {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t) ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
#if LWIP_IPV4 && LWIP_IPV6
    to.sin_addr.s_addr = addr->u_addr.ip4.addr;
#elif LWIP_IPV4
    to.sin_addr.s_addr = addr->addr;
#elif LWIP_IPV6
#error Not supported IPv6.
#endif

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *) &to, sizeof(to));
    vPortFree(iecho);

    return (err == ping_size ? ERR_OK : ERR_VAL);
}

int lwip_ping_recv(int s, int *ttl)
{
    char buf[64];
    int fromlen = sizeof(struct sockaddr_in), len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *) &from, (socklen_t *) &fromlen)) > 0) {
        if (len >= (int) (sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
            iphdr = (struct ip_hdr *) buf;
            iecho = (struct icmp_echo_hdr *) (buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                *ttl = iphdr->_ttl;
                return len;
            }
        }
    }

    return len;
}


/* using the lwIP custom ping */
int ping(char *target_name, uint32_t count, uint16_t size, const char *netif_name)
{
#if LWIP_VERSION_MAJOR >= 2U
    struct timeval timeout = { PING_RCV_TIMEO / (1000 * portTICK_PERIOD_MS), PING_RCV_TIMEO % (1000 * portTICK_PERIOD_MS) };
#else
    int timeout = PING_RCV_TIMEO * 1000UL / (1000 * portTICK_PERIOD_MS);
#endif

    int s, ttl, recv_len;
    ip_addr_t target_addr;
    uint32_t send_times;
    uint32_t recv_times;
    uint32_t recv_start_tick;
#if LWIP_DNS && LWIP_SOCKET
    struct addrinfo hint, *res = NULL;
#endif
    struct in_addr ina;

    send_times = 0;
    recv_times = 0;
    ping_seq_num = 0;

    if (size == 0) {
        size = PING_DATA_SIZE;
    }

#if LWIP_DNS && LWIP_SOCKET
    memset(&hint, 0, sizeof(hint));
    /* convert URL to IP */
    if (lwip_getaddrinfo(target_name, NULL, &hint, &res) != 0) {
        printf("ping: unknown host %s\n\r", target_name);
        return -1;
    }
    ina = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    lwip_freeaddrinfo(res);
#else
    /* DNS APIs are unavailable when LWIP_DNS=0, only dotted-decimal IPv4 is supported. */
    if (inet_aton(target_name, &ina) == 0) {
        printf("ping: unknown host %s (DNS disabled)\n\r", target_name);
        return -1;
    }
#endif
    if (inet_aton(inet_ntoa(ina), &target_addr) == 0) {
        printf("ping: unknown host %s\n\r", target_name);
        return -1;
    }
    /* new a socket */
    if ((s = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
        printf("ping: create socket failed\n\r");
        return -1;
    }

    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* bind to specific netif if specified */
    if (netif_name != NULL) {
        struct ifreq ifreq;
        memset(&ifreq, 0, sizeof(ifreq));
        strncpy(ifreq.ifr_name, netif_name, sizeof(ifreq.ifr_name) - 1);
        if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, &ifreq, sizeof(ifreq)) < 0) {
            printf("ping: bind to netif %s failed\n\r", netif_name);
            closesocket(s);
            return -1;
        }
    }

    while (1) {
        int elapsed_time, ret;

        if ((ret = lwip_ping_send(s, &target_addr, size)) == ERR_OK) {
            recv_start_tick = sys_now();
            if ((recv_len = lwip_ping_recv(s, &ttl)) >= 0) {
                recv_times++;
                elapsed_time = (sys_now() - recv_start_tick) * 1000UL / (1000 * portTICK_PERIOD_MS);
                printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n\r", recv_len, inet_ntoa(ina), send_times,
                       ttl, elapsed_time);
            } else {
                printf("From %s icmp_seq=%d timeout\n\r", inet_ntoa(ina), send_times);
            }
        } else {
            printf("Send %s - error %d\n\r", inet_ntoa(ina), ret);
        }

        send_times++;
        if (send_times >= count) {
            /* send ping times reached, stop */
            break;
        }

        vTaskDelay(PING_DELAY); /* take a delay */
    }

    closesocket(s);

    return 0;
}

int ping_cmd(int argc, char **argv)
{
    const char *netif_name = NULL;
    char *host = NULL;
    int i = 1;

    if (argc == 1) {
        printf("Usage: ping [-S <netif>] <host address>\n");
        return 0;
    }

    while (i < argc) {
        if (strcmp(argv[i], "-S") == 0) {
            if (i + 1 >= argc) {
                printf("ping: -S requires a netif name\n");
                return -1;
            }
            netif_name = argv[++i];
        } else {
            host = argv[i];
        }
        i++;
    }

    if (host == NULL) {
        printf("Usage: ping [-S <netif>] <host address>\n");
        return 0;
    }
    struct netif *netif;

    printf("list netif:\n");

    NETIF_FOREACH(netif) {
        printf("netif name: %c%c%d\n", netif->name[0], netif->name[1], netif->num);
    }

    ping(host, 4, 0, netif_name);
    return 0;
}

CSH_CMD_EXPORT_ALIAS(ping_cmd, ping, "ping <ip addr>")
