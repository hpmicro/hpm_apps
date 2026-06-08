# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_CMSIS_RTOS2 1)
#Set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR to use GPTMR as system's tick source
set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
set(CONFIG_HPM_LOG 1)
set(CONFIG_CHERRYSH 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_CDC_ACM 1)
set(CONFIG_CHERRYSH_INTERFACE "usb")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(gateway2_device)

sdk_inc(drivers)
sdk_inc(apps)
sdk_inc(.)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DHPM_LOG_CONFIG_ENABLE_TIMESTAMP=0)
sdk_compile_definitions(-DHPM_LOG_CONFIG_ENABLE_FUNC_NAME=0)
sdk_compile_definitions(-DHPM_LOG_CONFIG_LEVEL=HPM_LOG_LEVEL_DEBUG)

sdk_app_src(drivers/drv_log.c)
sdk_app_src(drivers/drv_led.c)
sdk_app_src(drivers/drv_mcan.c)
sdk_app_src(drivers/drv_segment.c)
sdk_app_src(apps/gw_thread.c)
sdk_app_src(apps/gw_mq.c)
sdk_app_src(apps/gw_misc.c)
sdk_app_src(apps/gw_usb.c)
sdk_app_src(apps/gw_can.c)

sdk_app_src(main.c)
add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ide_projects()
