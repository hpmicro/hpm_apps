# hpm_monitor

[中文](./README_zh.md)

(hpm_monitor_instruction_en)=

## Introduction

hpm_monitor is a highly efficient, user-friendly, and portable service designed for real-time viewing and configuration of global variables within devices, as well as high-speed reporting of global variables. It is commonly used as a monitoring data oscilloscope and is particularly well-suited for debugging motors, power supplies, and similar equipment.

The hpm_monitor service requires the HPMicro Monitor Studio tool on a PC host computer. For detailed instructions on using the HP MicroMonitor Studio tool, please refer to the tool's help documentation.

Operational Effect:
![pc_hpm_monitor_1](doc/api/assets/pc_hpm_monitor_1.png)

## Features

- **High Portability**: Requires only adaptation to relevant communication ports
- **Simple Invocation**: Only two interfaces needed (init and handle polling), with no modifications to original project logic
- **Lightweight Protocol**: Minimal communication protocol with easy scalability
- **Versatile Operations**: Supports active retrieval (GET) and configuration (SET) of global variables
- **High-Speed Reporting**: Multiple modes for rapid global variable updates
- **Diverse Sampling Modes**: Notify, Stream, and Buffer modes supported
- **Custom Reporting**: Supports manual data reporting via user-defined channels
- **Independent Memory Pool**: Completely isolated from user memory, ensuring no mutual interference
- **Low CPU Usage**: UART/USB transmission uses DMA; Stream/Buffer modes utilize GPTMR + DMA


## Protocol Version

hpm_monitor currently supports the latest V2 protocol.
``` c
#define MONITOR_PROFILE_VERSION (2)
```

## Sampling Mode

hpm_monitor v2.0 offers three sampling modes:

### 1. Notify Mode

- **Sampling Method**: Data sampling performed by the CPU
- **Features**: Supports batch reporting via multipack mechanism; sampling jitter and accuracy affected by CPU load
- **Suitable Scenarios**: Low CPU load scenarios requiring real-time sampling and reporting

### 2. Stream Mode

- **Sampling Method**: Data sampling performed by GPTMR + DMA
- **Features**: High sampling frequency, low jitter, no CPU resource consumption; sampling frequency limited by communication channel bandwidth
- **Resource Usage**: Utilizes DMA and GPTMR resources
- **Suitable Scenarios**: Applications requiring high-frequency, low-jitter real-time sampling
- **Note**: Since sampling is performed by DMA, ensure data cache coherence is maintained.

### 3. Buffer Mode

- **Sampling Method**: Data sampling performed by GPTMR + DMA
- **Features**: Extremely high sampling frequency, minimal jitter, no CPU resource consumption, large buffer per sampling cycle
- **Advantages**: Sampling frequency unrestricted by communication channel bandwidth
- **Resource Usage**: Utilizes DMA and GPTMR resources
- **Suitable Scenarios**: Applications requiring ultra-high-frequency sampling that do not depend on real-time communication
- **Note**: Since sampling is performed by DMA, ensure data cache consistency.

### 4. User-defined channel

- **Sampling Method**: User-initiated manual sampling and reporting
- **Features**: Supports reporting of individual data points or entire data arrays
- **Applicable Scenarios**: Prevents sampling loss due to CPU or DMA constraints, such as adding current values within a motor current loop

## Instructions for Use

The hpm_monitor service is simple to call. To enable the current tool, simply call monitor_init() during initialization and poll monitor_handle() in the main loop.

### 1. APPS CMakeLists.txt Config

``` cmake

cmake_minimum_required(VERSION 3.13)
set(APP_VERSION_STRING "\"1.10.0\"")

# Launch HPMMONITOR
set(CONFIG_A_HPMMONITOR 1)
# Select communication interface: UART or USB or Ethernet
set(CONFIG_MONITOR_INTERFACE "uart")
# set(CONFIG_MONITOR_INTERFACE "usb")
# set(CONFIG_MONITOR_INTERFACE "enet")

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")

elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    # USB Configuration (The following options must be enabled)
    set(CONFIG_CHERRYUSB 1)
    set(CONFIG_USB_DEVICE 1)
    set(CONFIG_USB_DEVICE_CDC 1)
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "enet")
    # ENET Configuration (The following options must be enabled)
    set(CONFIG_LWIP 1)
    set(CONFIG_ENET_PHY 1)
    set(APP_USE_ENET_PORT_COUNT 1)
    #set(APP_USE_ENET_ITF_RGMII 1)
    #set(APP_USE_ENET_ITF_RMII 1)
    #set(APP_USE_ENET_PHY_DP83867 1)
    #set(APP_USE_ENET_PHY_RTL8211 1)
    #set(APP_USE_ENET_PHY_DP83848 1)
    #set(APP_USE_ENET_PHY_RTL8201 1)
    if(NOT DEFINED APP_USE_ENET_PORT_COUNT)
        message(FATAL_ERROR "APP_USE_ENET_PORT_COUNT is undefined!")
    endif()

    if(NOT APP_USE_ENET_PORT_COUNT EQUAL 1)
        message(FATAL_ERROR "This sample supports only one Ethernet port!")
    endif()

    if (APP_USE_ENET_ITF_RGMII AND APP_USE_ENET_ITF_RMII)
        message(FATAL_ERROR "This sample doesn't support more than one Ethernet phy!")
    endif()
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

# Define enabled port macros
if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")
    sdk_compile_definitions("-DCONFIG_UART_CHANNEL=1")
    sdk_compile_definitions("-DCONFIG_USE_CONSOLE_UART=1")
    sdk_compile_definitions("-DCONFIG_MONITOR_DBG_LEVEL=0")
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    sdk_compile_definitions("-DCONFIG_USB_CHANNEL=1")
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "enet")
    sdk_compile_definitions("-DCONFIG_ENET_CHANNEL=1")
    sdk_inc(inc/enet)
endif()

```

### 2. monitor_config.h Config
monitor_config.h is the configuration header file for the hpm_monitor service. By default, it references core/monitor_kconfig.h.

``` C
/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __MONITOR_CONFIG_H
#define __MONITOR_CONFIG_H


#define MONITOR_PID                  (0xFFFF)
#define MONITOR_VID                  (0x34B7) /* HPMicro VID */

#define MONITOR_PROFILE_MAXSIZE      (4096)

/*Monitor memory pool size*/
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

/**UART配置*/
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
/**USB Configuration*/
#if defined(CONFIG_USB_CHANNEL) && CONFIG_USB_CHANNEL

#define CONFIG_USB_POLLING_ENABLE

#include "usb_config.h"
#define MONITOR_USB_BASE             CONFIG_HPM_USBD_BASE
#define MONITOR_USB_BUSID            0
#define MONITOR_USB_IRQ              CONFIG_HPM_USBD_IRQn
#define MONITOR_USB_PRIORITY         2

#endif
/**ENET Configuration*/
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

/**Sampling Data Limit Configuration*/
#define MONITOR_REPORT_MAXCOUNT              (16)
#define MONITOR_STREAM_BUFFER_MAXCOUNT       MONITOR_REPORT_MAXCOUNT
#define MONITOR_CHANNEL_MAXCOUNT             MONITOR_REPORT_MAXCOUNT
#define MONITOR_TRIGGER_MAXCOUNT             (16)
#define MONITOR_DATA_LIST_MAXCOUNT           (20)

/** Sampling GPTMR + DMA Configuration*/
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

#endif //__MONITOR_CONFIG_H

```

### 3.Service Invocation
The hpm_monitor service is enabled. Simply call init to initialize it in the upper-layer application and poll the handle in the main loop to complete the process.
Reference as follows:

``` C
#include "monitor.h"

int main(void)
{
    board_init();
    printf("hpm_monitor v2.0 demo!\r\n");
    
    // Initialize the hpm_monitor service
    monitor_init();
    
    while (1)
    {
        // Polling processing of monitor transactions
        monitor_handle();
        
        // User Business Code...
    }
    
    return 0;
}
```

## Custom Channel Data Reporting

### 1. Define global variables and register them to the monitoring channel.

Use the macro MONITOR_DEFINE_GLOBAL_VAR to define global variables:

``` c
// Parameter Description: name (variable name), channel (channel number), type (data type), frequency (sampling frequency), count (number of data points; 0 indicates the data point represented by the variable name, >0 indicates multiple data points in an array)
MONITOR_DEFINE_GLOBAL_VAR(my_variable, 0, float, 1000, 1);
```

### 2. Add Single Data Upload

``` c
float current_value = 3.14f;
int result = monitor_channel_add_data(0, &current_value);
if (result != 0) {
    // Handling Errors
}
```

### 3. Add array data reporting

``` c
float sensor_data[10] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
int result = monitor_channel_report_array(0, sensor_data, 10);
if (result != 0) {
    // Handling Errors
}
```

### 4. Example of Motor Current Loop

``` c
// Add current value reporting to the motor control cycle
void motor_current_control_loop(void)
{
    float i_d, i_q;
    
    // Obtain the current value
    get_motor_current(&i_d, &i_q);
    
    // Report D-axis current (Channel 0)
    monitor_channel_add_data(0, &i_d);
    
    // Report Q-axis current (Channel 1)
    monitor_channel_add_data(1, &i_q);
    
    // Continue with the motor control logic...
}
```

## Core Functionality

### Protocol Analysis

- Analyze data frames transmitted by the host computer
- Verify frame integrity

### Memory Access Engine
- Securely reads and writes memory based on the address and length specified in the command
- Supports basic data types: int8/16/32, uint8/16/32, float, double

### Data Packetization
- Packages and uploads read memory data or status codes according to protocol formats
- Supports data encapsulation for multiple sampling modes

### Integration Method
- Provides *.c / *.h source files for simple porting
- Zero code intrusion: No need to add debug statements in business code

## Technical Advantages

### 1. Low CPU Utilization

- DMA utilized for both UART/USB transmission and reception
- Stream and buffer sampling accomplished via GPTMR + DMA
- Minimizes CPU utilization

### 2. Independent Memory Pool

- Dedicated memory pool management
- Does not interfere with user memory usage

### 3. Flexible Sampling Modes

- Supports three sampling modes to adapt to diverse application scenarios
- Enables user-defined channels for maximum flexibility

### 4. Real-Time Performance Guarantee

- Supports polling mode to bypass interrupts and ensure user real-time requirements
- Multiple sampling modes accommodate varying real-time demands



## Version History

 - v1.0: Initial release supporting basic variable viewing and configuration, plus Notify mode
 - v2.0: New features:
    - Support for V2 protocol
    - Added Stream mode and Buffer mode
    - Support for user-defined channel reporting
    - Support for independent memory pools

## Important Notes

1. When using Stream mode and Buffer mode, ensure GPTMR and DMA resources are available
2. When configuring custom channel reporting, pay attention to data frequency and length to avoid communication bottlenecks
3. Adjust the size of the dedicated memory pool based on actual requirements
4. In multi-task environments, be mindful of data synchronization issues

## API

:::{eval-rst}

About software API: `API doc <../../_static/middleware/hpm_monitor/html/index.html>`_ .
:::