# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
# set(CONFIG_HPM_MATH 1)
#set(CONFIG_A_MOTORCTRL 1)
set(CONFIG_MOTORCTRL 1)
set(CONFIG_HPM_ADC 1)
set(CONFIG_A_LIBMOTORCTRL 1)
# set(HPM_MATH_SES_LIB "libdspf")

#for monitor
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
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    sdk_compile_definitions("-DCONFIG_USB_CHANNEL=1")
endif()
#end for monitor



#find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
# sdk_compile_options("-Os")
project(pmsm)



sdk_app_inc(src)
sdk_app_src(src/bldc_foc_callback.c)
sdk_app_src(src/pmsm.c)
sdk_app_src(src/pmsm_currentctrl.c)
sdk_app_src(src/pmsm_speedctrl.c)
sdk_app_src(src/pmsm_init.c)
sdk_app_src(src/command_process.c)
#sdk_app_src(src/cmd_gene.c)



sdk_ld_options("-lm")
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)


#use hpm_apps/middleware
add_subdirectory(../../../ hpm_apps/build_tmp)
#end use hpm_apps/middleware
generate_ses_project()