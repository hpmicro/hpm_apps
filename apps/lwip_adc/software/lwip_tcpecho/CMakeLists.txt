# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.8.0\"")

#set(HPM_SDK_LD_NO_NANO_SPECS 1)
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
    message(FATAL_ERROR "This sample doesn't supported more than one Ethernet phy!")
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=0)
sdk_compile_definitions(-DLWIP_DHCP=0)

project(lwip_tcpecho_example)
sdk_inc(../common)
sdk_inc(../common/arch)
sdk_inc(../common/feature)
sdk_inc(../common/adc)
sdk_inc(inc)
sdk_inc(inc/eth)
sdk_inc(inc/app)

sdk_app_src(../common/arch/sys_arch.c)
sdk_app_src(../common/arch/ethernetif.c)
sdk_app_src(../common/feature/common_lwip.c)
sdk_app_src(../common/adc/adc_16_pmt.c)
sdk_app_src(src/eth/netconf.c)
sdk_app_src(src/app/tcp_echo.c)
sdk_app_src(src/lwip.c)

add_subdirectory(../../../.. hpm_apps/build_tmp)

generate_ses_project()
