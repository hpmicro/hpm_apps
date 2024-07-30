/*
 * Copyright (c) 2022-2023 HPMicro
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

#define BOARD_NAME "hpm6300_adc_evk"
#define BOARD_UF2_SIGNATURE (0x0A4D5048UL)

/* dma section */
#define BOARD_APP_XDMA HPM_XDMA
#define BOARD_APP_HDMA HPM_HDMA
#define BOARD_APP_XDMA_IRQ IRQn_XDMA
#define BOARD_APP_HDMA_IRQ IRQn_HDMA
#define BOARD_APP_DMAMUX HPM_DMAMUX

/* uart section */
#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE HPM_CORE0
#endif
#ifndef BOARD_APP_UART_BASE
#define BOARD_APP_UART_BASE HPM_UART0
#define BOARD_APP_UART_IRQ  IRQn_UART0
#else
#ifndef BOARD_APP_UART_IRQ
#warning no IRQ specified for application uart
#endif
#endif

#define BOARD_APP_UART_BAUDRATE (115200UL)
#define BOARD_APP_UART_CLK_NAME clock_uart0
#define BOARD_APP_UART_RX_DMA_REQ HPM_DMA_SRC_UART0_RX
#define BOARD_APP_UART_TX_DMA_REQ HPM_DMA_SRC_UART0_TX

#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#ifndef BOARD_CONSOLE_TYPE
#define BOARD_CONSOLE_TYPE CONSOLE_TYPE_UART
#endif

#if CONSOLE_TYPE_UART == BOARD_CONSOLE_TYPE
#ifndef BOARD_CONSOLE_BASE
#if BOARD_RUNNING_CORE == HPM_CORE0
#define BOARD_CONSOLE_BASE HPM_UART0
#define BOARD_CONSOLE_CLK_NAME clock_uart0
#else
#define BOARD_CONSOLE_BASE HPM_UART13
#define BOARD_CONSOLE_CLK_NAME clock_uart13
#endif
#endif
#define BOARD_CONSOLE_BAUDRATE (115200UL)
#endif
#endif

#define BOARD_FREEMASTER_UART_BASE HPM_UART0
#define BOARD_FREEMASTER_UART_IRQ IRQn_UART0
#define BOARD_FREEMASTER_UART_CLK_NAME clock_uart0

/* uart rx idle demo section */
#define BOARD_UART_IDLE HPM_UART2
#define BOARD_UART_IDLE_IRQ        IRQn_UART2
#define BOARD_UART_IDLE_CLK_NAME   clock_uart2
#define BOARD_UART_IDLE_TX_DMA_SRC HPM_DMA_SRC_UART2_TX
#define BOARD_UART_IDLE_DMA_SRC HPM_DMA_SRC_UART2_RX

#define BOARD_UART_IDLE_TRGM HPM_TRGM1
#define BOARD_UART_IDLE_TRGM_PIN IOC_PAD_PA24
#define BOARD_UART_IDLE_TRGM_INPUT_SRC HPM_TRGM1_INPUT_SRC_TRGM1_P4
#define BOARD_UART_IDLE_TRGM_OUTPUT_GPTMR_IN HPM_TRGM1_OUTPUT_SRC_GPTMR2_IN2
#define BOARD_UART_IDLE_TRGM_OUTPUT_GPTMR_SYNCI HPM_TRGM1_OUTPUT_SRC_GPTMR2_SYNCI

#define BOARD_UART_IDLE_GPTMR HPM_GPTMR2
#define BOARD_UART_IDLE_GPTMR_CLK_NAME clock_gptmr2
#define BOARD_UART_IDLE_GPTMR_IRQ IRQn_GPTMR2
#define BOARD_UART_IDLE_GPTMR_CMP_CH 0
#define BOARD_UART_IDLE_GPTMR_CAP_CH 2

#ifndef BOARD_APP_CORE
#define BOARD_APP_CORE  HPM_CORE0   /* Before programming the demo application, please double check the core number! */
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
#define BOARD_APP_ADC16_NAME "ADC0"
#define BOARD_APP_ADC16_IRQn IRQn_ADC0
#define BOARD_APP_ADC16_BASE HPM_ADC0
#define BOARD_APP_ADC16_CH_1    8
#define BOARD_ADC_CLK_DIV        4
#define BOARD_ADC_SAMPLE_CYCLE   4
#define BOARD_VSOC_USE_EXTERNEL_DCDC      true

#define BOARD_APP_ADC_TRIG_PWM    HPM_PWM0
#define BOARD_ADC_TRIGGER_PWM_FRE_IN_KHZ 200000
#define BOARD_ADC_SAMPLE_FRE_IN_KHZ 2000

#define BOARD_ADC_SEQ_START_POS                  (0U)
#define BOARD_ADC_DMA_BUFFER_LEN                 (4096U)
#define BOARD_ADC_DATA_BUFFER_LEN                (2*BOARD_ADC_DMA_BUFFER_LEN)
#define BOARD_APP_ADC_PMT_PWM_REFCH_A            (8U)

#define BOARD_APP_ADC_TRIG_TRGM                 HPM_TRGM0
#define BOARD_TRGM_SRC_PWM_CH                   HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_TRGM_TRGOCFG_ADC_STRGI            TRGM_TRGOCFG_ADC0_STRGI


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);

void board_init(void);
void board_init_console(void);

void board_init_uart(UART_Type *ptr);

/* Initialize SoC overall clocks */
void board_init_clock(void);

uint32_t board_init_adc16_clock(ADC16_Type *ptr, bool clk_src_ahb);

void board_init_adc16_pins(void);

/*
 * @brief Initialize PMP and PMA for but not limited to the following purposes:
 *      -- non-cacheable memory initialization
 */
void board_init_pmp(void);

void board_delay_us(uint32_t us);
void board_delay_ms(uint32_t ms);

void board_timer_create(uint32_t ms, board_timer_cb cb);

/* Initialize the UART clock */
uint32_t board_init_uart_clock(UART_Type *ptr);

void board_DCDC_power_config(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
