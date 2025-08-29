# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/segger/ucs1903_ram.icf)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ucs1903_led)

sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)

sdk_inc(inc)

sdk_app_src(src/timer_led.c)

add_subdirectory(../../../.. hpm_apps/build_tmp)

generate_ses_project()
