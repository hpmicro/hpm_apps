# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_A_ETHERCAT 1)

set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 1)
#set(APP_USE_ENET_ITF_RGMII 1)
#set(APP_USE_ENET_ITF_RMII 1)
#set(APP_USE_ENET_PHY_DP83867 1)
#set(APP_USE_ENET_PHY_RTL8211 1)
#set(APP_USE_ENET_PHY_DP83848 1)
set(APP_USE_ENET_PHY_RTL8201 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ethercat_master_freertos)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(src)
sdk_app_src(src/main.c)

add_subdirectory(../../../.. hpm_apps/build_tmp)
generate_ses_project()

