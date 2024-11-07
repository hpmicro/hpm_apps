# hpm_monitor service

[English](./README.md)

(hpm_monitor_instruction_en)=

## overview

hpm_monitor is an efficient, easy to use, and highly portable service, which is used to view and set global variables in the current device in real time, or to report global variables at high speed (1Khz-1ms). Often used as a monitoring data oscilloscope; Very friendly to motor, power supply and other debugging;

The hpm_monitor service must be used with the HPMicroMonitorStudio tool on the PC. For details about how to use the HPMicroMonitorStudio tool, see the help document. This document does not describe it.  

Operation effect:
![pc_hpm_monitor_1](doc/api/assets/pc_hpm_monitor_1.png)


## peculiarity

- hpm_monitor has high portability and only needs to be adapted to the relevant communication port.
- hpm_monitor is simple to invoke and has only two interfaces. You only need to call init and handle polling, without modifying the original engineering logic.
- The hpm_monitor communication protocol is lightweight and easily extensible.
- Support to actively GET (GET) and SET (SET) global variables.
- Host high speed report (Notify) global variable.

## Usage method

The hpm_monitor service is simple to invoke. You only need to call monitor_init() during initialization, and then query monitor_handle() in the main loop to enable the current tool.

### APPS CMakeLists.txt Enable the current tool
CMakeLists.txt Refer to

``` C

cmake_minimum_required(VERSION 3.13)

# Activate HPMMONITOR
set(CONFIG_A_HPMMONITOR 1)
# Enable UART port
set(CONFIG_MONITOR_INTERFACE "uart")
# Enable USB port
# set(CONFIG_MONITOR_INTERFACE "usb")

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")

elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
# Enable the USB port after use
    set(CONFIG_CHERRYUSB 1)
    set(CONFIG_USB_DEVICE 1)
    set(CONFIG_USB_DEVICE_CDC 1)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

# Defines the enabled port macro definition
if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")
    sdk_compile_definitions("-DCONFIG_UART_CHANNEL=1")
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    sdk_compile_definitions("-DCONFIG_USB_CHANNEL=1")
endif()

```

### monitor_config.h Configuration
monitor_config.h is the configuration header file of the hpm_monitor service, which can refer to core/monitor_kconfig.h by default.

``` C
#define MONITOR_PID                  (0xFFFF)
#define MONITOR_VID                  (0x34B7) /* HPMicro VID */

#define MONITOR_PROFILE_MAXSIZE      (512)

#define MONITOR_RINGBUFFSER_SIZE     (MONITOR_PROFILE_MAXSIZE*2)

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

```

### hpm_monitor service invocation
To enable the hpm_monitor service, you only need to call init in the upper-layer application and poll handle in the main loop.
For reference:

``` C

int main(void)
{
    uint64_t time = 0;
    board_init();
    printf("general debug demo!\r\n");

    //init invoke
    monitor_init();

    while (1)
    {
        //handle polling
        monitor_handle();
    }
    return 0;
}
```

## API

:::{eval-rst}

About software API: `API doc <../../_static/middleware/hpm_monitor/html/index.html>`_ .
:::