/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_BOARD_H
#define _HPM_BOARD_H
#include <stdio.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "hpm_clock_drv.h"
#include "pinmux.h"

#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#include "hpm_debug_console.h"
#endif

#define BOARD_NAME "hpm6750_adc_evk"
#define BOARD_UF2_SIGNATURE (0x0A4D5048UL)

#define SEC_CORE_IMG_START ILM_LOCAL_BASE

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

#ifndef BOARD_CONSOLE_TYPE
#define BOARD_CONSOLE_TYPE CONSOLE_TYPE_UART
#endif

#if BOARD_CONSOLE_TYPE == CONSOLE_TYPE_UART
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

#ifndef BOARD_APP_CORE
#define BOARD_APP_CORE  HPM_CORE0   /* Before programming the demo application, please double check the core number! */
#endif

/*
 * timer for board delay
 */
#define BOARD_DELAY_TIMER (HPM_GPTMR7)
#define BOARD_DELAY_TIMER_CH 0
#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr7)

#define BOARD_CALLBACK_TIMER (HPM_GPTMR7)
#define BOARD_CALLBACK_TIMER_CH 1
#define BOARD_CALLBACK_TIMER_IRQ IRQn_GPTMR7
#define BOARD_CALLBACK_TIMER_CLK_NAME (clock_gptmr7)


#define BOARD_LED_PULL_STATUS IOC_PAD_PAD_CTL_PS_GET(HPM_IOC->PAD[IOC_PAD_PB18].PAD_CTL)
#define BOARD_LED_OFF_LEVEL BOARD_LED_PULL_STATUS
#define BOARD_LED_ON_LEVEL !BOARD_LED_PULL_STATUS

#define BOARD_LED_GPIO_CTRL       HPM_GPIO0
#define BOARD_LED_GPIO_INDEX      GPIO_DI_GPIOB
#define BOARD_LED_GPIO_PIN        18





#define BOARD_APP_ADC_TRIG_TRGM                 HPM_TRGM3
#define BOARD_TRGM_SRC_PWM_CH                   HPM_TRGM3_INPUT_SRC_PWM3_CH8REF
#define BOARD_TRGM_TRGOCFG_ADC_STRGI            TRGM_TRGOCFG_ADC3_STRGI

/* dma section */
#define BOARD_APP_XDMA HPM_XDMA
#define BOARD_APP_HDMA HPM_HDMA
#define BOARD_APP_XDMA_IRQ IRQn_XDMA
#define BOARD_APP_HDMA_IRQ IRQn_HDMA
#define BOARD_APP_DMAMUX HPM_DMAMUX

/* Flash section */
#define BOARD_APP_XPI_NOR_XPI_BASE            (HPM_XPI0)
#define BOARD_APP_XPI_NOR_CFG_OPT_HDR         (0xfcf90001U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT0        (0x00000007U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT1        (0x00000000U)

#define BOARD_FLASH_BASE_ADDRESS (0x80000000UL)
#define BOARD_FLASH_SIZE (8 << 20)

/* adc section */
#define BOARD_APP_ADC12_NAME "ADC0"
#define BOARD_APP_ADC12_BASE HPM_ADC0
#define BOARD_APP_ADC12_IRQn IRQn_ADC0
#define BOARD_APP_ADC12_CH_1                     (7U)

#define BOARD_APP_ADC16_NAME "ADC3"
#define BOARD_APP_ADC16_IRQn IRQn_ADC3
#define BOARD_APP_ADC16_BASE HPM_ADC3
#define BOARD_APP_ADC16_CH_1                     (0U)

#define BOARD_APP_ADC_TRIG_PWM    HPM_PWM3
#define BOARD_ADC_TRIGGER_PWM_FRE_IN_KHZ 200000
#define BOARD_ADC_SAMPLE_FRE_IN_KHZ 2000

#define BOARD_ADC_CLK_DIV        4
#define BOARD_ADC_SAMPLE_CYCLE   4
#define BOARD_VSOC_USE_EXTERNEL_DCDC     false

/* USB section */
#define BOARD_USB0_ID_PORT       (HPM_GPIO0)
#define BOARD_USB0_ID_GPIO_INDEX (GPIO_DO_GPIOF)
#define BOARD_USB0_ID_GPIO_PIN   (10)

#define BOARD_USB0_OC_PORT       (HPM_GPIO0)
#define BOARD_USB0_OC_GPIO_INDEX (GPIO_DI_GPIOF)
#define BOARD_USB0_OC_GPIO_PIN   (8)

#define BOARD_ADC_SEQ_START_POS                  (0U)
#define BOARD_ADC_DMA_BUFFER_LEN                 (1024U)
#define BOARD_ADC_DATA_BUFFER_LEN                (100*BOARD_ADC_DMA_BUFFER_LEN)
#define BOARD_APP_ADC_PMT_PWM_REFCH_A            (8U)


#define BOARD_CPU_FREQ (648000000UL)


#ifndef BOARD_SHOW_CLOCK
#define BOARD_SHOW_CLOCK 1
#endif
#ifndef BOARD_SHOW_BANNER
#define BOARD_SHOW_BANNER 1
#endif

#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE 0
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);

void board_init(void);

void board_delay_ms(uint32_t ms);

void board_DCDC_power_config(void);


void board_init_console(void);

/* Initialize SoC overall clocks */
void board_init_clock(void);

uint32_t board_init_adc12_clock(ADC12_Type *ptr, bool clk_src_ahb);

uint32_t board_init_adc16_clock(ADC16_Type *ptr, bool clk_src_ahb);

void board_init_adc12_pins(void);
void board_init_adc16_pins(void);

void board_init_usb_pins(void);

void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level);

/*
 * @brief Initialize PMP and PMA for but not limited to the following purposes:
 *      -- non-cacheable memory initialization
 */
void board_init_pmp(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
