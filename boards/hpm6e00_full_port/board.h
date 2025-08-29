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

#define BOARD_NAME                      "hpm6e00_full_port"
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

/* LED */
#define BOARD_LED0_GPIO_CTRL            HPM_GPIO0
#define BOARD_LED0_GPIO_INDEX           GPIO_DO_GPIOD
#define BOARD_LED0_GPIO_PIN             0
#define BOARD_LED1_GPIO_CTRL            HPM_GPIO0
#define BOARD_LED1_GPIO_INDEX           GPIO_DO_GPIOD
#define BOARD_LED1_GPIO_PIN             1
#define BOARD_LED0_CTRL_INDEX           0
#define BOARD_LED1_CTRL_INDEX           1
#define BOARD_LED_CTRL_FLAG_OFF         0
#define BOARD_LED_CTRL_FLAG_ON          1
#define BOARD_LED_CTRL_FLAG_TOG         2

/* KEY */
#define BOARD_KEYA_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYA_GPIO_INDEX           GPIO_DI_GPIOE
#define BOARD_KEYA_GPIO_PIN             5

#define BOARD_KEYB_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYB_GPIO_INDEX           GPIO_DI_GPIOE
#define BOARD_KEYB_GPIO_PIN             6

#define BOARD_KEYC_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYC_GPIO_INDEX           GPIO_DI_GPIOE
#define BOARD_KEYC_GPIO_PIN             7

#define BOARD_KEYD_GPIO_CTRL            HPM_GPIO0
#define BOARD_KEYD_GPIO_INDEX           GPIO_DI_GPIOE
#define BOARD_KEYD_GPIO_PIN             2

/* LCD */
#define BOARD_LCD_BL_CTRL               HPM_GPIO0
#define BOARD_LCD_BL_INDEX              GPIO_DO_GPIOF
#define BOARD_LCD_BL_PIN                25

#define BOARD_LCD_D_C_CTRL              HPM_GPIO0
#define BOARD_LCD_D_C_INDEX             GPIO_DO_GPIOF
#define BOARD_LCD_D_C_PIN               28

#define BOARD_LCD_RESET_CTRL            HPM_GPIO0
#define BOARD_LCD_RESET_INDEX           GPIO_DO_GPIOF
#define BOARD_LCD_RESET_PIN             30

/* tftlcd config */
#define BOARD_LCD_SPI                   HPM_SPI7
#define BOARD_LCD_SPI_CLK_NAME          clock_spi7
#define BOARD_LCD_SPI_CLK_FREQ          (20000000UL)

/* TSW section */
#define BOARD_TSN HPM_TSW

#define BOARD_TSN_P1_RESET_CTRL         HPM_GPIO0
#define BOARD_TSN_P1_RESET_INDEX        GPIO_DO_GPIOD
#define BOARD_TSN_P1_RESET_PIN          16
#define BOARD_TSN_P1_INT_CTRL           HPM_GPIO0
#define BOARD_TSN_P1_INT_INDEX          GPIO_DI_GPIOD
#define BOARD_TSN_P1_INT_PIN            15

#define BOARD_TSN_P2_RESET_CTRL         HPM_GPIO0
#define BOARD_TSN_P2_RESET_INDEX        GPIO_DO_GPIOD
#define BOARD_TSN_P2_RESET_PIN          17
#define BOARD_TSN_P2_INT_CTRL           HPM_GPIO0
#define BOARD_TSN_P2_INT_INDEX          GPIO_DI_GPIOD
#define BOARD_TSN_P2_INT_PIN            18

#define BOARD_TSN_P3_RESET_CTRL         HPM_GPIO0
#define BOARD_TSN_P3_RESET_INDEX        GPIO_DO_GPIOD
#define BOARD_TSN_P3_RESET_PIN          22
#define BOARD_TSN_P3_INT_CTRL           HPM_GPIO0
#define BOARD_TSN_P3_INT_INDEX          GPIO_DI_GPIOD
#define BOARD_TSN_P3_INT_PIN            19


/* EtherCAT definitions */
/* ECAT PORT0 must support */
#define BOARD_ECAT_SUPPORT_PORT1 (1)
#define BOARD_ECAT_SUPPORT_PORT2 (1)

#define BOARD_ECAT_PHY_RESET_LEVEL            (0)
#define BOARD_ECAT_PHY0_RESET_GPIO            HPM_GPIO0
#define BOARD_ECAT_PHY0_RESET_GPIO_PORT_INDEX GPIO_DO_GPIOA
#define BOARD_ECAT_PHY0_RESET_PIN_INDEX       (10)

#define BOARD_ECAT_PHY1_RESET_GPIO            HPM_GPIO0
#define BOARD_ECAT_PHY1_RESET_GPIO_PORT_INDEX GPIO_DO_GPIOA
#define BOARD_ECAT_PHY1_RESET_PIN_INDEX       (10)

#define BOARD_ECAT_PHY2_RESET_GPIO            HPM_GPIO0
#define BOARD_ECAT_PHY2_RESET_GPIO_PORT_INDEX GPIO_DO_GPIOA
#define BOARD_ECAT_PHY2_RESET_PIN_INDEX       (10)

#define BOARD_ECAT_IN1_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN1_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN1_GPIO_PIN_INDEX   (10U)

#define BOARD_ECAT_IN2_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN2_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN2_GPIO_PIN_INDEX   (11U)

#define BOARD_ECAT_IN3_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN3_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN3_GPIO_PIN_INDEX   (23U)

#define BOARD_ECAT_IN4_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN4_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN4_GPIO_PIN_INDEX   (24U)

#define BOARD_ECAT_IN5_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN5_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN5_GPIO_PIN_INDEX   (25U)

#define BOARD_ECAT_IN6_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN6_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN6_GPIO_PIN_INDEX   (26U)

#define BOARD_ECAT_IN7_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN7_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN7_GPIO_PIN_INDEX   (27U)

#define BOARD_ECAT_IN8_GPIO             HPM_GPIO0
#define BOARD_ECAT_IN8_GPIO_PORT_INDEX  GPIO_DI_GPIOD
#define BOARD_ECAT_IN8_GPIO_PIN_INDEX   (28U)

#define BOARD_ECAT_OUT1_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT1_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT1_GPIO_PIN_INDEX  (2U)

#define BOARD_ECAT_OUT2_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT2_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT2_GPIO_PIN_INDEX  (3U)

#define BOARD_ECAT_OUT3_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT3_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT3_GPIO_PIN_INDEX  (4U)

#define BOARD_ECAT_OUT4_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT4_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT4_GPIO_PIN_INDEX  (5U)

#define BOARD_ECAT_OUT5_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT5_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT5_GPIO_PIN_INDEX  (6U)

#define BOARD_ECAT_OUT6_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT6_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT6_GPIO_PIN_INDEX  (7U)

#define BOARD_ECAT_OUT7_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT7_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT7_GPIO_PIN_INDEX  (8U)

#define BOARD_ECAT_OUT8_GPIO            HPM_GPIO0
#define BOARD_ECAT_OUT8_GPIO_PORT_INDEX GPIO_DO_GPIOD
#define BOARD_ECAT_OUT8_GPIO_PIN_INDEX  (9U)

#define BOARD_ECAT_OUT_ON_LEVEL (0) /* ECAT control LED on level */

#define BOARD_ECAT_NMII_LINK0_CTRL_INDEX 0
#define BOARD_ECAT_NMII_LINK1_CTRL_INDEX 2
#define BOARD_ECAT_NMII_LINK2_CTRL_INDEX 3
#define BOARD_ECAT_LED_RUN_CTRL_INDEX    4
#define BOARD_ECAT_LED_ERROR_CTRL_INDEX  5
#define BOARD_ECAT_ACT0_CTRL_INDEX       6
#define BOARD_ECAT_ACT1_CTRL_INDEX       7
#define BOARD_ECAT_ACT2_CTRL_INDEX       8

#define BOARD_ECAT_PHY_ADDR_OFFSET (1U)

#ifndef BOARD_SHOW_CLOCK
#define BOARD_SHOW_CLOCK 1
#endif
#ifndef BOARD_SHOW_BANNER
#define BOARD_SHOW_BANNER 1
#endif
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
void board_delay_us(uint32_t us);
void board_delay_ms(uint32_t ms);
void board_timer_create(uint32_t ms, board_timer_cb cb);
void board_control_led(uint8_t index, uint8_t flag);
void board_init_led(void);
void board_init_key(void);
void board_lcd_bl(bool enabled);
void board_lcd_d_c(bool enabled);
void board_lcd_reset(uint16_t ms);
void board_init_lcd(void);
void board_init_pmp(void);
void board_init_clock(void);
uint32_t board_init_uart_clock(UART_Type *ptr);
void board_tsw_phy_set(uint8_t index, bool flag);
void board_init_tsw(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
