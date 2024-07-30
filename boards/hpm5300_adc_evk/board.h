/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_BOARD_H
#define _HPM_BOARD_H
#include <stdio.h>
#include <stdarg.h>
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "pinmux.h"
#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#include "hpm_debug_console.h"
#endif

#define BOARD_NAME "hpm5300_adc_evk"
#define BOARD_UF2_SIGNATURE (0x0A4D5048UL)

/* dma section */
#define BOARD_APP_HDMA HPM_HDMA
#define BOARD_APP_HDMA_IRQ IRQn_HDMA
#define BOARD_APP_DMAMUX HPM_DMAMUX
#define TEST_DMA_CONTROLLER HPM_HDMA
#define TEST_DMA_IRQ IRQn_HDMA

#ifndef BOARD_APP_CORE
#define BOARD_APP_CORE  HPM_CORE0   /* Before programming the demo application, please double check the core number! */
#endif

/* uart section */
#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE HPM_CORE0
#endif
#ifndef BOARD_APP_UART_BASE
#define BOARD_APP_UART_BASE HPM_UART0
#define BOARD_APP_UART_IRQ  IRQn_UART0
#else
#ifndef BOARD_APP_UART_IRQ
#warning no IRQ specified for applicaiton uart
#endif
#endif

#define BOARD_APP_UART_BAUDRATE (115200UL)
#define BOARD_APP_UART_CLK_NAME clock_uart0
#define BOARD_APP_UART_RX_DMA_REQ HPM_DMA_SRC_UART0_RX
#define BOARD_APP_UART_TX_DMA_REQ HPM_DMA_SRC_UART0_TX

#ifndef BOARD_CONSOLE_TYPE
#define BOARD_CONSOLE_TYPE CONSOLE_TYPE_UART
#endif

#if BOARD_CONSOLE_TYPE == CONSOLE_TYPE_UART
#ifndef BOARD_CONSOLE_BASE
#define BOARD_CONSOLE_BASE HPM_UART0
#define BOARD_CONSOLE_CLK_NAME clock_uart0
#endif
#define BOARD_CONSOLE_BAUDRATE (115200UL)
#endif

/*
 * timer for board delay
 */
#define BOARD_DELAY_TIMER (HPM_GPTMR3)
#define BOARD_DELAY_TIMER_CH 0
#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr3)

#define BOARD_CALLBACK_TIMER (HPM_GPTMR3)
#define BOARD_CALLBACK_TIMER_CH 1
#define BOARD_CALLBACK_TIMER_IRQ IRQn_GPTMR3
#define BOARD_CALLBACK_TIMER_CLK_NAME (clock_gptmr3)

/* adc section */
#define BOARD_APP_ADC16_NAME                    "ADC0"
#define BOARD_APP_ADC16_IRQn                    IRQn_ADC0
#define BOARD_APP_ADC16_BASE                    HPM_ADC0
#define BOARD_APP_ADC16_CH_1                    3
#define BOARD_ADC_CLK_DIV                       3
#define BOARD_ADC_SAMPLE_CYCLE                  9
#define USING_EXTERNEL_DCDC                     true

#define BOARD_APP_ADC_TRIG_PWM                  HPM_PWM0
#define BOARD_ADC_TRIGGER_PWM_FRE_IN_KHZ        180000
#define BOARD_ADC_SAMPLE_FRE_IN_KHZ             2000

#define BOARD_ADC_SEQ_START_POS                 (0U)
#define BOARD_ADC_DMA_BUFFER_LEN                (5*4096U)
#define BOARD_ADC_DATA_BUFFER_LEN               (BOARD_ADC_DMA_BUFFER_LEN)
#define BOARD_APP_ADC_PMT_PWM_REFCH_A           (8U)

#define BOARD_APP_ADC_TRIG_TRGM                  HPM_TRGM0
#define BOARD_TRGM_SRC_PWM_CH                    HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_TRGM_TRGOCFG_ADC_STRGI             TRGM_TRGOCFG_ADC0_STRGI

/* USB */
#define BOARD_USB HPM_USB0

#ifndef BOARD_SHOW_CLOCK
#define BOARD_SHOW_CLOCK 1
#endif
#ifndef BOARD_SHOW_BANNER
#define BOARD_SHOW_BANNER 1
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);



void board_init_usb_pins(void);
void board_init_uart(UART_Type *ptr);
void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level);
void board_init_adc16_pins(void);
void board_init(void);
void board_init_clock(void);
void board_delay_us(uint32_t us);
void board_delay_ms(uint32_t ms);
void board_timer_create(uint32_t ms, board_timer_cb cb);
void board_init_pmp(void);
uint32_t board_init_uart_clock(UART_Type *ptr);
uint32_t board_init_adc16_clock(ADC16_Type *ptr, bool clk_src_ahb);
void board_DCDC_power_config(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
