# HPM_MONITOR + ADC 高性能数据监控解决方案

## 依赖SDK1.10.0

## 介绍

-本方案展示了如何在先楫半导体MCU微控制器上实现高性能 ADC 采样，并通过 hpm_monitor 服务实时传输数据到 PC 上位机，用于波形显示和分析。

-HPM6300 系列支持 3 个 16 位的 ADC 转换器，可以转换来自外部引脚以及芯片内部的模拟信号。ADC 的转换精度设置为 16 位时，最大采样率 2MSPS；ADC 的转换精度设置为 12 位时，最大采样率 4MSPS。ADC 支持读取转换模式、周期转换模式、序列转换模式和抢占转换模式。

-本方案以 HPM6300 开发板为例，介绍如何实现高速 ADC 采样，并通过 hpm_monitor 服务将采样数据实时传输到 PC 端的 HPMicroMonitorStudio 工具进行波形显示。方案采用抢占转换模式、PWM 触发方式，通过 DMA 自动搬运数据，实现高效的数据采集和传输。

## 方案特点

1. **高性能数据采集：**
    - 支持高达 2MSPS 的 16 位 ADC 采样
    - 采用 PWM 触发，采样时间精确可控
    - DMA 自动搬运，零 CPU 干预

2. **实时数据监控：**
    - 通过 hpm_monitor 服务实时传输 ADC 数据
    - 支持 USB/UART 两种通信接口
    - 低延迟、高带宽数据传输

3. **灵活的数据处理：**
    - 支持多种采样模式（Notify/Stream/Buffer）
    - 可自定义数据上报频率和格式
    - 支持多通道监控

4. **用户友好：**
    - PC 端图形化界面实时显示波形
    - 参数实时可调

## 框图及调用流程

![adc_lwipapi_main](doc/api/assets/ADC_LWIPAPI_main.png)

## 硬件配置

1. **开发板支持**
   支持全系列HPM EVK开发板。
   当前以HPM6300EVK验证，其余EVK需手动微调程序或引脚。

2. **通信接口选择**
    - USB CDC：推荐使用，带宽高，连接方便
    - UART：可选，最高支持 10Mbps 波特率

## 软件配置

1. **工程配置 (CMakeLists.txt)**

``` cmake
# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(RV_ARCH "rv32imafc")
set(RV_ABI "ilp32f")

# set(SES_TOOLCHAIN_VARIANT "Andes")
# set(CONFIG_HPM_MATH 1)
# set(CONFIG_HPM_MATH_DSP 1)
# set(HPM_MATH_DSP_SES_LIB "libdspf")


set(CONFIG_A_HPMMONITOR 1)
# set(CONFIG_MONITOR_INTERFACE "uart")
set(CONFIG_MONITOR_INTERFACE "usb")
# set(CONFIG_MONITOR_INTERFACE "enet")

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")

elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    set(CONFIG_CHERRYUSB 1)
    set(CONFIG_USB_DEVICE 1)
    set(CONFIG_USB_DEVICE_CDC 1)
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "enet")
    set(CONFIG_LWIP 1)
    set(CONFIG_ENET_PHY 1)
    set(APP_USE_ENET_PORT_COUNT 1)
    #set(APP_USE_ENET_ITF_RGMII 1)
    #set(APP_USE_ENET_ITF_RMII 1)
    #set(APP_USE_ENET_PHY_DP83867 1)
    #set(APP_USE_ENET_PHY_RTL8211 1)
    #set(APP_USE_ENET_PHY_DP83848 1)
    set(APP_USE_ENET_PHY_RTL8201 1)
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

sdk_ld_options("-lm")
# sdk_compile_options("-O2")

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

sdk_compile_definitions("-DHPM_MONITOR_ENABLE=1")

project(hpm_monitor_adc)

sdk_inc(../common/adc)
sdk_inc(inc)
sdk_inc(inc/config)

sdk_app_src(src/main.c)
sdk_app_src(../common/adc/adc_16_pmt.c)

add_subdirectory(../../../../ hpm_apps/build_tmp)

generate_ses_project()

```

2. **MONITOR配置 (monitor_config.h)**

```c
#define MONITOR_PID                  (0xFFFF)
#define MONITOR_VID                  (0x34B7) /* HPMicro VID */

#define MONITOR_PROFILE_MAXSIZE      (4096)

#define MONITOR_MEM_SIZE             (10*1024)

/*--------monitor log--------*/
#define CONFIG_MONITOR_PRINTF(...) printf(__VA_ARGS__)

#ifndef CONFIG_MONITOR_DBG_LEVEL
#define CONFIG_MONITOR_DBG_LEVEL MONITOR_DBG_INFO
#endif

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

## 核心代码实现

 - **定义ADC的触发源PWM和使用的互联管理器**
```c
#define BOARD_APP_ADC16_PMT_PWM         HPM_PWM0
#define BOARD_APP_ADC16_PMT_TRGM        HPM_TRGM0
#define BOARD_APP_ADC16_PMT_TRGM_IN     HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_APP_ADC16_PMT_TRGM_OUT    TRGM_TRGOCFG_ADCX_PTRGI0A

```
  - **定义ADC的采样率和每次传输的数据长度**
```c
#define APP_ADC16_DMA_BUFF_LEN_IN_BYTES   (1024U)
#define APP_ADC16_TCP_SEND_SIZE           (APP_ADC16_DMA_BUFF_LEN_IN_BYTES*sizeof(uint16_t))

```

  - **用户自定义通道定义**

```c
MONITOR_DEFINE_GLOBAL_VAR(adc_ch, 0, uint16_t, ADC_SAMPLE_FREQ, APP_ADC16_DMA_BUFF_LEN_IN_BYTES);
```

  - **MONITOR对ADC采样发送**
```c
static void monitor_adc_handle(void)
{
    if (adc_get_done(0) && adc_get_done(1))
    {
        printf("adc dual buff full up!\r\n");
    }
    if (adc_get_done(0) && monitor_report_ch_is_released(0))
    {
        adc_clear_done(0);
        monitor_channel_report_array(0, adc_get_buf_addr_point(0), APP_ADC16_DMA_BUFF_LEN_IN_BYTES);
    }
    else if (adc_get_done(1) && monitor_report_ch_is_released(0))
    {
        adc_clear_done(1);
        monitor_channel_report_array(0, adc_get_buf_addr_point(1), APP_ADC16_DMA_BUFF_LEN_IN_BYTES);
    }
}
```

  - **DEMO例程**
```c
int main(void)
{
    board_init();
    printf("monitor adc demo!\r\n");
    printf("__DATE__:%s, __TIME__:%s\r\n", __DATE__, __TIME__);

    monitor_init();

    adc_pmt_auto_config(ADC_SAMPLE_FREQ);
    while (1)
    {
        monitor_adc_handle();
        monitor_handle();
    }
    return 0;
}
```

## 工程路径

- 工程路径：monitor_adc/software/hpm_monitor

## 工程构建
- windows下工程构建\n
![WIN构建](doc/api/assets/windows_build.png)

- linux工程构建编译
```shell
    //切换到示例应用目录
    cd hpm_apps\apps\monitor_adc\software\hpm_monitor
    //创建build目录
    mkdir build
    //切换目录到"build"
    cd build
    //Ninja-build产生构建文件
    cmake -GNinja  -DBOARD=hpm6300evk -DCMAKE_BUILD_TYPE=flash_xip ..
    //编译
    ninja
  ```

## 硬件设置
- 本方案使用的hpm6300evk板。
- 用户可以使用其他evk板，注意修改对应的pin脚。
- hpm6300evk usb/uart连接PC
- hpm6300evk adc pin 和 hpm6200evk(DAC数模转换) dac pin连接。
- hpm6200evk 烧录DAC 例程，直接运行三角波发波，hpm6300evk adc 采集；
  ![had_env](doc/api/assets/hal_env_1.png)

## 工程运行

- 运行工程
- 使用串口助手观察是否有报错的日志。
- PC运行HPMicroMonitorStudio工具。
- 设置HPMicroMonitorStudio采样adc_ch数据，示波器监控adc波形。

## 上位机配置

 详细配置可查看[hpm_monitor服务说明](hpm_monitor_instruction_zh)

1. HPMicroMonitorStudio 配置
    - 打开 HPMicroMonitorStudio 工具
    - 选择正确的通信端口（USB 或 COM 端口）
    - 设置波特率（如使用 UART）
    - 连接设备

2. 波形显示配置
    - 添加波形显
    - 选择 adc_ch
    - 启动实时显示
  ![monitor_config_1](doc/api/assets/monitor_config_1.png)
  ![monitor_cofnig_2](doc/api/assets/monitor_config_2.png)

## 运行效果

  ![monitor_run_1](doc/api/assets/monitor_run_1.png)

## 注意事项
   - 当出现波形断帧时，说明传输存在丢包等情况。当前HPMicroMonitorStudio工具性能还在持续优化中，可适当降低ADC采样频率。

## API

:::{eval-rst}

关于软件API 请查看 `方案API 文档 <../../_static/apps/monitor_adc/html/index.html>`_ 。
:::
