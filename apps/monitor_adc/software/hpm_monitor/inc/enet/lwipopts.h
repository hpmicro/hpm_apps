/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#define SYS_LIGHTWEIGHT_PROT    0
#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           0
#define IP_FRAG                 0
#define ARP_QUEUEING            0
#define LWIP_RAW                1
#define LWIP_IPV4               1
#define LWIP_TIMERS             1
#define LWIP_NETIF_TX_SINGLE_PBUF 1
#define NO_SYS                  1

#define MEM_ALIGNMENT           4
#define MEM_SIZE                (20*1024)
#define MEMP_NUM_PBUF           100
#define MEMP_NUM_UDP_PCB        6
#define MEMP_NUM_TCP_PCB        10
#define MEMP_NUM_TCP_PCB_LISTEN 5
#define MEMP_NUM_TCP_SEG        20
#define MEMP_NUM_SYS_TIMEOUT    10

#define PBUF_POOL_SIZE          20
#define PBUF_POOL_BUFSIZE       1600

#define LWIP_TCP                1
#define TCP_TTL                 255
#define TCP_QUEUE_OOSEQ         0
#define TCP_MSS                 (1500 - 40)
#define TCP_SND_BUF             (5*TCP_MSS)
#define TCP_SND_QUEUELEN        (4* TCP_SND_BUF/TCP_MSS)
#define TCP_WND                 (2*TCP_MSS)

#define LWIP_ICMP                       1
#define ICMP_TTL                        64

#ifndef LWIP_DHCP
#define LWIP_DHCP               0
#endif

#define LWIP_UDP                1
#define UDP_TTL                 255

#define LWIP_STATS 0
#define LWIP_PROVIDE_ERRNO 1
#define LWIP_NETIF_LINK_CALLBACK        1

#define CHECKSUM_BY_HARDWARE 1
#ifdef CHECKSUM_BY_HARDWARE
  #define CHECKSUM_GEN_IP                 0
  #define CHECKSUM_GEN_UDP                0
  #define CHECKSUM_GEN_TCP                0
  #define CHECKSUM_CHECK_IP               0
  #define CHECKSUM_CHECK_UDP              0
  #define CHECKSUM_CHECK_TCP              0
  #define CHECKSUM_GEN_ICMP               0
#else
  #define CHECKSUM_GEN_IP                 1
  #define CHECKSUM_GEN_UDP                1
  #define CHECKSUM_GEN_TCP                1
  #define CHECKSUM_CHECK_IP               1
  #define CHECKSUM_CHECK_UDP              1
  #define CHECKSUM_CHECK_TCP              1
  #define CHECKSUM_GEN_ICMP               1
#endif

#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0

#define LWIP_DEBUG                      1

#ifdef LWIP_DEBUG
#define LWIP_DBG_MIN_LEVEL         0
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#define ETHARP_DEBUG               LWIP_DBG_OFF
#endif

#define TCPIP_THREAD_NAME              "TCP/IP"
#define TCPIP_THREAD_STACKSIZE          1500
#define TCPIP_MBOX_SIZE                 5
#define DEFAULT_UDP_RECVMBOX_SIZE       1000
#define DEFAULT_TCP_RECVMBOX_SIZE       1000
#define DEFAULT_ACCEPTMBOX_SIZE         1000
#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               3
#define LWIP_SINGLE_NETIF               1
#define LWIP_COMPAT_MUTEX               0

#endif /* __LWIPOPTS_H__ */
