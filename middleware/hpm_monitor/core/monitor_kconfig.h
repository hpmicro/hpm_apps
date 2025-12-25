/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MONITOR_KCONFIG_H
#define __MONITOR_KCONFIG_H

#if (__has_include("monitor_config.h"))

#include "monitor_config.h"

#else

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


#endif

#define MONITOR_VERSION       0x010000
#define MONITOR_VERSION_STR   "v1.0.0"

#define MONITOR_RINGBUFFSER_SIZE     (MONITOR_PROFILE_MAXSIZE*2)

#if (MONITOR_STREAM_BUFFER_MAXCOUNT > 16) || (MONITOR_STREAM_BUFFER_MAXCOUNT > MONITOR_REPORT_MAXCOUNT)
#error "MONITOR_STREAM_BUFFER_MAXCOUNT max support 16"
#endif

#if MONITOR_CHANNEL_MAXCOUNT > MONITOR_REPORT_MAXCOUNT
#error "MONITOR_CHANNEL_MAXCOUNT max support MONITOR_REPORT_MAXCOUNT"
#endif

#include <stdint.h>
#include <stdbool.h>

#define ALIGN_4(x) (((x) + 3) & ~3)

#define MONITOR_PRIVATE_VAR_NAME(name) b5e21f8a3d709c4f6a82e1b90c367d2a_##name
#define MONITOR_SPECIAL_VAR_NAME(name) d73a1f4e8c02b59e61f807ac45d316ba_##name

typedef struct {
    const char* name_t;
    uint8_t channel_t;
    uint8_t type_index_t;
    uint16_t count_t;
    uint32_t freq_t;
    uint64_t encoded_value;
} monitor_var_info_t;

extern monitor_var_info_t MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_CHANNEL_MAXCOUNT];
extern uint32_t MONITOR_PRIVATE_VAR_NAME(monitor_registry_count);

const monitor_var_info_t* find_monitor_by_name(const char* name);
const monitor_var_info_t* find_monitor_by_channel(uint8_t channel);

// the DATA_TYPE enum
#define TYPE_INDEX(type) (_Generic((type){0}, \
                        bool:        0x00, \
                        uint8_t:     0x01, \
                        int8_t:      0x02, \
                        uint16_t:    0x03, \
                        int16_t:     0x04, \
                        uint32_t:    0x05, \
                        int32_t:     0x06, \
                        uint64_t:    0x07, \
                        int64_t:     0x08, \
                        float:       0x09, \
                        double:      0x0A, \
                        default:     -1 \
))

#define MONITOR_DEFINE_GLOBAL_VAR(name, channel, type, frequency, count) \
    __attribute__((section(".rodata"), used)) \
    const uint64_t c1f8a3d2b7e1de45a8f1c4b32e3f0b7d_##name = \
    (((uint64_t)(frequency) << 32) | \
    ((uint64_t)(count & 0xFFFF) << 16) | \
    ((uint64_t)TYPE_INDEX(type) & 0xFF) << 8 | \
    ((uint64_t)(channel) & 0xFF)); \
    \
    __attribute__((constructor, used)) \
    static void __register_monitor_##name(void) { \
        extern monitor_var_info_t MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_CHANNEL_MAXCOUNT]; \
        extern uint32_t MONITOR_PRIVATE_VAR_NAME(monitor_registry_count); \
        /* Inform the compiler about variables c1f8a3d2b7e1de45a8f1c4b32e3f0b7d_##name is being used, and memory may change */ \
        __asm__ volatile ("" : : "r"(&c1f8a3d2b7e1de45a8f1c4b32e3f0b7d_##name) : "memory"); \
        \
        if (MONITOR_PRIVATE_VAR_NAME(monitor_registry_count) < MONITOR_CHANNEL_MAXCOUNT) { \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)].name_t = #name; \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)].channel_t = (channel); \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)].type_index_t = TYPE_INDEX(type); \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)].count_t = (count); \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)].freq_t = (frequency); \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)].encoded_value = c1f8a3d2b7e1de45a8f1c4b32e3f0b7d_##name; \
            MONITOR_PRIVATE_VAR_NAME(monitor_registry_count)++; \
        } else { \
            printf("bad! monitor define global var overflow!"); \
            while(1); \
        } \
    }


extern bool monitor_send_is_idle(void);

#endif //__MONITOR_KCONFIG_H