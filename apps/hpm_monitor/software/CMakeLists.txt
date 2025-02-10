# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ARCH "rv32imafc")
set(RV_ABI "ilp32f")
set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_DSP 1)
set(HPM_MATH_DSP_SES_LIB "libdspf")


set(CONFIG_A_HPMMONITOR 1)
# set(CONFIG_MONITOR_INTERFACE "uart")
set(CONFIG_MONITOR_INTERFACE "usb")

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")

elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    set(CONFIG_CHERRYUSB 1)
    set(CONFIG_USB_DEVICE 1)
    set(CONFIG_USB_DEVICE_CDC 1)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})


if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")
    sdk_compile_definitions("-DCONFIG_UART_CHANNEL=1")
    sdk_compile_definitions("-DCONFIG_USE_CONSOLE_UART=1")
    sdk_compile_definitions("-DCONFIG_MONITOR_DBG_LEVEL=0")
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    sdk_compile_definitions("-DCONFIG_USB_CHANNEL=1")
endif()

project(hpm_monitor_demo)

sdk_inc(inc)
sdk_inc(inc/config)

sdk_app_src(src/main.c)

add_subdirectory(../../../ hpm_apps/build_tmp)

generate_ses_project()
