# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(CONFIG_A_OTA 1)

#enable diff image
set(CONFIG_DIFF_IMAGE_ENABLE 1)

# set(CONFIG_UART_CHANNEL 1)
# set(CONFIG_ENET_CHANNEL 1)
set(CONFIG_USB_DEVICE_CHANNEL 1)
# set(CONFIG_USB_HOST_CHANNEL 1)
# set(CONFIG_ECAT_FOE_CHANNEL 1)

if(DEFINED CONFIG_DIFF_IMAGE_ENABLE)
    if (${CONFIG_DIFF_IMAGE_ENABLE})
    set(HEAP_SIZE 0x8000)
    endif()
endif()

string(FIND ${BOARD} "53" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_FLASH_1M 1)
endif()

string(FIND ${BOARD} "5e" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_FLASH_1M 1)
    set(CONFIG_MBEDTLS 1)
endif()

if("${HPM_BUILD_TYPE}" STREQUAL "ram")
    set(CONFIG_USE_RAM_TYPE 1)
endif()

if(DEFINED CONFIG_UART_CHANNEL)
    if (${CONFIG_UART_CHANNEL})
    endif()
endif()

if(DEFINED CONFIG_ENET_CHANNEL)
    if (${CONFIG_ENET_CHANNEL})
        set(CONFIG_ENET_PHY 1)
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
        set(CONFIG_USB_DEVICE_MSC 1)
    endif()
endif()

if(DEFINED CONFIG_USB_HOST_CHANNEL)
    if (${CONFIG_USB_HOST_CHANNEL})
        set(CONFIG_TINYUSB 1)
        set(CONFIG_USB_HOST 1)
        set(CONFIG_TINYUSB_HOST 1)
        set(CONFIG_FATFS 1)
        set(CONFIG_USB_FATFS 1)
        set(CONFIG_USB_FATFS_TINYUSB 1)
        # sdk_compile_options("-Os")
    endif()
endif()

if(DEFINED CONFIG_ECAT_FOE_CHANNEL)
    if (${CONFIG_ECAT_FOE_CHANNEL})
        set(CONFIG_EEPROM_EMULATION 1) # using flash to emulate eeprom
    endif()
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_compile_definitions(-DBOOTUSER_ENABLE=1)

project(boot_user)

sdk_inc(inc)

sdk_app_src(src/boot_user.c)

add_subdirectory(../../../../ hpm_apps/build_tmp)

generate_ses_project()