# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
# set(CONFIG_HPM_MATH 1)
set(CONFIG_MOTORCTRL  1)
set(CONFIG_A_LIBMOTORCTRL 1)
set(CONFIG_HPM_ADC 1)
# set(HPM_MATH_SES_LIB "libdspf")

cmake_minimum_required(VERSION 3.13)

set(CONFIG_UART_CHANNEL 1)
#set(CONFIG_ENET_CHANNEL 1)
#set(CONFIG_USB_DEVICE_CHANNEL 1)


if(DEFINED CONFIG_UART_CHANNEL)
    if (${CONFIG_UART_CHANNEL})
    endif()
endif()

if(DEFINED CONFIG_ENET_CHANNEL)
    if (${CONFIG_ENET_CHANNEL})
        set(CONFIG_LWIP 1)
        set(APP_USE_ENET_PORT_COUNT 1)
        #set(APP_USE_ENET_PHY_DP83867 1)
        set(APP_USE_ENET_PHY_RTL8211 1)
        #set(APP_USE_ENET_PHY_DP83848 1)
        # set(APP_USE_ENET_PHY_RTL8201 1)
    endif()
endif()

if(DEFINED CONFIG_USB_DEVICE_CHANNEL)
    if (${CONFIG_USB_DEVICE_CHANNEL})
        set(CONFIG_CHERRYUSB 1)
        set(CONFIG_USB_DEVICE 1)
        set(CONFIG_USB_DEVICE_CDC 1)
    endif()
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

if(DEFINED CONFIG_UART_CHANNEL)
    if (${CONFIG_UART_CHANNEL})
        sdk_compile_definitions("-DCONFIG_UART_CHANNEL=1")
        sdk_inc(channel/uart/inc)
        sdk_app_src(channel/uart/src/uart.c)
    endif()
endif()

if(DEFINED CONFIG_ENET_CHANNEL)
    if (${CONFIG_ENET_CHANNEL})
        sdk_compile_definitions("-DCONFIG_ENET_CHANNEL=1")
        sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
        sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=0)
        sdk_compile_definitions(-DLWIP_DHCP=0)

    endif()
endif()

if(DEFINED CONFIG_USB_DEVICE_CHANNEL)
    if (${CONFIG_USB_DEVICE_CHANNEL})
        sdk_compile_definitions("-DCONFIG_USB_DEVICE_CHANNEL=1")

        sdk_inc(channel/usb/config)
        sdk_app_src(channel/usb/device/cdc_acm/cdc_acm.c)
        sdk_app_src(channel/usb/device/cdc_acm/usb_channel.c)
    endif()
endif()

project(pmsm)

sdk_app_inc(channel/inc)
sdk_app_inc(debug)
sdk_app_inc(inc)
sdk_app_src(debug/debug_profile.c)
sdk_app_src(debug/debug_task.c)
sdk_app_src(debug/hpm_ticktime.c)

sdk_app_inc(src)
sdk_app_src(src/bldc_foc_callback.c)
sdk_app_src(src/pmsm.c)
sdk_app_src(src/pmsm_currentctrl.c)
sdk_app_src(src/pmsm_speedctrl.c)
sdk_app_src(src/pmsm_init.c)
sdk_app_src(src/wr_eeprom.c)
sdk_app_src(src/parm_global.c)



sdk_ld_options("-lm")
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)
# sdk_compile_options("-mabi=ilp32f")
# sdk_compile_options("-march=rv32imafc")
# sdk_compile_options("-mabi=ilp32d")
generate_ses_project()