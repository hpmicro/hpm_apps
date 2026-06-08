/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_BOARD_H
#define _HPM_BOARD_H
#include <stdio.h>
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "pinmux.h"
#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#include "hpm_debug_console.h"
#endif

#define BOARD_NAME                      "hpm6e00_enet_connect_tsn"
#define BOARD_UF2_SIGNATURE             (0x0A4D5048UL)
#define BOARD_CPU_FREQ                  (600000000UL)

#define SEC_CORE_IMG_START              CORE1_ILM_LOCAL_BASE

#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE              HPM_CORE0
#endif


#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#ifndef BOARD_CONSOLE_TYPE
#define BOARD_CONSOLE_TYPE              CONSOLE_TYPE_UART
#endif

#if BOARD_CONSOLE_TYPE == CONSOLE_TYPE_UART
#ifndef BOARD_CONSOLE_UART_BASE
#if BOARD_RUNNING_CORE == HPM_CORE0
#define BOARD_CONSOLE_UART_BASE         HPM_UART0
#define BOARD_CONSOLE_UART_CLK_NAME     clock_uart0
#define BOARD_CONSOLE_UART_IRQ          IRQn_UART0
#define BOARD_CONSOLE_UART_TX_DMA_REQ   HPM_DMA_SRC_UART0_TX
#define BOARD_CONSOLE_UART_RX_DMA_REQ   HPM_DMA_SRC_UART0_RX
#else   
#define BOARD_CONSOLE_UART_BASE         HPM_UART1
#define BOARD_CONSOLE_UART_CLK_NAME     clock_uart1
#define BOARD_CONSOLE_UART_IRQ          IRQn_UART1
#define BOARD_CONSOLE_UART_TX_DMA_REQ   HPM_DMA_SRC_UART1_TX
#define BOARD_CONSOLE_UART_RX_DMA_REQ   HPM_DMA_SRC_UART1_RX
#endif
#endif
#define BOARD_CONSOLE_UART_BAUDRATE     (115200UL)
#endif
#endif

/* rtthread-nano finsh section */
#define BOARD_RT_CONSOLE_BASE           BOARD_CONSOLE_UART_BASE

/* nor flash section */
#define BOARD_FLASH_BASE_ADDRESS        (0x80000000UL)
#define BOARD_FLASH_SIZE                (16 * SIZE_1MB)

/* Flash section */
#define BOARD_APP_XPI_NOR_XPI_BASE      (HPM_XPI0)
#define BOARD_APP_XPI_NOR_CFG_OPT_HDR   (0xfcf90001U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT0  (0x00000005U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT1  (0x00001000U)

/*
 * timer for board delay
 */
#define BOARD_CALLBACK_TIMER            (HPM_GPTMR3)
#define BOARD_CALLBACK_TIMER_CH         1
#define BOARD_CALLBACK_TIMER_IRQ        IRQn_GPTMR3
#define BOARD_CALLBACK_TIMER_CLK_NAME   (clock_gptmr3)

/* gptmr section */
#define BOARD_GPTMR                   HPM_GPTMR4
#define BOARD_GPTMR_IRQ               IRQn_GPTMR4
#define BOARD_GPTMR_CHANNEL           0
#define BOARD_GPTMR_DMA_SRC           HPM_DMA_SRC_GPTMR4_0
#define BOARD_GPTMR_CLK_NAME          clock_gptmr4
#define BOARD_GPTMR_PWM               HPM_GPTMR4
#define BOARD_GPTMR_PWM_CHANNEL       0
#define BOARD_GPTMR_PWM_DMA_SRC       HPM_DMA_SRC_GPTMR4_0
#define BOARD_GPTMR_PWM_CLK_NAME      clock_gptmr4
#define BOARD_GPTMR_PWM_IRQ           IRQn_GPTMR4
#define BOARD_GPTMR_PWM_SYNC          HPM_GPTMR4
#define BOARD_GPTMR_PWM_SYNC_CHANNEL  3
#define BOARD_GPTMR_PWM_SYNC_CLK_NAME clock_gptmr4

/* LED */
#define BOARD_LED0_GPIO_CTRL            HPM_GPIO0
#define BOARD_LED0_GPIO_INDEX           GPIO_DO_GPIOB
#define BOARD_LED0_GPIO_PIN             25
#define BOARD_LED0_CTRL_INDEX           25
#define BOARD_LED1_GPIO_CTRL            HPM_GPIO0
#define BOARD_LED1_GPIO_INDEX           GPIO_DO_GPIOB
#define BOARD_LED1_GPIO_PIN             24
#define BOARD_LED1_CTRL_INDEX           24
#define BOARD_LED_CTRL_FLAG_OFF         0
#define BOARD_LED_CTRL_FLAG_ON          1
#define BOARD_LED_CTRL_FLAG_TOG         2

/* KEY */
#define BOARD_KEYA_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYA_GPIO_INDEX           GPIO_DI_GPIOC
#define BOARD_KEYA_GPIO_PIN             30

#define BOARD_KEYB_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYB_GPIO_INDEX           GPIO_DI_GPIOC
#define BOARD_KEYB_GPIO_PIN             28

#define BOARD_KEYC_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYC_GPIO_INDEX           GPIO_DI_GPIOC
#define BOARD_KEYC_GPIO_PIN             26

#define BOARD_KEYD_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYD_GPIO_INDEX           GPIO_DI_GPIOC
#define BOARD_KEYD_GPIO_PIN             24

/* OLED */
#define BOARD_OLED_BS0_CTRL             HPM_GPIO0
#define BOARD_OLED_BS0_INDEX            GPIO_DO_GPIOA
#define BOARD_OLED_BS0_PIN              16

#define BOARD_OLED_BS1_CTRL             HPM_GPIO0
#define BOARD_OLED_BS1_INDEX            GPIO_DO_GPIOA
#define BOARD_OLED_BS1_PIN              17

#define BOARD_OLED_RESET_CTRL           HPM_GPIO0
#define BOARD_OLED_RESET_INDEX          GPIO_DO_GPIOA
#define BOARD_OLED_RESET_PIN            18

#define BOARD_OLED_D_C_CTRL             HPM_GPIO0
#define BOARD_OLED_D_C_INDEX            GPIO_DO_GPIOA
#define BOARD_OLED_D_C_PIN              19

#define BOARD_OLED_SPI                  HPM_SPI2
#define BOARD_OLED_SPI_CLK_NAME         clock_spi2
#define BOARD_OLED_SPI_CLK_FREQ         (20000000UL)
#define BOARD_OLED_SPI_DMA              HPM_DMA_SRC_SPI2_TX
#define BOARD_OLED_SPI_DMA_CH           0
#define BOARD_OLED_HDMA                 HPM_HDMA
#define BOARD_OLED_HDMA_IRQ             IRQn_HDMA
#define BOARD_OLED_DMAMUX               HPM_DMAMUX
#define BOARD_OLED_DMAMUX_CH            DMA_SOC_CHN_TO_DMAMUX_CHN(BOARD_OLED_HDMA, BOARD_OLED_SPI_DMA_CH)

/* TSN */
#define BOARD_TSN                       HPM_TSW

#define BOARD_TSN_P1_RESET_CTRL         HPM_GPIO0
#define BOARD_TSN_P1_RESET_INDEX        GPIO_DO_GPIOC
#define BOARD_TSN_P1_RESET_PIN          4
#define BOARD_TSN_P1_INT_CTRL           HPM_GPIO0
#define BOARD_TSN_P1_INT_INDEX          GPIO_DI_GPIOC
#define BOARD_TSN_P1_INT_PIN            3
#define BOARD_TSN_P1_PORT               TSW_TSNPORT_PORT1
#define BOARD_TSN_P1_PORT_ITF           tsw_port_phy_itf_rgmii

#define BOARD_TSN_P2_RESET_CTRL         HPM_GPIO0
#define BOARD_TSN_P2_RESET_INDEX        GPIO_DO_GPIOC
#define BOARD_TSN_P2_RESET_PIN          0
#define BOARD_TSN_P2_INT_CTRL           HPM_GPIO0
#define BOARD_TSN_P2_INT_INDEX          GPIO_DI_GPIOC
#define BOARD_TSN_P2_INT_PIN            1
#define BOARD_TSN_P2_PORT               TSW_TSNPORT_PORT2
#define BOARD_TSN_P2_PORT_ITF           tsw_port_phy_itf_rgmii

#define BOARD_TSN_P3_PORT               TSW_TSNPORT_PORT3
#define BOARD_TSN_P3_PORT_ITF           tsw_port_phy_itf_rgmii

/* ENET */
#define BOARD_ENET               HPM_ENET0
#define BOARD_ENET_RGMII_TX_DLY        (0U)
#define BOARD_ENET_RGMII_RX_DLY        (24U)

/* USB section */
#define BOARD_USB HPM_USB0
#define BOARD_USB_IRQn IRQn_USB0
#define BOARD_USB_BASE HPM_USB0_BASE

#ifndef BOARD_SHOW_CLOCK
#define BOARD_SHOW_CLOCK 1
#endif
#ifndef BOARD_SHOW_BANNER
#define BOARD_SHOW_BANNER 1
#endif

/* FreeRTOS Definitions */
#define BOARD_FREERTOS_TIMER          HPM_GPTMR6
#define BOARD_FREERTOS_TIMER_CHANNEL  1
#define BOARD_FREERTOS_TIMER_IRQ      IRQn_GPTMR6
#define BOARD_FREERTOS_TIMER_CLK_NAME clock_gptmr6

#define BOARD_FREERTOS_LOWPOWER_TIMER          HPM_PTMR
#define BOARD_FREERTOS_LOWPOWER_TIMER_CHANNEL  1
#define BOARD_FREERTOS_LOWPOWER_TIMER_IRQ      IRQn_PTMR
#define BOARD_FREERTOS_LOWPOWER_TIMER_CLK_NAME clock_ptmr

/* Threadx Definitions */
#define BOARD_THREADX_TIMER           HPM_GPTMR6
#define BOARD_THREADX_TIMER_CHANNEL   1
#define BOARD_THREADX_TIMER_IRQ       IRQn_GPTMR6
#define BOARD_THREADX_TIMER_CLK_NAME  clock_gptmr6

#define BOARD_THREADX_LOWPOWER_TIMER           HPM_PTMR
#define BOARD_THREADX_LOWPOWER_TIMER_CHANNEL   1
#define BOARD_THREADX_LOWPOWER_TIMER_IRQ       IRQn_PTMR
#define BOARD_THREADX_LOWPOWER_TIMER_CLK_NAME  clock_ptmr


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);

void board_init_console(void);
void board_print_clock_freq(void);
void board_init_uart(UART_Type *ptr);
void board_print_banner(void);
void board_ungate_mchtmr_at_lp_mode(void);
void board_init(void);
void board_init_core1(void);
void board_delay_us(uint32_t us);
void board_delay_ms(uint32_t ms);
void board_timer_create(uint32_t ms, board_timer_cb cb);
void board_control_led(uint8_t index, uint8_t flag);
void board_init_led(void);
void board_init_key(void);
void board_init_oled(void);
void board_init_pmp(void);
void board_init_clock(void);
uint32_t board_init_uart_clock(UART_Type *ptr);
void board_tsw_phy_set(uint8_t index, bool flag);
void board_init_tsw(void);
void board_init_usb(void);
hpm_stat_t board_init_enet(ENET_Type *ptr);
uint8_t board_get_enet_dma_pbl(ENET_Type *ptr);
hpm_stat_t board_enable_enet_irq(ENET_Type *ptr);
hpm_stat_t board_disable_enet_irq(ENET_Type *ptr);
hpm_stat_t board_init_enet_rgmii_clock_delay(ENET_Type *ptr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
