/*
 * Copyright (c) 2021-2025 HPMicro
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
#include "hpm_lcdc_drv.h"
#include "hpm_trgm_drv.h"
#ifdef CONFIG_HPM_PANEL
#include "hpm_panel.h"
#endif
#if !defined(CONFIG_NDEBUG_CONSOLE) || !CONFIG_NDEBUG_CONSOLE
#include "hpm_debug_console.h"
#endif
#if defined(CONFIG_ENET_PHY) && CONFIG_ENET_PHY
#include "hpm_enet_phy.h"
#endif
#if defined(CONFIG_EEPROM_DEVICE_I2C)
#include "eeprom_device.h"
#endif

#define BOARD_NAME          "hpm6754evk2"
#define BOARD_UF2_SIGNATURE (0x0A4D5048UL)

#define SEC_CORE_IMG_START ILM_LOCAL_BASE

#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE HPM_CORE0
#endif

/* uart section */
#ifndef BOARD_APP_UART_BASE
#define BOARD_APP_UART_BASE       HPM_UART13
#define BOARD_APP_UART_IRQ        IRQn_UART13
#define BOARD_APP_UART_BAUDRATE   (115200UL)
#define BOARD_APP_UART_CLK_NAME   clock_uart13
#define BOARD_APP_UART_RX_DMA_REQ HPM_DMA_SRC_UART13_RX
#define BOARD_APP_UART_TX_DMA_REQ HPM_DMA_SRC_UART13_TX
#endif
#define BOARD_APP_UART_BREAK_SIGNAL_PIN IOC_PAD_PE31

/* uart rx idle demo section */
#define BOARD_UART_IDLE            BOARD_APP_UART_BASE
#define BOARD_UART_IDLE_IRQ        BOARD_APP_UART_IRQ
#define BOARD_UART_IDLE_CLK_NAME   BOARD_APP_UART_CLK_NAME
#define BOARD_UART_IDLE_TX_DMA_SRC BOARD_APP_UART_TX_DMA_REQ
#define BOARD_UART_IDLE_DMA_SRC    BOARD_APP_UART_RX_DMA_REQ

#define BOARD_UART_IDLE_TRGM                    HPM_TRGM2
#define BOARD_UART_IDLE_TRGM_PIN                IOC_PAD_PD19
#define BOARD_UART_IDLE_TRGM_INPUT_SRC          HPM_TRGM2_INPUT_SRC_TRGM2_P9
#define BOARD_UART_IDLE_TRGM_OUTPUT_GPTMR_IN    HPM_TRGM2_OUTPUT_SRC_GPTMR4_IN2
#define BOARD_UART_IDLE_TRGM_OUTPUT_GPTMR_SYNCI HPM_TRGM2_OUTPUT_SRC_GPTMR4_SYNCI

#define BOARD_UART_IDLE_GPTMR          HPM_GPTMR4
#define BOARD_UART_IDLE_GPTMR_CLK_NAME clock_gptmr4
#define BOARD_UART_IDLE_GPTMR_IRQ      IRQn_GPTMR4
#define BOARD_UART_IDLE_GPTMR_CMP_CH   0
#define BOARD_UART_IDLE_GPTMR_CAP_CH   2

/* uart microros sample section */
#define BOARD_MICROROS_UART_BASE     BOARD_APP_UART_BASE
#define BOARD_MICROROS_UART_IRQ      BOARD_APP_UART_IRQ
#define BOARD_MICROROS_UART_CLK_NAME BOARD_APP_UART_CLK_NAME

/* rtthread-nano finsh section */
#define BOARD_RT_CONSOLE_BASE        BOARD_CONSOLE_UART_BASE
#define BOARD_RT_CONSOLE_CLK_NAME    BOARD_CONSOLE_UART_CLK_NAME
#define BOARD_RT_CONSOLE_IRQ         BOARD_CONSOLE_UART_IRQ

/* usb cdc acm uart section */
#define BOARD_USB_CDC_ACM_UART            BOARD_APP_UART_BASE
#define BOARD_USB_CDC_ACM_UART_CLK_NAME   BOARD_APP_UART_CLK_NAME
#define BOARD_USB_CDC_ACM_UART_TX_DMA_SRC BOARD_APP_UART_TX_DMA_REQ
#define BOARD_USB_CDC_ACM_UART_RX_DMA_SRC BOARD_APP_UART_RX_DMA_REQ

/* modbus sample section */
#define BOARD_MODBUS_UART_BASE       BOARD_APP_UART_BASE
#define BOARD_MODBUS_UART_CLK_NAME   BOARD_APP_UART_CLK_NAME
#define BOARD_MODBUS_UART_RX_DMA_REQ BOARD_APP_UART_RX_DMA_REQ
#define BOARD_MODBUS_UART_TX_DMA_REQ BOARD_APP_UART_TX_DMA_REQ

/* uart lin sample section */
#define BOARD_UART_LIN          BOARD_APP_UART_BASE
#define BOARD_UART_LIN_IRQ      BOARD_APP_UART_IRQ
#define BOARD_UART_LIN_CLK_NAME BOARD_APP_UART_CLK_NAME
#define BOARD_UART_LIN_TX_PORT  GPIO_DI_GPIOZ
#define BOARD_UART_LIN_TX_PIN   (9U) /* PZ09 should align with used pin in pinmux configuration */

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
#define BOARD_CONSOLE_UART_BASE       HPM_UART13
#define BOARD_CONSOLE_UART_CLK_NAME   clock_uart13
#define BOARD_CONSOLE_UART_IRQ        IRQn_UART13
#define BOARD_CONSOLE_UART_TX_DMA_REQ HPM_DMA_SRC_UART13_TX
#define BOARD_CONSOLE_UART_RX_DMA_REQ HPM_DMA_SRC_UART13_RX
#endif
#endif
#define BOARD_CONSOLE_UART_BAUDRATE (115200UL)
#endif
#endif

/* sdram section */
#define BOARD_SDRAM_ADDRESS          (0x40000000UL)
#define BOARD_SDRAM_SIZE             (32 * SIZE_1MB)
#define BOARD_SDRAM_CS               FEMC_SDRAM_CS0
#define BOARD_SDRAM_PORT_SIZE        FEMC_SDRAM_PORT_SIZE_32_BITS
#define BOARD_SDRAM_COLUMN_ADDR_BITS FEMC_SDRAM_COLUMN_ADDR_9_BITS
#define BOARD_SDRAM_REFRESH_COUNT    (8192UL)
#define BOARD_SDRAM_REFRESH_IN_MS    (64UL)

#define BOARD_FLASH_BASE_ADDRESS (0x80000000UL)
#define BOARD_FLASH_SIZE         (16 << 20)

/* dma section */
#define BOARD_APP_XDMA      HPM_XDMA
#define BOARD_APP_HDMA      HPM_HDMA
#define BOARD_APP_XDMA_IRQ  IRQn_XDMA
#define BOARD_APP_HDMA_IRQ  IRQn_HDMA
#define BOARD_APP_DMAMUX    HPM_DMAMUX
#define TEST_DMA_CONTROLLER HPM_XDMA
#define TEST_DMA_IRQ        IRQn_XDMA

/* gptmr section */
#define BOARD_GPTMR                   HPM_GPTMR4
#define BOARD_GPTMR_IRQ               IRQn_GPTMR4
#define BOARD_GPTMR_CHANNEL           1
#define BOARD_GPTMR_DMA_SRC           HPM_DMA_SRC_GPTMR4_1
#define BOARD_GPTMR_CLK_NAME          clock_gptmr4
#define BOARD_GPTMR_PWM               HPM_GPTMR5
#define BOARD_GPTMR_PWM_DMA_SRC       HPM_DMA_SRC_GPTMR5_2
#define BOARD_GPTMR_PWM_CHANNEL       2
#define BOARD_GPTMR_PWM_CLK_NAME      clock_gptmr5
#define BOARD_GPTMR_PWM_IRQ           IRQn_GPTMR5
#define BOARD_GPTMR_PWM_SYNC          HPM_GPTMR5
#define BOARD_GPTMR_PWM_SYNC_CHANNEL  3
#define BOARD_GPTMR_PWM_SYNC_CLK_NAME clock_gptmr5

#define BOARD_APP_GPIO_INDEX GPIO_DI_GPIOZ
#define BOARD_APP_GPIO_PIN   2
#define BOARD_BUTTON_PRESSED_VALUE 0

/* pinmux section */
#define USING_GPIO0_FOR_GPIOZ
#ifndef USING_GPIO0_FOR_GPIOZ
#define BOARD_APP_GPIO_CTRL HPM_BGPIO
#define BOARD_APP_GPIO_IRQ  IRQn_BGPIO
#else
#define BOARD_APP_GPIO_CTRL HPM_GPIO0
#define BOARD_APP_GPIO_IRQ  IRQn_GPIO0_Z
#endif

/* gpiom section */
#define BOARD_APP_GPIOM_BASE            HPM_GPIOM
#define BOARD_APP_GPIOM_USING_CTRL      HPM_FGPIO
#define BOARD_APP_GPIOM_USING_CTRL_NAME gpiom_core0_fast

/* Flash section */
#define BOARD_APP_XPI_NOR_XPI_BASE     (HPM_XPI0)
#define BOARD_APP_XPI_NOR_CFG_OPT_HDR  (0xfcf90002U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT0 (0x00000005U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT1 (0x00001000U)

/* enet section */
#define BOARD_ENET_COUNT         (2U)
#define BOARD_ENET_PPS           HPM_ENET0
#define BOARD_ENET_PPS_IDX       enet_pps_0
#define BOARD_ENET_PPS_PTP_CLOCK clock_ptp0

#define BOARD_ENET_AUXI_SNAPSHOT           HPM_ENET0
#define BOARD_ENET_AUXI_SNAPSHOT_IDX       enet_ptp_auxi_snapshot_trigger_1
#define BOARD_ENET_AUXI_SNAPSHOT_PTP_CLOCK clock_ptp0

#define BOARD_ENET_RGMII_PHY_ITF        enet_inf_rgmii
#define BOARD_ENET_RGMII_RST_GPIO       HPM_GPIO0
#define BOARD_ENET_RGMII_RST_GPIO_INDEX GPIO_DO_GPIOF
#define BOARD_ENET_RGMII_RST_GPIO_PIN   (0U)
#define BOARD_ENET_RGMII                HPM_ENET0
#define BOARD_ENET_RGMII_TX_DLY         (10U)
#define BOARD_ENET_RGMII_RX_DLY         (5U)
#define BOARD_ENET_RGMII_PTP_CLOCK      (clock_ptp0)
#define BOARD_ENET_RGMII_PPS0_PINOUT    (1)

#define BOARD_ENET_RMII_PHY_ITF        enet_inf_rmii
#define BOARD_ENET_RMII_RST_GPIO       HPM_GPIO0
#define BOARD_ENET_RMII_RST_GPIO_INDEX GPIO_DO_GPIOE
#define BOARD_ENET_RMII_RST_GPIO_PIN   (26U)
#define BOARD_ENET_RMII                HPM_ENET1
#define BOARD_ENET_RMII_INT_REF_CLK    enet_phy_rmii_refclk_dir_in
#define BOARD_ENET_RMII_PTP_CLOCK      (clock_ptp1)
#define BOARD_ENET_RMII_PPS0_PINOUT    (0)

/*
 * timer for board delay
 */
#define BOARD_DELAY_TIMER          (HPM_GPTMR7)
#define BOARD_DELAY_TIMER_CH       0
#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr7)

#define BOARD_CALLBACK_TIMER          (HPM_GPTMR7)
#define BOARD_CALLBACK_TIMER_CH       1
#define BOARD_CALLBACK_TIMER_IRQ      IRQn_GPTMR7
#define BOARD_CALLBACK_TIMER_CLK_NAME (clock_gptmr7)

/* SDXC section */
#define BOARD_APP_SDCARD_SDXC_BASE                 (HPM_SDXC1)
#define BOARD_APP_SDCARD_SDXC_IRQ                  IRQn_SDXC1
#define BOARD_APP_SDCARD_SUPPORT_3V3               (1)
#define BOARD_APP_SDCARD_SUPPORT_1V8               (0)
#define BOARD_APP_SDCARD_SUPPORT_4BIT              (1)
#define BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION    (1)
#define BOARD_APP_SDCARD_SUPPORT_POWER_SWITCH      (1)
#define BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO   (1)
#define BOARD_APP_SDCARD_SUPPORT_VOLTAGE_SWITCH    (0)
#define BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION    (1)
#define BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO (1)
#if defined(BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO) && (BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO == 1)
#define BOARD_APP_SDCARD_CARD_DETECTION_PIN     IOC_PAD_PD15
#define BOARD_APP_SDCARD_CARD_DETECTION_PIN_POL 1 /* PIN value 0 means card is inserted */
#endif
#if defined(BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO) && (BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO == 1)
#define BOARD_APP_SDCARD_POWER_SWITCH_PIN     IOC_PAD_PC20
#define BOARD_APP_SDCARD_POWER_SWITCH_PIN_POL 0 /* PIN value 1 means power is supplied */
#endif

#define BOARD_APP_EMMC_SDXC_BASE               (HPM_SDXC1)
#define BOARD_APP_EMMC_SDXC_IRQ                IRQn_SDXC1
#define BOARD_APP_EMMC_SUPPORT_3V3             (1)
#define BOARD_APP_EMMC_SUPPORT_1V8             (0)
#define BOARD_APP_EMMC_SUPPORT_4BIT            (1)
#define BOARD_APP_EMMC_SUPPORT_POWER_SWITCH    (1)
#define BOARD_APP_EMMC_POWER_SWITCH_USING_GPIO (1)
#define BOARD_APP_EMMC_HOST_USING_IRQ          (0)
#if defined(BOARD_APP_EMMC_POWER_SWITCH_USING_GPIO) && (BOARD_APP_EMMC_POWER_SWITCH_USING_GPIO == 1)
#define BOARD_APP_EMMC_POWER_SWITCH_PIN     IOC_PAD_PC20
#define BOARD_APP_EMMC_POWER_SWITCH_PIN_POL 0 /* PIN value 1 means power is supplied */
#endif

#define BOARD_APP_SDIO_SDXC_BASE                 (HPM_SDXC1)
#define BOARD_APP_SDIO_SDXC_IRQ                  IRQn_SDXC1
#define BOARD_APP_SDIO_SUPPORT_3V3               (0)
#define BOARD_APP_SDIO_SUPPORT_1V8               (1)
#define BOARD_APP_SDIO_SUPPORT_4BIT              (1)
#define BOARD_APP_SDIO_SUPPORT_POWER_SWITCH      (1)
#define BOARD_APP_SDIO_SUPPORT_VOLTAGE_SWITCH    (1)
#define BOARD_APP_SDIO_SUPPORT_CARD_DETECTION    (0)
#define BOARD_APP_SDIO_POWER_SWITCH_USING_GPIO   (1)
#define BOARD_APP_SDIO_VOLTAGE_SWITCH_USING_GPIO (1)
#ifdef BOARD_APP_SDIO_POWER_SWITCH_USING_GPIO
#define BOARD_APP_SDIO_POWER_SWITCH_PIN IOC_PAD_PD10
#endif
#ifdef BOARD_APP_SDIO_VOLTAGE_SWITCH_USING_GPIO
#define BOARD_APP_SDIO_VSEL_PIN IOC_PAD_PD13
#endif

#define BOARD_APP_SDIO_WIFI_OOB_PORT       (HPM_GPIO0)
#define BOARD_APP_SDIO_WIFI_OOB_PIN        (IOC_PAD_PD20)
#define BOARD_APP_SDIO_WIFI_OOB_IRQ        IRQn_GPIO0_D
#define BOARD_APP_SDIO_WIFI_WL_REG_ON_PORT (HPM_GPIO0)
#define BOARD_APP_SDIO_WIFI_WL_REG_ON_PIN  (IOC_PAD_PD16)
#define BOARD_APP_SDIO_WIFI_SDXC_PORT      BOARD_APP_SDIO_SDXC_BASE

#define BOARD_CPU_FREQ (816000000UL)

#define BOARD_APP_DISPLAY_CLOCK clock_display

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

#define BOARD_FREERTOS_TICK_SRC_PWM          HPM_PWM0
#define BOARD_FREERTOS_TICK_SRC_PWM_IRQ      IRQn_PWM0
#define BOARD_FREERTOS_TICK_SRC_PWM_CLK_NAME clock_mot0

#define BOARD_FREERTOS_LOWPOWER_TIMER          HPM_PTMR
#define BOARD_FREERTOS_LOWPOWER_TIMER_CHANNEL  1
#define BOARD_FREERTOS_LOWPOWER_TIMER_IRQ      IRQn_PTMR
#define BOARD_FREERTOS_LOWPOWER_TIMER_CLK_NAME clock_ptmr

/* Threadx Definitions */
#define BOARD_THREADX_TIMER          HPM_GPTMR6
#define BOARD_THREADX_TIMER_CHANNEL  1
#define BOARD_THREADX_TIMER_IRQ      IRQn_GPTMR6
#define BOARD_THREADX_TIMER_CLK_NAME clock_gptmr6

#define BOARD_THREADX_LOWPOWER_TIMER          HPM_PTMR
#define BOARD_THREADX_LOWPOWER_TIMER_CHANNEL  1
#define BOARD_THREADX_LOWPOWER_TIMER_IRQ      IRQn_PTMR
#define BOARD_THREADX_LOWPOWER_TIMER_CLK_NAME clock_ptmr

/* uC/OS-III Definitions */
#define BOARD_UCOS_TIMER          HPM_GPTMR6
#define BOARD_UCOS_TIMER_CHANNEL  1
#define BOARD_UCOS_TIMER_IRQ      IRQn_GPTMR6
#define BOARD_UCOS_TIMER_CLK_NAME clock_gptmr6

/* BGPR */
#define BOARD_BGPR HPM_BGPR

/* usb id pin */
#define BOARD_USB_ID_GPIO_CTRL  HPM_GPIO0
#define BOARD_USB_ID_GPIO_INDEX GPIO_DI_GPIOF
#define BOARD_USB_ID_GPIO_PIN   (10U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);

void board_init(void);
void board_init_console(void);

void board_init_core1(void);

void board_init_uart(UART_Type *ptr);

uint32_t board_init_femc_clock(void);

void board_init_sdram_pins(void);

/* Initialize SoC overall clocks */
void board_init_clock(void);

/* Initialize the UART clock */
uint32_t board_init_uart_clock(UART_Type *ptr);

uint32_t board_sd_configure_clock(SDXC_Type *ptr, uint32_t freq, bool need_inverse);

void board_init_usb(USB_Type *ptr);

void board_init_enet_pps_pins(ENET_Type *ptr);
void board_init_enet_pps_capture_pins(ENET_Type *ptr);
uint8_t board_get_enet_dma_pbl(ENET_Type *ptr);
hpm_stat_t board_reset_enet_phy(ENET_Type *ptr);
hpm_stat_t board_init_enet_pins(ENET_Type *ptr);
hpm_stat_t board_init_enet_rmii_reference_clock(ENET_Type *ptr, bool internal);
hpm_stat_t board_init_enet_rgmii_clock_delay(ENET_Type *ptr);
hpm_stat_t board_init_enet_ptp_clock(ENET_Type *ptr);
hpm_stat_t board_enable_enet_irq(ENET_Type *ptr);
hpm_stat_t board_disable_enet_irq(ENET_Type *ptr);

/*
 * @brief Initialize PMP and PMA for but not limited to the following purposes:
 *      -- non-cacheable memory initialization
 */
void board_init_pmp(void);

void board_delay_ms(uint32_t ms);
void board_delay_us(uint32_t us);

void board_timer_create(uint32_t ms, board_timer_cb cb);

uint32_t board_init_gptmr_clock(GPTMR_Type *ptr);

void board_led_write(uint8_t state);
void board_led_toggle(void);

void board_init_gpio_pins();
void board_init_led_pins();
/*
 * Wrap pinmux initialization.
 */
void init_uart_pins(UART_Type *ptr);
void init_gpio_pins(void);
void init_enet_pins(ENET_Type *ptr);
void init_usb_pins(USB_Type *ptr);
void init_sdxc_cmd_pin(SDXC_Type *ptr, bool open_drain, bool is_1v8);
void init_sdxc_cd_pin(SDXC_Type  *ptr, bool as_gpio);
void init_sdxc_clk_data_pins(SDXC_Type *ptr, uint32_t width, bool is_1v8);
void init_sdxc_pwr_pin(SDXC_Type *ptr, bool as_gpio);
void init_sdxc_vsel_pin(SDXC_Type *ptr, bool as_gpio);
#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
