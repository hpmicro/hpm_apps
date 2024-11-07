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

#define BOARD_NAME          "hpm6e00-validation"
#define BOARD_UF2_SIGNATURE (0x0A4D5048UL)
#define BOARD_CPU_FREQ      (600000000UL)

#define SEC_CORE_IMG_START CORE1_ILM_LOCAL_BASE

#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE HPM_CORE0
#endif

/* ACMP desction */
#define BOARD_ACMP             HPM_ACMP0
#define BOARD_ACMP_CHANNEL     ACMP_CHANNEL_CHN1
#define BOARD_ACMP_IRQ         IRQn_ACMP0_1
#define BOARD_ACMP_PLUS_INPUT  ACMP_INPUT_DAC_OUT  /* use internal DAC */
#define BOARD_ACMP_MINUS_INPUT ACMP_INPUT_ANALOG_4 /* align with used pin */

/* uart section */
#define BOARD_APP_UART_BASE       HPM_UART6
#define BOARD_APP_UART_IRQ        IRQn_UART6
#define BOARD_APP_UART_BAUDRATE   (115200UL)
#define BOARD_APP_UART_CLK_NAME   clock_uart6
#define BOARD_APP_UART_RX_DMA_REQ HPM_DMA_SRC_UART6_RX
#define BOARD_APP_UART_TX_DMA_REQ HPM_DMA_SRC_UART6_TX

#define BOARD_APP_UART_BREAK_SIGNAL_PIN IOC_PAD_PA05

/* uart rx idle demo section */
#define BOARD_UART_IDLE            BOARD_APP_UART_BASE
#define BOARD_UART_IDLE_IRQ        BOARD_APP_UART_IRQ
#define BOARD_UART_IDLE_CLK_NAME   BOARD_APP_UART_CLK_NAME
#define BOARD_UART_IDLE_TX_DMA_SRC BOARD_APP_UART_TX_DMA_REQ
#define BOARD_UART_IDLE_DMA_SRC    BOARD_APP_UART_RX_DMA_REQ

#define BOARD_UART_IDLE_GPTMR          HPM_GPTMR4
#define BOARD_UART_IDLE_GPTMR_CLK_NAME clock_gptmr4
#define BOARD_UART_IDLE_GPTMR_IRQ      IRQn_GPTMR4
#define BOARD_UART_IDLE_GPTMR_CMP_CH   0
#define BOARD_UART_IDLE_GPTMR_CAP_CH   2

/* uart microros sample section */
#define BOARD_MICROROS_UART_BASE     BOARD_APP_UART_BASE
#define BOARD_MICROROS_UART_IRQ      BOARD_APP_UART_IRQ
#define BOARD_MICROROS_UART_CLK_NAME BOARD_APP_UART_CLK_NAME

/* usb cdc acm uart section */
#define BOARD_USB_CDC_ACM_UART            BOARD_APP_UART_BASE
#define BOARD_USB_CDC_ACM_UART_CLK_NAME   BOARD_APP_UART_CLK_NAME
#define BOARD_USB_CDC_ACM_UART_TX_DMA_SRC BOARD_APP_UART_TX_DMA_REQ
#define BOARD_USB_CDC_ACM_UART_RX_DMA_SRC BOARD_APP_UART_RX_DMA_REQ

/* uart lin sample section */
#define BOARD_UART_LIN          BOARD_APP_UART_BASE
#define BOARD_UART_LIN_IRQ      BOARD_APP_UART_IRQ
#define BOARD_UART_LIN_CLK_NAME BOARD_APP_UART_CLK_NAME
#define BOARD_UART_LIN_TX_PORT  GPIO_DI_GPIOC
#define BOARD_UART_LIN_TX_PIN   (23U) /* PC23 should align with used pin in pinmux configuration */

#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#ifndef BOARD_CONSOLE_TYPE
#define BOARD_CONSOLE_TYPE CONSOLE_TYPE_UART
#endif

#if BOARD_CONSOLE_TYPE == CONSOLE_TYPE_UART
#ifndef BOARD_CONSOLE_UART_BASE
#if BOARD_RUNNING_CORE == HPM_CORE0
#define BOARD_CONSOLE_UART_BASE       HPM_UART0
#define BOARD_CONSOLE_UART_CLK_NAME   clock_uart0
#define BOARD_CONSOLE_UART_IRQ        IRQn_UART0
#define BOARD_CONSOLE_UART_TX_DMA_REQ HPM_DMA_SRC_UART0_TX
#define BOARD_CONSOLE_UART_RX_DMA_REQ HPM_DMA_SRC_UART0_RX
#else
#define BOARD_CONSOLE_UART_BASE       HPM_UART11
#define BOARD_CONSOLE_UART_CLK_NAME   clock_uart11
#define BOARD_CONSOLE_UART_IRQ        IRQn_UART11
#define BOARD_CONSOLE_UART_TX_DMA_REQ HPM_DMA_SRC_UART11_TX
#define BOARD_CONSOLE_UART_RX_DMA_REQ HPM_DMA_SRC_UART11_RX
#endif
#endif
#define BOARD_CONSOLE_UART_BAUDRATE (115200UL)
#endif
#endif

/* rtthread-nano finsh section */
#define BOARD_RT_CONSOLE_BASE BOARD_CONSOLE_UART_BASE

/* modbus sample section */
#define BOARD_MODBUS_UART_BASE       BOARD_APP_UART_BASE
#define BOARD_MODBUS_UART_CLK_NAME   BOARD_APP_UART_CLK_NAME
#define BOARD_MODBUS_UART_RX_DMA_REQ BOARD_APP_UART_RX_DMA_REQ
#define BOARD_MODBUS_UART_TX_DMA_REQ BOARD_APP_UART_TX_DMA_REQ

/* sdram section */
#define BOARD_SDRAM_ADDRESS       (0x40000000UL)
#define BOARD_SDRAM_SIZE          (64 * SIZE_1MB)
#define BOARD_SDRAM_CS            FEMC_SDRAM_CS0
#define BOARD_SDRAM_PORT_SIZE     FEMC_SDRAM_PORT_SIZE_32_BITS
#define BOARD_SDRAM_REFRESH_COUNT (8192UL)
#define BOARD_SDRAM_REFRESH_IN_MS (64UL)

/* nor flash section */
#define BOARD_FLASH_BASE_ADDRESS (0x80000000UL)
#define BOARD_FLASH_SIZE         (16 * SIZE_1MB)


/* pdm selection */
#define BOARD_PDM_SINGLE_CHANNEL_MASK (1U)
#define BOARD_PDM_DUAL_CHANNEL_MASK   (0x21U)

/* dma section */
#define BOARD_APP_XDMA      HPM_XDMA
#define BOARD_APP_HDMA      HPM_HDMA
#define BOARD_APP_XDMA_IRQ  IRQn_XDMA
#define BOARD_APP_HDMA_IRQ  IRQn_HDMA
#define BOARD_APP_DMAMUX    HPM_DMAMUX
#define TEST_DMA_CONTROLLER HPM_HDMA
#define TEST_DMA_IRQ        IRQn_HDMA

/* APP PWM */
#define BOARD_APP_PWM             HPM_PWM0
#define BOARD_APP_PWM_CLOCK_NAME  clock_pwm0
#define BOARD_APP_PWM_OUT1        0
#define BOARD_APP_PWM_OUT2        1
#define BOARD_APP_PWM_OUT3        2
#define BOARD_APP_PWM_OUT4        3
#define BOARD_APP_PWM_FAULT_PIN   (5)
#define BOARD_APP_TRGM            HPM_TRGM0
#define BOARD_APP_PWM_IRQ         IRQn_PWM0
#define BOARD_APP_TRGM_PWM_OUTPUT HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN0
#define BOARD_APP_TRGM_PWM_INPUT  HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0

/* gptmr section */
#define BOARD_GPTMR                   HPM_GPTMR2
#define BOARD_GPTMR_IRQ               IRQn_GPTMR2
#define BOARD_GPTMR_CHANNEL           0
#define BOARD_GPTMR_DMA_SRC           HPM_DMA_SRC_GPTMR2_0
#define BOARD_GPTMR_CLK_NAME          clock_gptmr2
#define BOARD_GPTMR_PWM               HPM_GPTMR2
#define BOARD_GPTMR_PWM_CHANNEL       0
#define BOARD_GPTMR_PWM_DMA_SRC       HPM_DMA_SRC_GPTMR2_0
#define BOARD_GPTMR_PWM_CLK_NAME      clock_gptmr2
#define BOARD_GPTMR_PWM_IRQ           IRQn_GPTMR2
#define BOARD_GPTMR_PWM_SYNC          HPM_GPTMR2
#define BOARD_GPTMR_PWM_SYNC_CHANNEL  1
#define BOARD_GPTMR_PWM_SYNC_CLK_NAME clock_gptmr2

/* User button */
#define BOARD_APP_GPIO_CTRL  HPM_GPIO0
#define BOARD_APP_GPIO_INDEX GPIO_DI_GPIOA
#define BOARD_APP_GPIO_PIN   10
#define BOARD_APP_GPIO_IRQ   IRQn_GPIO0_A

/* gpiom section */
#define BOARD_APP_GPIOM_BASE            HPM_GPIOM
#define BOARD_APP_GPIOM_USING_CTRL      HPM_FGPIO
#define BOARD_APP_GPIOM_USING_CTRL_NAME gpiom_core0_fast

/* Flash section */
#define BOARD_APP_XPI_NOR_XPI_BASE     (HPM_XPI0)
#define BOARD_APP_XPI_NOR_CFG_OPT_HDR  (0xfcf90001U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT0 (0x00000005U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT1 (0x00001000U)

/* ADC section */
#define BOARD_APP_ADC16_NAME     "ADC1"
#define BOARD_APP_ADC16_BASE     HPM_ADC1
#define BOARD_APP_ADC16_IRQn     IRQn_ADC1
#define BOARD_APP_ADC16_CH_1     (3U)
#define BOARD_APP_ADC_TRIG_PWM    HPM_PWM0

#define  BOARD_ADC_SEQ_START_POS        (0U)
#define  BOARD_ADC_DMA_BUFFER_LEN         4096
#define  BOARD_ADC_DATA_BUFFER_LEN        (BOARD_ADC_DMA_BUFFER_LEN*6)

#define  BOARD_APP_ADC_TRIG_TRGM           HPM_TRGM0
#define  BOARD_TRGM_SRC_PWM_CH             HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0
#define  BOARD_TRGM_TRGOCFG_ADC_STRGI      TRGM_TRGOCFG_ADC1_STRGI
#define  APP_ADC16_TRIG_SRC_FREQUENCY      (2000000U)
#define  APP_ADC16_HW_TRGM_SRC_OUT_CH       (0U)
#define  BOARD_ADC_CLK_DIV                  4
#define  BOARD_ADC_SAMPLE_CYCLE             4

#ifndef BOARD_APP_CORE
#define BOARD_APP_CORE BOARD_RUNNING_CORE
#endif

#define USING_EXTERNEL_DCDC      true

#define BOARD_APP_ADC16_HW_TRIG_SRC_CLK_NAME clock_pwm0
#define BOARD_APP_ADC16_HW_TRIG_SRC          HPM_PWM0
#define BOARD_APP_ADC16_HW_TRGM              HPM_TRGM0
#define BOARD_APP_ADC16_HW_TRGM_IN           HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0
#define BOARD_APP_ADC16_HW_TRGM_OUT_SEQ      TRGM_TRGOCFG_ADC0_STRGI
#define BOARD_APP_ADC16_HW_TRGM_OUT_PMT      TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_APP_ADC16_PMT_TRIG_CH          ADC16_CONFIG_TRG0A

/*
 * timer for board delay
 */
#define BOARD_DELAY_TIMER          (HPM_GPTMR3)
#define BOARD_DELAY_TIMER_CH       0
#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr3)

#define BOARD_CALLBACK_TIMER          (HPM_GPTMR3)
#define BOARD_CALLBACK_TIMER_CH       1
#define BOARD_CALLBACK_TIMER_IRQ      IRQn_GPTMR3
#define BOARD_CALLBACK_TIMER_CLK_NAME (clock_gptmr3)

/* USB section */
#define BOARD_USB HPM_USB0

/* LED */
#define BOARD_LED_GPIO_CTRL  HPM_GPIO0
#define BOARD_LED_GPIO_INDEX GPIO_DI_GPIOA
#define BOARD_LED_GPIO_PIN   9
#define BOARD_LED_OFF_LEVEL  1
#define BOARD_LED_ON_LEVEL   0

#ifndef BOARD_SHOW_CLOCK
#define BOARD_SHOW_CLOCK 1
#endif
#ifndef BOARD_SHOW_BANNER
#define BOARD_SHOW_BANNER 1
#endif

/*BLDC PWM */
#define BOARD_BLDCPWM              HPM_PWM2
#define BOARD_BLDC_UH_PWM_OUTPIN   (0U)
#define BOARD_BLDC_UL_PWM_OUTPIN   (1U)
#define BOARD_BLDC_VH_PWM_OUTPIN   (2U)
#define BOARD_BLDC_VL_PWM_OUTPIN   (3U)
#define BOARD_BLDC_WH_PWM_OUTPIN   (4U)
#define BOARD_BLDC_WL_PWM_OUTPIN   (5U)
#define BOARD_BLDCPWM_TRGM         HPM_TRGM0
#define BOARD_BLDCAPP_PWM_IRQ      IRQn_PWM2
#define BOARD_BLDCPWM_CMP_INDEX_0  (0U)
#define BOARD_BLDCPWM_CMP_INDEX_1  (1U)
#define BOARD_BLDCPWM_CMP_INDEX_2  (2U)
#define BOARD_BLDCPWM_CMP_INDEX_3  (3U)
#define BOARD_BLDCPWM_CMP_INDEX_4  (4U)
#define BOARD_BLDCPWM_CMP_INDEX_5  (5U)
#define BOARD_BLDCPWM_CMP_INDEX_6  (6U)
#define BOARD_BLDCPWM_CMP_INDEX_7  (7U)
#define BOARD_BLDCPWM_CMP_TRIG_CMP (20U)

/* BLDC ADC */
#define BOARD_BLDC_ADC_MODULE    ADCX_MODULE_ADC16
#define BOARD_BLDC_ADC_U_BASE    HPM_ADC0
#define BOARD_BLDC_ADC_V_BASE    HPM_ADC1
#define BOARD_BLDC_ADC_W_BASE    HPM_ADC2
#define BOARD_BLDC_ADC_TRIG_FLAG adc16_event_trig_complete

#define BOARD_BLDC_ADC_CH_U                   (7U)
#define BOARD_BLDC_ADC_CH_V                   (12U)
#define BOARD_BLDC_ADC_CH_W                   (2U)
#define BOARD_BLDC_ADC_IRQn                   IRQn_ADC0
#define BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES (ADC_SOC_PMT_MAX_DMA_BUFF_LEN_IN_4BYTES)
#define BOARD_BLDC_ADC_TRG                    ADC16_CONFIG_TRG0A
#define BOARD_BLDC_ADC_PREEMPT_TRIG_LEN       (1U)
#define BOARD_BLDC_TRG_NUM                    HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A
#define BOARD_BLDC_PWM_TRIG_OUT_CHN           (0U)


/* PLB */
#define BOARD_PLB_COUNTER              HPM_PLB
#define BOARD_PLB_PWM_BASE             HPM_PWM0
#define BOARD_PLB_PWM_CLOCK_NAME       clock_mot0
#define BOARD_PLB_TRGM                 HPM_TRGM0
#define BOARD_PLB_PWM_TRG              (HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0)
#define BOARD_PLB_IN_PWM_TRG_NUM       (TRGM_TRGOCFG_PLB_IN_00)
#define BOARD_PLB_IN_PWM_PULSE_TRG_NUM (TRGM_TRGOCFG_PLB_IN_02)
#define BOARD_PLB_CLR_SIGNAL_INPUT     (HPM_TRGM0_INPUT_SRC_PLB_OUT32)
#define BOARD_PLB_TYPEB_INPUT0         (TRGM_TRGOCFG_PLB_IN_32)
#define BOARD_PLB_OUT_TRG              (HPM_TRGM0_INPUT_SRC_PLB_OUT00)
#define BOARD_PLB_IO_TRG_NUM           (TRGM_TRGOCFG_MOT_GPIO2)
#define BOARD_PLB_IO_TRG_SHIFT         (2)
#define BOARD_PLB_PWM_CMP              (8U)
#define BOARD_PLB_PWM_CHN              (8U)
#define BOARD_PLB_CHN                  plb_chn0


/* sdm section */
#define BOARD_SDM                 HPM_SDM0
#define BOARD_SDM_IRQ             IRQn_SDM0
#define BOARD_SDM_CHANNEL         0
#define BOARD_SDM_TRGM            HPM_TRGM0
#define BOARD_SDM_TRGM_GPTMR      HPM_GPTMR3
#define BOARD_SDM_TRGM_GPTMR_CH   2
#define BOARD_SDM_TRGM_INPUT_SRC  HPM_TRGM0_INPUT_SRC_GPTMR3_OUT2
#define BOARD_SDM_TRGM_OUTPUT_DST HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC0

/* LOBS */
#define BOARD_LOBS_TRIG_GROUP 5 /* group5 <--> PF */
#define BOARD_LOBS_TRIG_PIN   26

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);

void board_init(void);
void board_init_console(void);

void board_init_core1(void);

void board_init_uart(UART_Type *ptr);
void board_init_i2c(I2C_Type *ptr);
void board_init_can(MCAN_Type *ptr);

void board_init_sdram_pins(void);
void board_init_gpio_pins(void);
void board_init_spi_pins(SPI_Type *ptr);
void board_init_spi_pins_with_gpio_as_cs(SPI_Type *ptr);
void board_write_spi_cs(uint32_t pin, uint8_t state);
uint8_t board_get_led_gpio_off_level(void);
void board_init_led_pins(void);

void board_led_write(uint8_t state);
void board_led_toggle(void);

/* Initialize SoC overall clocks */
void board_init_clock(void);
uint32_t board_init_femc_clock(void);
uint32_t board_init_uart_clock(UART_Type *ptr);
uint32_t board_init_adc16_clock(void *ptr, bool clk_src_bus);

void board_init_adc16_pins(void);

void board_init_usb_pins(void);
void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level);

void board_init_pmp(void);

void board_delay_us(uint32_t us);
void board_delay_ms(uint32_t ms);

void board_timer_create(uint32_t ms, board_timer_cb cb);
void board_ungate_mchtmr_at_lp_mode(void);

/*
 * Get GPIO pin level of onboard LED
 */
uint8_t board_get_led_gpio_off_level(void);

void init_pwm_fault_pins(void);

void board_DCDC_power_config(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
