# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(rom_uart_ota)

sdk_inc(../common)
sdk_inc(inc)

sdk_app_src(../common/ota_api.c)
sdk_app_src(src/uart_xmodem.c)
sdk_app_src(src/main.c)

generate_ses_project()
