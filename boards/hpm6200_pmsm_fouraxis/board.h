/*
 * Copyright (c) 2023 HPMicro
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

#define BOARD_NAME "hpm6200evk"
#define BOARD_UF2_SIGNATURE (0x0A4D5048UL)

#define SEC_CORE_IMG_START CORE1_ILM_LOCAL_BASE

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
#if BOARD_RUNNING_CORE == HPM_CORE0
#define BOARD_CONSOLE_BASE HPM_UART0
#define BOARD_CONSOLE_CLK_NAME clock_uart0
#else
#define BOARD_CONSOLE_BASE HPM_UART2
#define BOARD_CONSOLE_CLK_NAME clock_uart2
#endif
#endif
#define BOARD_CONSOLE_BAUDRATE (115200UL)
#endif

#define BOARD_FREEMASTER_UART_BASE HPM_UART0
#define BOARD_FREEMASTER_UART_IRQ IRQn_UART0
#define BOARD_FREEMASTER_UART_CLK_NAME clock_uart0

/* sdm section */
#define BOARD_SDM HPM_SDM
#define BOARD_SDM_IRQ IRQn_SDFM
#define BOARD_SDM_CHANNEL 3
#define BOARD_SDM_TRGM                     HPM_TRGM3
#define BOARD_SDM_TRGM_GPTMR               HPM_GPTMR3
#define BOARD_SDM_TRGM_GPTMR_CH            2
#define BOARD_SDM_TRGM_INPUT_SRC           HPM_TRGM3_INPUT_SRC_GPTMR3_OUT2
#define BOARD_SDM_TRGM_OUTPUT_DST          HPM_TRGM3_OUTPUT_SRC_SDFM_TRG15

/* lin section */
#define BOARD_LIN             HPM_LIN0
#define BOARD_LIN_CLK_NAME    clock_lin0
#define BOARD_LIN_IRQ         IRQn_LIN0
#define BOARD_LIN_BAUDRATE    (19200U)

/* nor flash section */
#define BOARD_FLASH_BASE_ADDRESS (0x80000000UL)
#define BOARD_FLASH_SIZE (16 * SIZE_1MB)

/* i2c section */
#define BOARD_APP_I2C_BASE HPM_I2C0
#define BOARD_APP_I2C_IRQ IRQn_I2C0
#define BOARD_APP_I2C_CLK_NAME clock_i2c0
#define BOARD_APP_I2C_DMA HPM_HDMA
#define BOARD_APP_I2C_DMAMUX HPM_DMAMUX
#define BOARD_APP_I2C_DMA_SRC HPM_DMA_SRC_I2C0
#define BOARD_APP_I2C_DMAMUX_CH DMAMUX_MUXCFG_HDMA_MUX0

/* ACMP desction */
#define BOARD_ACMP HPM_ACMP
#define BOARD_ACMP_CHANNEL ACMP_CHANNEL_CHN1
#define BOARD_ACMP_IRQ IRQn_ACMP_1
#define BOARD_ACMP_PLUS_INPUT ACMP_INPUT_DAC_OUT /* use internal DAC */
#define BOARD_ACMP_MINUS_INPUT ACMP_INPUT_ANALOG_5 /* align with used pin */

/* dma section */
#define BOARD_APP_XDMA HPM_XDMA
#define BOARD_APP_HDMA HPM_HDMA
#define BOARD_APP_XDMA_IRQ IRQn_XDMA
#define BOARD_APP_HDMA_IRQ IRQn_HDMA
#define BOARD_APP_DMAMUX HPM_DMAMUX

/* gptmr section */
#define BOARD_GPTMR HPM_GPTMR2
#define BOARD_GPTMR_IRQ IRQn_GPTMR2
#define BOARD_GPTMR_CHANNEL 0
#define BOARD_GPTMR_PWM HPM_GPTMR2
#define BOARD_GPTMR_PWM_CHANNEL 0
#define BOARD_GPTMR_CLK_NAME clock_gptmr2


#define BOARD_LED_GPIO_CTRL BOARD_G_GPIO_CTRL
#define BOARD_LED_GPIO_INDEX BOARD_G_GPIO_INDEX
#define BOARD_LED_GPIO_PIN BOARD_G_GPIO_PIN

#define BOARD_LED_OFF_LEVEL 0
#define BOARD_LED_ON_LEVEL !BOARD_LED_OFF_LEVEL
#define BOARD_LED_TOGGLE_RGB 1

#define BOARD_APP_GPIO_INDEX GPIO_DI_GPIOZ
#define BOARD_APP_GPIO_PIN 2

/* pinmux section */
#define USING_GPIO0_FOR_GPIOZ
#ifndef USING_GPIO0_FOR_GPIOZ
#define BOARD_APP_GPIO_CTRL HPM_BGPIO
#define BOARD_APP_GPIO_IRQ IRQn_BGPIO
#else
#define BOARD_APP_GPIO_CTRL HPM_GPIO0
#define BOARD_APP_GPIO_IRQ IRQn_GPIO0_Z
#endif

/* gpiom section */
#define BOARD_APP_GPIOM_BASE            HPM_GPIOM
#define BOARD_APP_GPIOM_USING_CTRL      HPM_FGPIO
#define BOARD_APP_GPIOM_USING_CTRL_NAME gpiom_core0_fast

/* spi section */
#define BOARD_APP_SPI_BASE              HPM_SPI1
#define BOARD_APP_SPI_CLK_NAME          clock_spi1
#define BOARD_APP_SPI_IRQ               IRQn_SPI1
#define BOARD_APP_SPI_SCLK_FREQ         (20000000UL)
#define BOARD_APP_SPI_ADDR_LEN_IN_BYTES (1U)
#define BOARD_APP_SPI_DATA_LEN_IN_BITS  (8U)
#define BOARD_APP_SPI_RX_DMA HPM_DMA_SRC_SPI1_RX
#define BOARD_APP_SPI_RX_DMAMUX_CH DMAMUX_MUXCFG_HDMA_MUX0
#define BOARD_APP_SPI_TX_DMA HPM_DMA_SRC_SPI1_TX
#define BOARD_APP_SPI_TX_DMAMUX_CH DMAMUX_MUXCFG_HDMA_MUX1
#define BOARD_SPI_CS_GPIO_CTRL           HPM_GPIO0
#define BOARD_SPI_CS_PIN                 IOC_PAD_PB02
#define BOARD_SPI_CS_ACTIVE_LEVEL        (0U)

/* mtimer section */
#define BOARD_MCHTMR_FREQ_IN_HZ (1000000UL)

/* Flash section */
#define BOARD_APP_XPI_NOR_XPI_BASE            (HPM_XPI0)
#define BOARD_APP_XPI_NOR_CFG_OPT_HDR         (0xfcf90001U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT0        (0x00000005U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT1        (0x00001000U)

/* ADC section */
#define BOARD_APP_ADC16_NAME "ADC0"
#define BOARD_APP_ADC16_BASE HPM_ADC0
#define BOARD_APP_ADC16_IRQn IRQn_ADC0
#define BOARD_APP_ADC16_CH_1                     (1U)
#define BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES     (1024U)
#define BOARD_APP_ADC_PMT_DMA_SIZE_IN_4BYTES     (192U)
#define BOARD_APP_ADC_PREEMPT_TRIG_LEN           (1U)
#define BOARD_APP_ADC_SINGLE_CONV_CNT            (6)
#define BOARD_APP_ADC_TRIG_PWMT0                 HPM_PWM0
#define BOARD_APP_ADC_TRIG_PWMT1                 HPM_PWM1
#define BOARD_APP_ADC_TRIG_TRGM0                 HPM_TRGM0
#define BOARD_APP_ADC_TRIG_TRGM1                 HPM_TRGM1
#define BOARD_APP_ADC_TRIG_PWM_SYNC              HPM_SYNT

/* DAC section */
#define BOARD_DAC_BASE       HPM_DAC0
#define BOARD_DAC_IRQn       IRQn_DAC0
#define BOARD_DAC_CLOCK_NAME clock_dac0

/* CAN section */
#define BOARD_APP_CAN_BASE                       HPM_MCAN0
#define BOARD_APP_CAN_IRQn                       IRQn_CAN0

/*
 * timer for board delay
 */
//#define BOARD_DELAY_TIMER (HPM_GPTMR3)
//#define BOARD_DELAY_TIMER_CH 0
//#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr3)

#define BOARD_DELAY_TIMER (HPM_GPTMR1)
#define BOARD_DELAY_TIMER_CH 0
#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr1)

#define BOARD_CALLBACK_TIMER (HPM_GPTMR3)
#define BOARD_CALLBACK_TIMER_CH 1
//#define BOARD_CALLBACK_TIMER_IRQ IRQn_GPTMR3
#define BOARD_CALLBACK_TIMER_CLK_NAME (clock_gptmr3)
#define BOARD_CALLBACK_TIMER_IRQ IRQn_GPTMR1  //和5号轴定时器中断冲突
/* USB section */
#define BOARD_USB0_ID_PORT       (HPM_GPIO0)
#define BOARD_USB0_ID_GPIO_INDEX (GPIO_DO_GPIOC)
#define BOARD_USB0_ID_GPIO_PIN   (23)

/*stepper motor*/
#define BOARD_STEPPER0_MOTOR_CLOCK_SOURCE   clock_mot0
#define BOARD_STEPPER1_MOTOR_CLOCK_SOURCE   clock_mot1
#define BOARD_STEPPER2_MOTOR_CLOCK_SOURCE   clock_mot2
#define BOARD_STEPPER3_MOTOR_CLOCK_SOURCE   clock_mot3
/*stepper motor */
#define MOTOR_FOURAXIS_ENABLE_ON
/*PWM define*/
#define BOARD_STEPPER0PWM                     HPM_PWM0//HPM_PWM3
#define BOARD_STEPPER0_A0H_PWM_OUTPIN         (2U)
#define BOARD_STEPPER0_A0L_PWM_OUTPIN         (3U)
#define BOARD_STEPPER0_B0H_PWM_OUTPIN         (4U)
#define BOARD_STEPPER0_B0L_PWM_OUTPIN         (5U)
#define BOARD_STEPPER0PWM_TRGM                HPM_TRGM0//HPM_TRGM3
#define BOARD_STEPPER0APP_PWM_IRQ             IRQn_PWM0//IRQn_PWM3
#define BOARD_STEPPER0PWM_CMP_INDEX_A0H         (0U)
#define BOARD_STEPPER0PWM_CMP_INDEX_A0L         (1U)
#define BOARD_STEPPER0PWM_CMP_INDEX_A1H         (2U)
#define BOARD_STEPPER0PWM_CMP_INDEX_A1L         (3U)
#define BOARD_STEPPER0PWM_CMP_INDEX_B0H         (4U)
#define BOARD_STEPPER0PWM_CMP_INDEX_B0L         (5U)
#define BOARD_STEPPER0PWM_CMP_INDEX_B1H         (6U)
#define BOARD_STEPPER0PWM_CMP_INDEX_B1L         (7U)

/*PWM define*/
#define BOARD_STEPPER1PWM                     HPM_PWM1//HPM_PWM3//HPM_PWM1
#define BOARD_STEPPER1_A0H_PWM_OUTPIN         (2U)//(5U)
#define BOARD_STEPPER1_A0L_PWM_OUTPIN         (3U)//(3U)
#define BOARD_STEPPER1_B0H_PWM_OUTPIN         (4U)//(4U)
#define BOARD_STEPPER1_B0L_PWM_OUTPIN         (5U)//(2U)
#define BOARD_STEPPER1PWM_TRGM                HPM_TRGM1//HPM_TRGM3//HPM_TRGM1
#define BOARD_STEPPER1APP_PWM_IRQ             IRQn_PWM1//IRQn_PWM3//IRQn_PWM1
#define BOARD_STEPPER1PWM_CMP_INDEX_A0H         (0U)
#define BOARD_STEPPER1PWM_CMP_INDEX_A0L         (1U)
#define BOARD_STEPPER1PWM_CMP_INDEX_A1H         (2U)
#define BOARD_STEPPER1PWM_CMP_INDEX_A1L         (3U)
#define BOARD_STEPPER1PWM_CMP_INDEX_B0H         (4U)
#define BOARD_STEPPER1PWM_CMP_INDEX_B0L         (5U)
#define BOARD_STEPPER1PWM_CMP_INDEX_B1H         (6U)
#define BOARD_STEPPER1PWM_CMP_INDEX_B1L         (7U)

/*PWM define*/
#define BOARD_STEPPER2PWM                     HPM_PWM2//HPM_PWM3//HPM_PWM1
#define BOARD_STEPPER2_A0H_PWM_OUTPIN         (2U)//(5U)
#define BOARD_STEPPER2_A0L_PWM_OUTPIN         (3U)//(3U)
#define BOARD_STEPPER2_B0H_PWM_OUTPIN         (4U)//(4U)
#define BOARD_STEPPER2_B0L_PWM_OUTPIN         (5U)//(2U)
#define BOARD_STEPPER2PWM_TRGM                HPM_TRGM2//HPM_TRGM3//HPM_TRGM1
#define BOARD_STEPPER2APP_PWM_IRQ             IRQn_PWM2//IRQn_PWM3//IRQn_PWM1
#define BOARD_STEPPER2PWM_CMP_INDEX_A0H         (0U)
#define BOARD_STEPPER2PWM_CMP_INDEX_A0L         (1U)
#define BOARD_STEPPER2PWM_CMP_INDEX_A1H         (2U)
#define BOARD_STEPPER2PWM_CMP_INDEX_A1L         (3U)
#define BOARD_STEPPER2PWM_CMP_INDEX_B0H         (4U)
#define BOARD_STEPPER2PWM_CMP_INDEX_B0L         (5U)
#define BOARD_STEPPER2PWM_CMP_INDEX_B1H         (6U)
#define BOARD_STEPPER2PWM_CMP_INDEX_B1L         (7U)

/*PWM define*/
#define BOARD_STEPPER3PWM                     HPM_PWM3//HPM_PWM3//HPM_PWM1
#define BOARD_STEPPER3_A0H_PWM_OUTPIN         (2U)//(5U)
#define BOARD_STEPPER3_A0L_PWM_OUTPIN         (3U)//(3U)
#define BOARD_STEPPER3_B0H_PWM_OUTPIN         (4U)//(4U)
#define BOARD_STEPPER3_B0L_PWM_OUTPIN         (5U)//(2U)
#define BOARD_STEPPER3PWM_TRGM                HPM_TRGM3//HPM_TRGM3//HPM_TRGM1
#define BOARD_STEPPER3APP_PWM_IRQ             IRQn_PWM3//IRQn_PWM3//IRQn_PWM1
#define BOARD_STEPPER3PWM_CMP_INDEX_A0H         (0U)
#define BOARD_STEPPER3PWM_CMP_INDEX_A0L         (1U)
#define BOARD_STEPPER3PWM_CMP_INDEX_A1H         (2U)
#define BOARD_STEPPER3PWM_CMP_INDEX_A1L         (3U)
#define BOARD_STEPPER3PWM_CMP_INDEX_B0H         (4U)
#define BOARD_STEPPER3PWM_CMP_INDEX_B0L         (5U)
#define BOARD_STEPPER3PWM_CMP_INDEX_B1H         (6U)
#define BOARD_STEPPER3PWM_CMP_INDEX_B1L         (7U)

/*adc*/
#define BOARD_STEPPER0_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_STEPPER0_ADC_A_BASE                  HPM_ADC1
#define BOARD_STEPPER0_ADC_B_BASE                  HPM_ADC0
#define BOARD_STEPPER0_ADC_TRIG_FLAG               adc16_event_trig_complete

#define BOARD_STEPPER0_ADC_CH_A                    (13U)//four axis
#define BOARD_STEPPER0_ADC_CH_B                    (4U)
#define BOARD_STEPPER0_ADC_IRQn                    IRQn_ADC1//IRQn_PWM0//
#define BOARD_STEPPER0_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_STEPPER0_ADC_TRG                     ADC16_CONFIG_TRG0A//ADC12_CONFIG_TRG3A
#define BOARD_STEPPER0_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_STEPPER0_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_STEPPER0_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_STEPPER0_TRIGMUX_IN_NUM              HPM_TRGM0_INPUT_SRC_PWM0_CH8REF//HPM_TRGM3_INPUT_SRC_PWM3_CH8REF
#define BOARD_STEPPER0_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A

#define BOARD_STEPPER1_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_STEPPER1_ADC_A_BASE                  HPM_ADC1
#define BOARD_STEPPER1_ADC_B_BASE                  HPM_ADC0
#define BOARD_STEPPER1_ADC_TRIG_FLAG               adc16_event_trig_complete

#define BOARD_STEPPER1_ADC_CH_A                     (15U)//(14U)//(17U)
#define BOARD_STEPPER1_ADC_CH_B                    (5U)//(14U)//(9U)//(8U)
#define BOARD_STEPPER1_ADC_IRQn                    IRQn_PWM1//IRQn_ADC1
#define BOARD_STEPPER1_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_STEPPER1_ADC_TRG                     ADC16_CONFIG_TRG1A//ADC12_CONFIG_TRG3A//ADC12_CONFIG_TRG1A
#define BOARD_STEPPER1_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_STEPPER1_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_STEPPER1_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_STEPPER1_TRIGMUX_IN_NUM              HPM_TRGM1_INPUT_SRC_PWM1_CH8REF//HPM_TRGM3_INPUT_SRC_PWM3_CH8REF//HPM_TRGM1_INPUT_SRC_PWM1_CH8REF
#define BOARD_STEPPER1_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A

#define BOARD_STEPPER2_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_STEPPER2_ADC_A_BASE                  HPM_ADC1
#define BOARD_STEPPER2_ADC_B_BASE                  HPM_ADC0
#define BOARD_STEPPER2_ADC_TRIG_FLAG               adc16_event_trig_complete

#ifdef MOTOR_FOURAXIS_ENABLE_ON
#define BOARD_STEPPER2_ADC_CH_A                     (6U)//four axis
#define BOARD_STEPPER2_ADC_CH_B                    (6U)
#else
#define BOARD_STEPPER2_ADC_CH_A                    (7U)//single axis
#define BOARD_STEPPER2_ADC_CH_B                    (10U)
#endif
#define BOARD_STEPPER2_ADC_IRQn                    IRQn_ADC1//IRQn_PWM2//
#define BOARD_STEPPER2_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_STEPPER2_ADC_TRG                     ADC16_CONFIG_TRG2A//ADC12_CONFIG_TRG3A//ADC12_CONFIG_TRG1A
#define BOARD_STEPPER2_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_STEPPER2_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_STEPPER2_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_STEPPER2_TRIGMUX_IN_NUM              HPM_TRGM2_INPUT_SRC_PWM2_CH8REF//HPM_TRGM3_INPUT_SRC_PWM3_CH8REF//HPM_TRGM1_INPUT_SRC_PWM1_CH8REF
#define BOARD_STEPPER2_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A

#define BOARD_STEPPER3_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_STEPPER3_ADC_A_BASE                  HPM_ADC1
#define BOARD_STEPPER3_ADC_B_BASE                  HPM_ADC0
#define BOARD_STEPPER3_ADC_TRIG_FLAG               adc16_event_trig_complete

#define BOARD_STEPPER3_ADC_CH_A                    (7U)//(8U)//(14U)//(17U)
#define BOARD_STEPPER3_ADC_CH_B                    (7U)//(9U)//(14U)//(9U)//(8U)
#define BOARD_STEPPER3_ADC_IRQn                    IRQn_PWM3//IRQn_ADC1
#define BOARD_STEPPER3_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_STEPPER3_ADC_TRG                     ADC16_CONFIG_TRG3A//ADC12_CONFIG_TRG3A//ADC12_CONFIG_TRG1A
#define BOARD_STEPPER3_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_STEPPER3_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_STEPPER3_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_STEPPER3_TRIGMUX_IN_NUM              HPM_TRGM3_INPUT_SRC_PWM3_CH8REF//HPM_TRGM3_INPUT_SRC_PWM3_CH8REF//HPM_TRGM1_INPUT_SRC_PWM1_CH8REF
#define BOARD_STEPPER3_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A

#define MOTOR_STEPPER_PWM_SYNC                     HPM_SYNT

#define BOARD_STEPPER3_TRIGMUX_IN_NUM_TMR          HPM_TRGM3_INPUT_SRC_GPTMR3_OUT2
#define BOARD_STEPPER3_TMR_FOR_ADC                 HPM_GPTMR3
//#define BOARD_STEPPER3_TMR_FOR_ADC_CH              2
#define BOARD_STEPPER3_TMR_FOR_PWM_CH0              0
#define BOARD_STEPPER3_TMR_FOR_PWM_CH1              1
#define BOARD_STEPPER3_TMR_FOR_PWM_CH2              2
#define BOARD_STEPPER3_TMR_FOR_PWM_CH3              3
#define BOARD_TMR_ADC_IRQ                          IRQn_GPTMR3
#define BOARD_STEPPER3_ADC_IRQn                    IRQn_ADC1
#define BOARD_STEPPER3_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_STEPPER3_PWM_TRIG_TMR_CMP_INDEX      (10U)
#define BOARD_TMR_FOR_PWM_RELOAD                    (4000U)
#define BOARD_STEPPER3_PWM_TRIG_CMP_FOR_TMR_INDEX          (10U)
#define BOARD_STEPPER3_PWM_TRIG_CMP_FOR_TMR_INDEX_CURRENTLOOP          (11U)
#define BOARD_STEPPER3_PWM_TRIG_CMP_FOR_TMR_INDEX_RQN          (12U)
#define BOARD_STEPPER3_TMR_CMP                       (0U)
/*BLDC pwm*/

/*PWM define*/
#define BOARD_BLDCPWM                     HPM_PWM0
#define BOARD_BLDC_UH_PWM_OUTPIN         (0U)
#define BOARD_BLDC_UL_PWM_OUTPIN         (1U)
#define BOARD_BLDC_VH_PWM_OUTPIN         (2U)
#define BOARD_BLDC_VL_PWM_OUTPIN         (3U)
#define BOARD_BLDC_WH_PWM_OUTPIN         (4U)
#define BOARD_BLDC_WL_PWM_OUTPIN         (5U)
#define BOARD_BLDCPWM_TRGM                HPM_TRGM0
#define BOARD_BLDCAPP_PWM_IRQ             IRQn_PWM0
#define BOARD_BLDCPWM_CMP_INDEX_0         (0U)
#define BOARD_BLDCPWM_CMP_INDEX_1         (1U)
#define BOARD_BLDCPWM_CMP_INDEX_2         (2U)
#define BOARD_BLDCPWM_CMP_INDEX_3         (3U)
#define BOARD_BLDCPWM_CMP_INDEX_4         (4U)
#define BOARD_BLDCPWM_CMP_INDEX_5         (5U)
#define BOARD_BLDCPWM_CMP_TRIG_CMP        (15U)

/*HALL define*/

#define BOARD_BLDC_HALL_BASE                 HPM_HALL0
#define BOARD_BLDC_HALL_TRGM                 HPM_TRGM0
#define BOARD_BLDC_HALL_IRQ                  IRQn_HALL0
#define BOARD_BLDC_HALL_TRGM_HALL_U_SRC      HPM_TRGM0_INPUT_SRC_TRGM0_P8
#define BOARD_BLDC_HALL_TRGM_HALL_V_SRC      HPM_TRGM0_INPUT_SRC_TRGM0_P7
#define BOARD_BLDC_HALL_TRGM_HALL_W_SRC      HPM_TRGM0_INPUT_SRC_TRGM0_P6
#define BOARD_BLDC_HALL_MOTOR_PHASE_COUNT_PER_REV        (1000U)



/*QEI*/

#define BOARD_BLDC_QEI_BASE              HPM_QEI0
#define BOARD_BLDC_QEI_IRQ               IRQn_QEI0
#define BOARD_BLDC_QEI_TRGM              HPM_TRGM0
#define BOARD_BLDC_QEI_TRGM_QEI_A_SRC    HPM_TRGM0_INPUT_SRC_TRGM0_P6//0
#define BOARD_BLDC_QEI_TRGM_QEI_B_SRC    HPM_TRGM0_INPUT_SRC_TRGM0_P7//1
#define BOARD_BLDC_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_BLDC_QEI_CLOCK_SOURCE      clock_mot0
#define BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/*Timer define*/

#define BOARD_TMR_1MS                       HPM_GPTMR2
#define BOARD_TMR_1MS_CH                        0
#define BOARD_TMR_1MS_CMP                       0
#define BOARD_TMR_1MS_IRQ                       IRQn_GPTMR2
#define BOARD_TMR_1MS_RELOAD                    (100000U)//2000//

#define BOARD_BLDC_TMR_1MS                       BOARD_TMR_1MS
#define BOARD_BLDC_TMR_CH                        BOARD_TMR_1MS_CH
#define BOARD_BLDC_TMR_CMP                       BOARD_TMR_1MS_CMP
#define BOARD_BLDC_TMR_IRQ                       BOARD_TMR_1MS_IRQ
#define BOARD_BLDC_TMR_RELOAD                    BOARD_TMR_1MS_RELOAD
/*adc*/
#define BOARD_BLDC_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_BLDC_ADC_U_BASE                  HPM_ADC1
#define BOARD_BLDC_ADC_V_BASE                  HPM_ADC0
#define BOARD_BLDC_ADC_W_BASE                  HPM_ADC2
#define BOARD_BLDC_ADC_TRIG_FLAG               adc16_event_trig_complete

#define BOARD_BLDC_ADC_CH_U                    (13U)
#define BOARD_BLDC_ADC_CH_V                    (4U)
#define BOARD_BLDC_ADC_CH_W                    (13U)
#define BOARD_BLDC_ADC_IRQn                    IRQn_ADC1
#define BOARD_BLDC_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_BLDC_ADC_TRG                    ADC16_CONFIG_TRG0A
#define BOARD_BLDC_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_BLDC_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_BLDC_TRIGMUX_IN_NUM              HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_BLDC_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
//#define BOARD_BLDC_ADC_MODULE                  ADCX_MODULE_ADC12
//#define BOARD_BLDC_ADC_U_BASE                  HPM_ADC0
//#define BOARD_BLDC_ADC_V_BASE                  HPM_ADC1
//#define BOARD_BLDC_ADC_W_BASE                  HPM_ADC2
//#define BOARD_BLDC_ADC_TRIG_FLAG               adc12_event_trig_complete

//#define BOARD_BLDC_ADC_CH_U                    (11U)
//#define BOARD_BLDC_ADC_CH_V                    (0U)
//#define BOARD_BLDC_ADC_CH_W                    (13U)
//#define BOARD_BLDC_ADC_IRQn                    IRQn_ADC0
//#define BOARD_BLDC_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
//#define BOARD_BLDC_ADC_TRG                    ADC12_CONFIG_TRG2A
//#define BOARD_BLDC_ADC_PREEMPT_TRIG_LEN        (1U)
//#define BOARD_BLDC_PWM_TRIG_CMP_INDEX          (8U)
//#define BOARD_BLDC_TRIGMUX_IN_NUM              HPM_TRGM2_INPUT_SRC_PWM2_CH8REF
//#define BOARD_BLDC_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
//#define BOARD_BLDC_ADC_IRQn                    IRQn_ADC0

/*PMSM0 adc*/
#define BOARD_PMSM0_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM0_ADC_U_BASE                  HPM_ADC1//HPM_ADC0//
#define BOARD_PMSM0_ADC_V_BASE                  HPM_ADC0//HPM_ADC1//
#define BOARD_PMSM0_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM0_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM0_ADC_CH_U                    (13U)//(15U)//(11U)//
#define BOARD_PMSM0_ADC_CH_V                    (4U)//(5U)//(3U)//
#define BOARD_PMSM0_ADC_CH_W                    (13U)//(15U)//(7U)//
#define BOARD_PMSM0_ADC_IRQn                    IRQn_ADC1
#define BOARD_PMSM0_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM0_ADC_TRG                    ADC16_CONFIG_TRG0A
#define BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM0_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM0_TRIGMUX_IN_NUM              HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_PMSM0_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
//#define BOARD_PMSM0_ADC_IRQn                    IRQn_ADC1
/*PMSM0 PWM*/
#define BOARD_PMSM0PWM                     HPM_PWM0//HPM_PWM0
#define BOARD_PMSM0_UH_PWM_OUTPIN         (0U)
#define BOARD_PMSM0_UL_PWM_OUTPIN         (1U)
#define BOARD_PMSM0_VH_PWM_OUTPIN         (2U)
#define BOARD_PMSM0_VL_PWM_OUTPIN         (3U)
#define BOARD_PMSM0_WH_PWM_OUTPIN         (4U)
#define BOARD_PMSM0_WL_PWM_OUTPIN         (5U)
#define BOARD_PMSM0PWM_TRGM                HPM_TRGM0//HPM_TRGM0
#define BOARD_PMSM0APP_PWM_IRQ             IRQn_PWM0//IRQn_PWM0
#define BOARD_PMSM0PWM_CMP_INDEX_0         (0U)
#define BOARD_PMSM0PWM_CMP_INDEX_1         (1U)
#define BOARD_PMSM0PWM_CMP_INDEX_2         (2U)
#define BOARD_PMSM0PWM_CMP_INDEX_3         (3U)
#define BOARD_PMSM0PWM_CMP_INDEX_4         (4U)
#define BOARD_PMSM0PWM_CMP_INDEX_5         (5U)
#define BOARD_BLDCPWM_CMP_TRIG_CMP        (15U)
/*PMSM0 QEI*/
#define BOARD_PMSM0_QEI_BASE              HPM_QEI0//HPM_QEI0
#define BOARD_PMSM0_QEI_IRQ               IRQn_QEI0
#define BOARD_PMSM0_QEI_TRGM              HPM_TRGM0
#define BOARD_PMSM0_QEI_TRGM_QEI_A_SRC    HPM_TRGM0_INPUT_SRC_TRGM0_P0
#define BOARD_PMSM0_QEI_TRGM_QEI_B_SRC    HPM_TRGM0_INPUT_SRC_TRGM0_P1
#define BOARD_PMSM0_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM0_QEI_CLOCK_SOURCE      clock_mot0
#define BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/*PMSM1 adc*/
#define BOARD_PMSM1_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM1_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM1_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM1_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM1_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM1_ADC_CH_U                    (15U)
#define BOARD_PMSM1_ADC_CH_V                    (5U)
#define BOARD_PMSM1_ADC_CH_W                    (15U)
#define BOARD_PMSM1_ADC_IRQn                    IRQn_ADC1
#define BOARD_PMSM1_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM1_ADC_TRG                    ADC16_CONFIG_TRG1A
#define BOARD_PMSM1_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM1_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM1_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM1_TRIGMUX_IN_NUM              HPM_TRGM1_INPUT_SRC_PWM1_CH8REF
#define BOARD_PMSM1_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM1_ADC_IRQn                    IRQn_ADC1
/*PMSM1 PWM*/
#define BOARD_PMSM1PWM                     HPM_PWM1
#define BOARD_PMSM1_UH_PWM_OUTPIN         (0U)
#define BOARD_PMSM1_UL_PWM_OUTPIN         (1U)
#define BOARD_PMSM1_VH_PWM_OUTPIN         (2U)
#define BOARD_PMSM1_VL_PWM_OUTPIN         (3U)
#define BOARD_PMSM1_WH_PWM_OUTPIN         (4U)
#define BOARD_PMSM1_WL_PWM_OUTPIN         (5U)
#define BOARD_PMSM1PWM_TRGM                HPM_TRGM1
#define BOARD_PMSM1APP_PWM_IRQ             IRQn_PWM1
#define BOARD_PMSM1PWM_CMP_INDEX_0         (0U)
#define BOARD_PMSM1PWM_CMP_INDEX_1         (1U)
#define BOARD_PMSM1PWM_CMP_INDEX_2         (2U)
#define BOARD_PMSM1PWM_CMP_INDEX_3         (3U)
#define BOARD_PMSM1PWM_CMP_INDEX_4         (4U)
#define BOARD_PMSM1PWM_CMP_INDEX_5         (5U)
/*PMSM1 QEI*/
#define BOARD_PMSM1_QEI_BASE              HPM_QEI1
#define BOARD_PMSM1_QEI_IRQ               IRQn_QEI1
#define BOARD_PMSM1_QEI_TRGM              HPM_TRGM1
#define BOARD_PMSM1_QEI_TRGM_QEI_A_SRC    HPM_TRGM1_INPUT_SRC_TRGM1_P6
#define BOARD_PMSM1_QEI_TRGM_QEI_B_SRC    HPM_TRGM1_INPUT_SRC_TRGM1_P7
#define BOARD_PMSM1_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM1_QEI_CLOCK_SOURCE      clock_mot1
#define BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/*PMSM2 adc*/
#define BOARD_PMSM2_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM2_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM2_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM2_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM2_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM2_ADC_CH_U                    (6U)
#define BOARD_PMSM2_ADC_CH_V                    (6U)
#define BOARD_PMSM2_ADC_CH_W                    (6U)
#define BOARD_PMSM2_ADC_IRQn                    IRQn_ADC0
#define BOARD_PMSM2_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM2_ADC_TRG                    ADC16_CONFIG_TRG2A
#define BOARD_PMSM2_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM2_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM2_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM2_TRIGMUX_IN_NUM              HPM_TRGM2_INPUT_SRC_PWM2_CH8REF
#define BOARD_PMSM2_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM2_ADC_IRQn                    IRQn_ADC0
/*PMSM2 PWM*/
#define BOARD_PMSM2PWM                     HPM_PWM2
#define BOARD_PMSM2_UH_PWM_OUTPIN         (0U)
#define BOARD_PMSM2_UL_PWM_OUTPIN         (1U)
#define BOARD_PMSM2_VH_PWM_OUTPIN         (2U)
#define BOARD_PMSM2_VL_PWM_OUTPIN         (3U)
#define BOARD_PMSM2_WH_PWM_OUTPIN         (4U)
#define BOARD_PMSM2_WL_PWM_OUTPIN         (5U)
#define BOARD_PMSM2PWM_TRGM                HPM_TRGM2
#define BOARD_PMSM2APP_PWM_IRQ             IRQn_PWM2
#define BOARD_PMSM2PWM_CMP_INDEX_0         (0U)
#define BOARD_PMSM2PWM_CMP_INDEX_1         (1U)
#define BOARD_PMSM2PWM_CMP_INDEX_2         (2U)
#define BOARD_PMSM2PWM_CMP_INDEX_3         (3U)
#define BOARD_PMSM2PWM_CMP_INDEX_4         (4U)
#define BOARD_PMSM2PWM_CMP_INDEX_5         (5U)
/*PMSM2 QEI*/
#define BOARD_PMSM2_QEI_BASE              HPM_QEI2
#define BOARD_PMSM2_QEI_IRQ               IRQn_QEI2
#define BOARD_PMSM2_QEI_TRGM              HPM_TRGM2
#define BOARD_PMSM2_QEI_TRGM_QEI_A_SRC    HPM_TRGM2_INPUT_SRC_TRGM2_P6
#define BOARD_PMSM2_QEI_TRGM_QEI_B_SRC    HPM_TRGM2_INPUT_SRC_TRGM2_P7
#define BOARD_PMSM2_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM2_QEI_CLOCK_SOURCE      clock_mot2
#define BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/*PMSM3 adc*/
#define BOARD_PMSM3_ADC_MODULE                  ADCX_MODULE_ADC16
#define BOARD_PMSM3_ADC_U_BASE                  HPM_ADC1
#define BOARD_PMSM3_ADC_V_BASE                  HPM_ADC0
#define BOARD_PMSM3_ADC_W_BASE                  HPM_ADC2
#define BOARD_PMSM3_ADC_TRIG_FLAG               adc16_event_trig_complete
#define BOARD_PMSM3_ADC_CH_U                    (7U)
#define BOARD_PMSM3_ADC_CH_V                    (7U)
#define BOARD_PMSM3_ADC_CH_W                    (7U)
#define BOARD_PMSM3_ADC_IRQn                    IRQn_ADC0
#define BOARD_PMSM3_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_PMSM3_ADC_TRG                    ADC16_CONFIG_TRG3A
#define BOARD_PMSM3_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_PMSM3_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_PMSM3_PWM_TRIG_CMP_INDEX_CURRENTLOOP          (9U)
#define BOARD_PMSM3_TRIGMUX_IN_NUM              HPM_TRGM3_INPUT_SRC_PWM3_CH8REF
#define BOARD_PMSM3_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A
#define BOARD_PMSM3_ADC_IRQn                    IRQn_ADC0
/*PMSM3 PWM*/
#define BOARD_PMSM3PWM                     HPM_PWM3
#define BOARD_PMSM3_UH_PWM_OUTPIN         (0U)
#define BOARD_PMSM3_UL_PWM_OUTPIN         (1U)
#define BOARD_PMSM3_VH_PWM_OUTPIN         (2U)
#define BOARD_PMSM3_VL_PWM_OUTPIN         (3U)
#define BOARD_PMSM3_WH_PWM_OUTPIN         (4U)
#define BOARD_PMSM3_WL_PWM_OUTPIN         (5U)
#define BOARD_PMSM3PWM_TRGM                HPM_TRGM3
#define BOARD_PMSM3APP_PWM_IRQ             IRQn_PWM3
#define BOARD_PMSM3PWM_CMP_INDEX_0         (0U)
#define BOARD_PMSM3PWM_CMP_INDEX_1         (1U)
#define BOARD_PMSM3PWM_CMP_INDEX_2         (2U)
#define BOARD_PMSM3PWM_CMP_INDEX_3         (3U)
#define BOARD_PMSM3PWM_CMP_INDEX_4         (4U)
#define BOARD_PMSM3PWM_CMP_INDEX_5         (5U)
/*PMSM3 QEI*/
#define BOARD_PMSM3_QEI_BASE              HPM_QEI3
#define BOARD_PMSM3_QEI_IRQ               IRQn_QEI3
#define BOARD_PMSM3_QEI_TRGM              HPM_TRGM3
#define BOARD_PMSM3_QEI_TRGM_QEI_A_SRC    HPM_TRGM3_INPUT_SRC_TRGM3_P1
#define BOARD_PMSM3_QEI_TRGM_QEI_B_SRC    HPM_TRGM3_INPUT_SRC_TRGM3_P2
#define BOARD_PMSM3_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_PMSM3_QEI_CLOCK_SOURCE      clock_mot3
#define BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)
/*PLA*/
#define BOARD_PLA_COUNTER   HPM_PLA0
#define BOARD_PLA_PWM_BASE  HPM_PWM0
#define BOARD_PLA_PWM_CLOCK_NAME clock_mot0
#define BOARD_PLA_TRGM                HPM_TRGM0
#define BOARD_PLA_PWM_TRG                       (HPM_TRGM0_INPUT_SRC_PWM0_CH8REF)
#define BOARD_PLA_IN_TRG_NUM                    (TRGM_TRGOCFG_PLA_IN0)
#define BOARD_PLA_OUT_TRG                       (HPM_TRGM0_INPUT_SRC_PLA0_OUT0)
#define BOARD_PLA_IO_TRG_NUM                    (TRGM_TRGOCFG_TRGM_OUT5)
#define BOARD_PLA_PWM_CMP                       (8U)
#define BOARD_PLA_PWM_CHN                       (8U)

/* APP PWM */
#define BOARD_APP_PWM HPM_PWM0
#define BOARD_APP_PWM_CLOCK_NAME clock_mot0
#define BOARD_APP_PWM_OUT1 0
#define BOARD_APP_PWM_OUT2 1
#define BOARD_APP_TRGM HPM_TRGM0
#define BOARD_APP_PWM_IRQ IRQn_PWM0

/* APP HRPWM */
#define BOARD_APP_HRPWM HPM_PWM1
#define BOARD_APP_HRPWM_CLOCK_NAME clock_mot1
#define BOARD_APP_HRPWM_OUT1 0
#define BOARD_APP_HRPWM_OUT2 2
#define BOARD_APP_HRPWM_TRGM HPM_TRGM1

#define BOARD_CPU_FREQ (480000000UL)

/* LED */
#define BOARD_R_GPIO_CTRL HPM_GPIO0
#define BOARD_R_GPIO_INDEX GPIO_DI_GPIOA
#define BOARD_R_GPIO_PIN 27
#define BOARD_G_GPIO_CTRL HPM_GPIO0
#define BOARD_G_GPIO_INDEX GPIO_DI_GPIOB
#define BOARD_G_GPIO_PIN 1
#define BOARD_B_GPIO_CTRL HPM_GPIO0
#define BOARD_B_GPIO_INDEX GPIO_DI_GPIOB
#define BOARD_B_GPIO_PIN 19

/* RGB LED Section */
#define BOARD_RED_PWM_IRQ IRQn_PWM3
#define BOARD_RED_PWM HPM_PWM3
#define BOARD_RED_PWM_OUT 7
#define BOARD_RED_PWM_CMP 8
#define BOARD_RED_PWM_CMP_INITIAL_ZERO true
#define BOARD_RED_PWM_CLOCK_NAME clock_mot3

#define BOARD_GREEN_PWM_IRQ IRQn_PWM1
#define BOARD_GREEN_PWM HPM_PWM1
#define BOARD_GREEN_PWM_OUT 1
#define BOARD_GREEN_PWM_CMP 8
#define BOARD_GREEN_PWM_CMP_INITIAL_ZERO true
#define BOARD_GREEN_PWM_CLOCK_NAME clock_mot1

#define BOARD_BLUE_PWM_IRQ IRQn_PWM0
#define BOARD_BLUE_PWM HPM_PWM0
#define BOARD_BLUE_PWM_OUT 7
#define BOARD_BLUE_PWM_CMP 8
#define BOARD_BLUE_PWM_CMP_INITIAL_ZERO true
#define BOARD_BLUE_PWM_CLOCK_NAME clock_mot0

#define BOARD_RGB_RED 0
#define BOARD_RGB_GREEN (BOARD_RGB_RED + 1)
#define BOARD_RGB_BLUE  (BOARD_RGB_RED + 2)

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

void board_init(void);
void board_init_console(void);

void board_init_uart(UART_Type *ptr);
void board_init_i2c(I2C_Type *ptr);

void board_init_can(MCAN_Type *ptr);

void board_init_gpio_pins(void);
void board_init_spi_pins(SPI_Type *ptr);
void board_init_spi_pins_with_gpio_as_cs(SPI_Type *ptr);
void board_write_spi_cs(uint32_t pin, uint8_t state);
uint8_t board_get_led_gpio_off_level(void);
uint8_t board_get_led_pwm_off_level(void);
void board_init_led_pins(void);
void board_disable_output_rgb_led(uint8_t color);
void board_enable_output_rgb_led(uint8_t color);
void board_init_rgb_pwm_pins(void);

void board_led_write(uint8_t state);
void board_led_toggle(void);

/* Initialize SoC overall clocks */
void board_init_clock(void);

uint32_t board_init_spi_clock(SPI_Type *ptr);

uint32_t board_init_lin_clock(LIN_Type *ptr);

uint32_t board_init_adc16_clock(ADC16_Type *ptr);

uint32_t board_init_dac_clock(DAC_Type *ptr, bool clk_src_ahb);

void board_init_adc16_pins(void);

void board_init_dac_pins(DAC_Type *ptr);

uint32_t board_init_can_clock(MCAN_Type *ptr);

void board_init_usb_pins(void);
void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level);
uint8_t board_get_usb_id_status(void);


/*
 * @brief Initialize PMP and PMA for but not limited to the following purposes:
 *      -- non-cacheable memory initialization
 */
void board_init_pmp(void);

void board_delay_us(uint32_t us);
void board_delay_ms(uint32_t ms);

void board_timer_create(uint32_t ms, board_timer_cb cb);
void board_ungate_mchtmr_at_lp_mode(void);

/* Initialize the UART clock */
uint32_t board_init_uart_clock(UART_Type *ptr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
