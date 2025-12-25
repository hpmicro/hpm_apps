# hpm_monitor

[English](./README_en.md)

(hpm_monitor_instruction_zh)=

## 简介

hpm_monitor 是一个高效的、易用的、可移植性高的服务，用于实时查看和设置设备中的全局变量，以及高速上报全局变量。常被用作监控数据示波器，对电机、电源等调试非常友好。

hpm_monitor服务需要搭配PC上位机HPMicroMonitorStudio工具使用。HPMicroMonitorStudio工具详细使用方法请参考工具帮助文档。
运行效果:
![pc_hpm_monitor_1](doc/api/assets/pc_hpm_monitor_1.png)

## 特点

- **高可移植性**：只需适配相关通信端口
- **简单调用**：仅需两个接口（init和handle轮询），对原始工程逻辑无任何修改
- **轻量级协议**：通信协议轻量且易于扩展
- **多种操作支持**：支持主动获取(GET)和设置(SET)全局变量
- **高速上报**：支持多种模式高速上报全局变量
- **多种采样模式**：支持Notify模式、Stream模式和Buffer模式
- **自定义上报**：支持用户自定义通道手动上报数据
- **独立内存池**：与用户内存完全独立，互不影响
- **低CPU占用**：UART/USB收发使用DMA，Stream/Buffer模式使用GPTMR+DMA

## 协议版本

hpm_monitor 当前支持最新V2协议。
``` c
#define MONITOR_PROFILE_VERSION (2)
```

## 采样模式

hpm_monitor v2.0 提供三种采样模式：

### 1. Notify模式

 - **采样方式**：由CPU完成数据采样
 - **特点**：支持多包机制一次性上报，采样抖动及精度受CPU负载影响
 - **适用场景**：CPU负载较低，需要实时采样上报的场景

### 2. Stream模式

 - **采样方式**：由GPTMR+DMA完成数据采样
 - **特点**：采样频率高、抖动小，不占用CPU资源，采样频率受通信通道带宽限制
 - **资源占用**：会占用DMA和GPTMR资源
 - **适用场景**：需要高频率、低抖动实时采样的场景
 - **注意**：由于采样由DMA完成，注意数据cache一致性问题。

### 3. Buffer模式

 - **采样方式**：由GPTMR+DMA完成数据采样
 - **特点**：采样频率极高、抖动小，不占用CPU资源，单次采样大buffer
 - **优势**：采样频率不受通信通道带宽限制
 - **资源占用**：会占用DMA和GPTMR资源
 - **适用场景**：需要极高频率采样，不依赖实时通信的场景
 - **注意**：由于采样由DMA完成，注意数据cache一致性问题。

### 4. 用户自定义通道

 - **采样方式**：用户手动采样上报
 - **特点**：支持单个数据或整包数组上报
 - **适用场景**：避免CPU或DMA采样丢失的情况，如电机电流环中添加电流值

## 使用方法

hpm_monitor 服务调用简单，只需要在初始化时调用monitor_init()，并在主循环中轮询monitor_handle()即可完成当前工具的启用。

### 1. APPS CMakeLists.txt配置

``` cmake

cmake_minimum_required(VERSION 3.13)
set(APP_VERSION_STRING "\"1.10.0\"")

# 启动HPMMONITOR
set(CONFIG_A_HPMMONITOR 1)
# 选择通信接口：uart 或 usb 或enet
set(CONFIG_MONITOR_INTERFACE "uart")
# set(CONFIG_MONITOR_INTERFACE "usb")
# set(CONFIG_MONITOR_INTERFACE "enet")

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")

elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    # USB配置（需要启用以下选项）
    set(CONFIG_CHERRYUSB 1)
    set(CONFIG_USB_DEVICE 1)
    set(CONFIG_USB_DEVICE_CDC 1)
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "enet")
    # ENET配置（需要启用以下选项）
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

# 定义启用的端口宏
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

### 2. monitor_config.h 配置
monitor_config.h 是hpm_monitor服务的配置头文件，默认可参考core/monitor_kconfig.h。

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

/*Monitor内存池大小*/
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
/**USB配置*/
#if defined(CONFIG_USB_CHANNEL) && CONFIG_USB_CHANNEL

#define CONFIG_USB_POLLING_ENABLE

#include "usb_config.h"
#define MONITOR_USB_BASE             CONFIG_HPM_USBD_BASE
#define MONITOR_USB_BUSID            0
#define MONITOR_USB_IRQ              CONFIG_HPM_USBD_IRQn
#define MONITOR_USB_PRIORITY         2

#endif
/**ENET配置*/
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

/**采样数据限制配置*/
#define MONITOR_REPORT_MAXCOUNT              (16)
#define MONITOR_STREAM_BUFFER_MAXCOUNT       MONITOR_REPORT_MAXCOUNT
#define MONITOR_CHANNEL_MAXCOUNT             MONITOR_REPORT_MAXCOUNT
#define MONITOR_TRIGGER_MAXCOUNT             (16)
#define MONITOR_DATA_LIST_MAXCOUNT           (20)

/**采样GPTMR+DMA配置*/
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

### 3.服务调用
hpm_monitor服务启用，只需在上层应用中调用init初始化，并在主循环中轮询handle即可完成。
参考如下:

``` C
#include "monitor.h"

int main(void)
{
    board_init();
    printf("hpm_monitor v2.0 demo!\r\n");
    
    // 初始化hpm_monitor服务
    monitor_init();
    
    while (1)
    {
        // 轮询处理monitor事务
        monitor_handle();
        
        // 用户业务代码...
    }
    
    return 0;
}
```

## 自定义通道数据上报

### 1. 定义全局变量并注册到监控通道

使用宏 MONITOR_DEFINE_GLOBAL_VAR 定义全局变量：

``` c
// 参数说明：name(变量名), channel(通道号), type(数据类型), frequency(采样频率), count(数据个数,0表示的那个数据，>0 表示数组多个数据)
MONITOR_DEFINE_GLOBAL_VAR(my_variable, 0, float, 1000, 1);
```

### 2. 添加单个数据上报

``` c
float current_value = 3.14f;
int result = monitor_channel_add_data(0, &current_value);
if (result != 0) {
    // 处理错误
}
```

### 3. 添加数组数据上报

``` c
float sensor_data[10] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
int result = monitor_channel_report_array(0, sensor_data, 10);
if (result != 0) {
    // 处理错误
}
```

### 4. 电机电流环示例

``` c
// 在电机控制循环中添加电流值上报
void motor_current_control_loop(void)
{
    float i_d, i_q;
    
    // 获取电流值
    get_motor_current(&i_d, &i_q);
    
    // 上报D轴电流（通道0）
    monitor_channel_add_data(0, &i_d);
    
    // 上报Q轴电流（通道1）
    monitor_channel_add_data(1, &i_q);
    
    // 继续电机控制逻辑...
}
```

## 核心功能

### 协议解析

 - 解析上位机下发的数据帧
 - 验证帧完整性

### 内存访问引擎
 - 根据命令中的地址和长度，安全地读写内存
 - 支持基本数据类型：int8/16/32, uint8/16/32, float, double

### 数据封包
 - 将读取到的内存数据或状态码按照协议格式打包上传
 - 支持多种采样模式的数据封装

### 集成方式
 - 提供 *.c / *.h 源文件，移植简单
 - 零代码侵入：无需在业务代码中添加调试语句

## 技术优势

### 1. 低CPU占用率

 - UART/USB收发均使用DMA
 - Stream和Buffer采样通过GPTMR+DMA完成
 - 最大限度降低CPU使用率

### 2. 独立内存池

 - 独立的内存池管理
 - 不干扰用户内存使用

### 3. 灵活的采样模式

 - 支持三种采样模式，适应不同场景需求
 - 支持用户自定义通道，提供最大灵活性

### 4. 实时性保证

 - 支持轮询模式，避免中断影响用户实时性要求
 - 多种采样模式满足不同实时性需求

## 版本历史

 - v1.0：初始版本，支持基本变量查看和设置，以及Notify模式
 - v2.0：新增功能：
    - 支持V2协议
    - 新增Stream模式和Buffer模式
    - 支持用户自定义通道上报
    - 支持独立内存池

## 注意事项

1. 使用Stream模式和Buffer模式时，需要确保GPTMR和DMA资源可用
2. 自定义通道上报时，注意数据频率和长度，避免通信堵塞
3. 配置独立内存池大小时，根据实际需求调整
4. 在多任务环境中使用时，注意数据同步问题

## API

:::{eval-rst}

关于软件API 请查看 `方案API 文档 <../../_static/middleware/hpm_monitor/html/index.html>`_ 。
:::