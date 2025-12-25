/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __MONITOR_CONFIG_H
#define __MONITOR_CONFIG_H


#define MONITOR_PID                  (0xFFFF)
#define MONITOR_VID                  (0x34B7) /* HPMicro VID */

#define MONITOR_PROFILE_MAXSIZE      (4096)

#define MONITOR_MEM_SIZE             (40*1024)

/*--------monitor log--------*/
#define CONFIG_MONITOR_PRINTF(...) printf(__VA_ARGS__)

#ifndef CONFIG_MONITOR_DBG_LEVEL
#define CONFIG_MONITOR_DBG_LEVEL MONITOR_DBG_INFO
#endif

/* Enable print with color */
#define CONFIG_MONITOR_PRINTF_COLOR_ENABLE
/*-----------------------------*/
/* attribute data into no cache ram */
#define MONITOR_NOCACHE_RAM_SECTION __attribute__((section(".fast_ram")))
#define MONITOR_NOCACHE_AHB_SECTION __attribute__((section(".ahb_sram")))
#define MONITOR_ATTR_ALIGN(alignment) ATTR_ALIGN(alignment)

#define CONFIG_MONITOR_RUNNING_CORE        HPM_CORE0

#include "board.h"

#if defined(CONFIG_UART_CHANNEL) && CONFIG_UART_CHANNEL

#define MONITOR_UART_DMA_ENABLE

#ifndef CONFIG_USE_CONSOLE_UART
#define MONITOR_UART_BASE            HPM_UART2
#define MONITOR_UART_CLK_NAME        clock_uart2
#define MONITOR_UART_IRQ             IRQn_UART2
#define MONITOR_UART_BAUDRATE        (2000000UL)//(115200UL)

#ifdef MONITOR_UART_DMA_ENABLE
#define MONITOR_UART_TX_DMA_REQ        HPM_DMA_SRC_UART2_TX
#define MONITOR_UART_RX_DMA_REQ        HPM_DMA_SRC_UART2_RX
#endif

#else
#define MONITOR_UART_BASE            BOARD_CONSOLE_UART_BASE
#define MONITOR_UART_CLK_NAME        BOARD_CONSOLE_UART_CLK_NAME
#define MONITOR_UART_IRQ             BOARD_CONSOLE_UART_IRQ
#define MONITOR_UART_BAUDRATE        BOARD_CONSOLE_UART_BAUDRATE

#ifdef MONITOR_UART_DMA_ENABLE
#define MONITOR_UART_TX_DMA_REQ      BOARD_CONSOLE_UART_TX_DMA_REQ
#define MONITOR_UART_RX_DMA_REQ      BOARD_CONSOLE_UART_RX_DMA_REQ
#endif
#endif

#ifdef MONITOR_UART_DMA_ENABLE
#define MONITOR_UART_DMA_CONTROLLER    HPM_HDMA
#define MONITOR_UART_DMAMUX_CONTROLLER HPM_DMAMUX
#define MONITOR_UART_TX_DMA_CHN        (0U)
#define MONITOR_UART_RX_DMA_CHN        (1U)
#define MONITOR_UART_TX_DMAMUX_CHN     DMA_SOC_CHN_TO_DMAMUX_CHN(MONITOR_UART_DMA_CONTROLLER, MONITOR_UART_TX_DMA_CHN)
#define MONITOR_UART_RX_DMAMUX_CHN     DMA_SOC_CHN_TO_DMAMUX_CHN(MONITOR_UART_DMA_CONTROLLER, MONITOR_UART_RX_DMA_CHN)
#define MONITOR_UART_DMA_IRQ           IRQn_HDMA
#endif

#endif

#if defined(CONFIG_USB_CHANNEL) && CONFIG_USB_CHANNEL

#define CONFIG_USB_POLLING_ENABLE

#include "usb_config.h"
#define MONITOR_USB_BASE             CONFIG_HPM_USBD_BASE
#define MONITOR_USB_BUSID            0
#define MONITOR_USB_IRQ              CONFIG_HPM_USBD_IRQn
#define MONITOR_USB_PRIORITY         2

#endif

#if defined(CONFIG_ENET_CHANNEL) && CONFIG_ENET_CHANNEL

#define MONITOR_TCP_LOCAL_PORT       5001

/* Static IP Address */
#define MONITOR_ENET_IP_ADDR0 192
#define MONITOR_ENET_IP_ADDR1 168
#define MONITOR_ENET_IP_ADDR2 100
#define MONITOR_ENET_IP_ADDR3 10

#define MONITOR_NETMASK_ADDR0 255
#define MONITOR_NETMASK_ADDR1 255
#define MONITOR_NETMASK_ADDR2 255
#define MONITOR_NETMASK_ADDR3 0
/* Gateway Address */
#define MONITOR_ENET_GW_ADDR0 192
#define MONITOR_ENET_GW_ADDR1 168
#define MONITOR_ENET_GW_ADDR2 100
#define MONITOR_ENET_GW_ADDR3 1

#define MONITOR_ENET_TIMER           (HPM_GPTMR2)
#define MONITOR_ENET_TIMER_CH        1
#define MONITOR_ENET_TIMER_IRQ       IRQn_GPTMR2
#define MONITOR_ENET_TIMER_CLK_NAME  (clock_gptmr2)

#endif

#define MONITOR_REPORT_MAXCOUNT              (16)
#define MONITOR_STREAM_BUFFER_MAXCOUNT       MONITOR_REPORT_MAXCOUNT
#define MONITOR_CHANNEL_MAXCOUNT             MONITOR_REPORT_MAXCOUNT
#define MONITOR_TRIGGER_MAXCOUNT             (16)
#define MONITOR_DATA_LIST_MAXCOUNT           (20)

/*--------monitor timer sample config--------*/
#define MONITOR_SAMPLE_GPTMR_1_BASE          HPM_GPTMR0
#define MONITOR_SAMPLE_GPTMR_1_IRQ           IRQn_GPTMR0
#define MONITOR_SAMPLE_GPTMR_1_CLOCK         clock_gptmr0
#if MONITOR_STREAM_BUFFER_MAXCOUNT > 12
#define MONITOR_SAMPLE_GPTMR_4_BASE          HPM_GPTMR3
#define MONITOR_SAMPLE_GPTMR_4_IRQ           IRQn_GPTMR3
#define MONITOR_SAMPLE_GPTMR_4_CLOCK         clock_gptmr3
#endif
#if MONITOR_STREAM_BUFFER_MAXCOUNT > 8
#define MONITOR_SAMPLE_GPTMR_3_BASE          HPM_GPTMR2
#define MONITOR_SAMPLE_GPTMR_3_IRQ           IRQn_GPTMR2
#define MONITOR_SAMPLE_GPTMR_3_CLOCK         clock_gptmr2
#endif
#if MONITOR_STREAM_BUFFER_MAXCOUNT > 4
#define MONITOR_SAMPLE_GPTMR_2_BASE          HPM_GPTMR1
#define MONITOR_SAMPLE_GPTMR_2_IRQ           IRQn_GPTMR1
#define MONITOR_SAMPLE_GPTMR_2_CLOCK         clock_gptmr1
#endif

#ifdef HPM_XDMA
#define MONITOR_SAMPLE_DMA_1_BASE            HPM_XDMA_BASE
#define MONITOR_SAMPLE_DMA_1                 HPM_XDMA
#else
#define MONITOR_SAMPLE_DMA_1_BASE            HPM_HDMA_BASE
#define MONITOR_SAMPLE_DMA_1                 HPM_HDMA
#endif

#if DMA_SOC_CHANNEL_NUM < MONITOR_STREAM_BUFFER_MAXCOUNT
#define MONITOR_SAMPLE_DMA_2_BASE            HPM_HDMA_BASE
#define MONITOR_SAMPLE_DMA_2                 HPM_HDMA
#endif

#define MONITOR_SAMPLE_DMAMUX_BASE           HPM_DMAMUX

#endif //__MONITOR_CONFIG_H