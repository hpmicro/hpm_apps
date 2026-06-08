# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
#Set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR to use GPTMR as system's tick source
set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_NETCONN_API 1)
set(CONFIG_HPM_LOG 1)
set(CONFIG_CHERRYSH 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_CDC_ACM 1)
set(CONFIG_CHERRYSH_INTERFACE "usb")
set(APP_USE_TSW_PORT_COUNT 1)
set(CONFIG_A_U8G2 1)
set(CONFIG_A_SSD1306 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=1)
sdk_compile_definitions(-D_ENET_PORT_COUNT=2)
sdk_compile_definitions(-DLWIP_DHCP=0)
sdk_compile_definitions(-DUSE_LWIPOPTS_APP_H=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DHPM_LOG_CONFIG_ENABLE_TIMESTAMP=0)
sdk_compile_definitions(-DHPM_LOG_CONFIG_ENABLE_FUNC_NAME=0)
sdk_compile_definitions(-DHPM_LOG_CONFIG_LEVEL=HPM_LOG_LEVEL_DEBUG)
sdk_compile_definitions(-DUSE_U8G2=0)
sdk_compile_definitions(-D_TCP_ECHO=0)

project(enet_connect_tsn)

sdk_inc(drivers)
sdk_inc(drivers/enet_tsn)
sdk_inc(drivers/enet_tsn/ports)
sdk_inc(drivers/enet_tsn/ports/arch)
sdk_inc(drivers/enet_tsn/common)
sdk_inc(apps)
sdk_inc(.)

sdk_app_src(drivers/drv_log.c)
sdk_app_src(drivers/drv_led.c)
sdk_app_src(drivers/drv_key.c)
sdk_app_src(drivers/drv_oled.c)
sdk_app_src(drivers/enet_tsn/drv_enet_tsn.c)
sdk_app_src(drivers/enet_tsn/ports/arch/sys_arch.c)
sdk_app_src(drivers/enet_tsn/ports/ethernetif.c)
sdk_app_src(drivers/enet_tsn/common/common.c)
sdk_app_src(drivers/enet_tsn/common/netconf.c)

sdk_app_src(apps/app_thread.c)
sdk_app_src(apps/app_mq.c)
sdk_app_src(apps/app_misc.c)
sdk_app_src(apps/app_usb.c)
sdk_app_src(apps/app_enet_tsn.c)
sdk_app_src(apps/app_tcp.c)
sdk_app_src(apps/app_udp.c)



sdk_app_src(main.c)
add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ide_projects()
