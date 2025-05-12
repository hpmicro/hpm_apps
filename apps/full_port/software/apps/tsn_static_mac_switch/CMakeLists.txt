# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.9.0\"")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(static_mac_switch)

sdk_inc(./)
sdk_inc(../../drivers/inc)
sdk_inc(../../drivers/inc/tsn)

sdk_app_src(../../drivers/src/full_port_key_drv.c)
sdk_app_src(../../drivers/src/full_port_led_drv.c)
sdk_app_src(../../drivers/src/full_port_log_drv.c)
sdk_app_src(../../drivers/src/full_port_tsn_drv.c)
sdk_app_src(../../drivers/src/full_port_lcd_drv.c)

sdk_app_src(main.c)

add_subdirectory(../../../../.. hpm_apps/build_tmp)

generate_ide_projects()


