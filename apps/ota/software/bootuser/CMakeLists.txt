# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.9.0\"")

# set(FLASH_XIP 1)
# set(LINK_TO_FLASH 1)
set(INCLUDE_BOOTHEADER 1)

# set(CONFIG_UART_CHANNEL 1)
# set(CONFIG_ENET_CHANNEL 1)
set(CONFIG_USB_DEVICE_CHANNEL 1)
# set(CONFIG_USB_HOST_CHANNEL 1)
# set(CONFIG_ECAT_FOE_CHANNEL 1)


string(FIND ${BOARD} "67" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM6700 1)
endif()

string(FIND ${BOARD} "63" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM6300 1)
endif()

string(FIND ${BOARD} "62" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM6200 1)
endif()

string(FIND ${BOARD} "53" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM5300 1)
    set(CONFIG_USE_FLASH_1M 1)
endif()

string(FIND ${BOARD} "68" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM6800 1)
endif()

string(FIND ${BOARD} "6e" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM6E00 1)
endif()

string(FIND ${BOARD} "5e" found)
if(${found} GREATER_EQUAL 0)
    set(CONFIG_USE_HPM5E00 1)
    set(CONFIG_USE_FLASH_1M 1)
    set(CONFIG_MBEDTLS 1)
endif()

if(DEFINED CONFIG_UART_CHANNEL)
    if (${CONFIG_UART_CHANNEL})
    endif()
endif()

if(DEFINED CONFIG_ECAT_FOE_CHANNEL)
    if (${CONFIG_ECAT_FOE_CHANNEL})
        set(CONFIG_EEPROM_EMULATION 1) # using flash to emulate eeprom
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

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DFLASH_XIP=1)
sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_compile_definitions(-DBOOTUSER_ENABLE=1)

if(DEFINED CONFIG_USE_HPM5E00)
    if (${CONFIG_USE_HPM5E00})
        sdk_compile_definitions(-DMBEDTLS_CONFIG_FILE="hpm_sdk_mbedtls_config.h")
    endif()
endif()

if(DEFINED CONFIG_USE_FLASH_1M)
    if (${CONFIG_USE_FLASH_1M})
        sdk_compile_definitions("-DCONFIG_USE_FLASH_1M=1")
    endif()
endif()

if(DEFINED CONFIG_UART_CHANNEL)
    if (${CONFIG_UART_CHANNEL})
        sdk_compile_definitions("-DCONFIG_UART_CHANNEL=1")
        sdk_inc(../common/channel/uart/inc)
        sdk_app_src(../common/channel/uart/src/uart_xmodem.c)
        sdk_app_src(../common/channel/uart/src/uart.c)
    endif()
endif()

if(DEFINED CONFIG_ECAT_FOE_CHANNEL)
    if (${CONFIG_ECAT_FOE_CHANNEL})
        sdk_compile_definitions("-DCONFIG_ECAT_FOE_CHANNEL=1")
        # ECAT SSC stack
        sdk_inc(../common/channel/ecat/SSC/Src)
        sdk_src(../common/channel/ecat/SSC/Src/bootmode.c)
        sdk_src(../common/channel/ecat/SSC/Src/coeappl.c)
        sdk_src(../common/channel/ecat/SSC/Src/ecatappl.c)
        sdk_src(../common/channel/ecat/SSC/Src/ecatcoe.c)
        sdk_src(../common/channel/ecat/SSC/Src/ecatfoe.c)
        sdk_src(../common/channel/ecat/SSC/Src/ecatslv.c)
        sdk_src(../common/channel/ecat/SSC/Src/mailbox.c)
        sdk_src(../common/channel/ecat/SSC/Src/objdef.c)
        sdk_src(../common/channel/ecat/SSC/Src/sdoserv.c)
        sdk_src(../common/channel/ecat/SSC/Src/foeappl.c)

        # hpm ecat port
        sdk_inc(${HPM_SDK_BASE}/samples/ethercat/port)
        sdk_src(${HPM_SDK_BASE}/samples/ethercat/port/hpm_ecat_hw.c)
        sdk_src(${HPM_SDK_BASE}/samples/ethercat/port/hpm_ecat_phy.c)
        if(CONFIG_EEPROM_EMULATION)
        sdk_src(${HPM_SDK_BASE}/samples/ethercat/port/hpm_ecat_e2p_emulation.c)
        endif()
        sdk_src(${HPM_SDK_BASE}/samples/ethercat/port/hpm_ecat_foe.c)

        sdk_inc(../common/channel/ecat)
        sdk_src(../common/channel/ecat/ssc_application.c)
        sdk_src(../common/channel/ecat/ecat_foe_support.c)
        sdk_src(../common/channel/ecat/ecat_foe.c)
    endif()
endif()

if(DEFINED CONFIG_ENET_CHANNEL)
    if (${CONFIG_ENET_CHANNEL})
        sdk_compile_definitions("-DCONFIG_ENET_CHANNEL=1")
        sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
        sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=0)
        sdk_compile_definitions(-DLWIP_DHCP=0)

        sdk_inc(../common/channel/enet/ports/baremetal/single)
        sdk_inc(../common/channel/enet/ports/baremetal/single/arch)
        sdk_inc(../common/channel/enet/common/single)
        sdk_inc(../common/channel/enet/lwip_tcpecho/inc)
        sdk_inc(../common/channel/enet/lwip_tcpecho/inc/app)

        sdk_app_src(../common/channel/enet/ports/baremetal/single/arch/sys_arch.c)
        sdk_app_src(../common/channel/enet/ports/baremetal/single/ethernetif.c)
        sdk_app_src(../common/channel/enet/common/single/common.c)
        sdk_app_src(../common/channel/enet/common/single/netconf.c)
        sdk_app_src(../common/channel/enet/lwip_tcpecho/src/app/tcp_echo.c)
        sdk_app_src(../common/channel/enet/lwip_tcpecho/src/lwip.c)
    endif()
endif()

if(DEFINED CONFIG_USB_DEVICE_CHANNEL)
    if (${CONFIG_USB_DEVICE_CHANNEL})
        sdk_compile_definitions("-DCONFIG_USB_DEVICE_CHANNEL=1")

        sdk_inc(../common/channel/usb/config)
        sdk_app_src(../common/channel/usb/device/msc/ram_disk/src/device.c)
        sdk_app_src(../common/channel/usb/device/msc/ram_disk/src/msc_ram.c)
    endif()
endif()

if(DEFINED CONFIG_USB_HOST_CHANNEL)
    if (${CONFIG_USB_HOST_CHANNEL})
        sdk_compile_definitions("-DCONFIG_USB_HOST_CHANNEL=1")
        sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
        sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
        sdk_compile_definitions(-DUSB_FATFS_ENABLE)
        # sdk_compile_options("-Os")

        sdk_inc(../common/channel/usb/host/msc_udisk/inc)
        sdk_app_src(../common/channel/usb/host/msc_udisk/src/host.c)
        sdk_app_src(../common/channel/usb/host/msc_udisk/src/msc_app.c)
        sdk_app_src(../common/channel/usb/host/msc_udisk/src/file_op.c)
    endif()
endif()

project(boot_user)


sdk_inc(inc)
sdk_inc(../common/include)
sdk_inc(../common/channel/inc)

sdk_app_src(../common/apphead/hpm_appheader.c)
sdk_app_src(../common/crypto/hpm_hashs.c)
sdk_app_src(../common/otamgr/hpm_ota.c)
sdk_app_src(../common/platform/hpm_platform.c)
sdk_app_src(src/boot_user.c)

add_subdirectory(../../../.. hpm_apps/build_tmp)

generate_ses_project()