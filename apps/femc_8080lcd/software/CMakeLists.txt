# Copyright (c) 2022 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker/segger/flash_xip_8080.icf)
set(INCLUDE_BOOTHEADER 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DFLASH_XIP=1)
sdk_compile_definitions(-DFEMC_8080LCD=1)

project(femc_8080lcd)

sdk_app_src(src/lcd8080.c)

add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ses_project()


