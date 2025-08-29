# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(CONFIG_SEGGER_RTT 1)
set(RV_ABI  "ilp32d")
set(RV_ARCH "rv32gc")
set(APP_VERSION_STRING "\"1.10.0\"")
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1)
sdk_ses_opt_lib_io_type(RTT)
sdk_ses_opt_debug_connection(J-Link)

sdk_nds_compile_options("-mext-dsp")
sdk_ld_options("-lm")
project(soft_rdc)


sdk_app_inc(src)
sdk_app_src(src/rdc_soft.c)
sdk_app_src(src/adc_dma.c)
sdk_app_src(src/adc_data_process.c)
sdk_app_src(src/pwm_dma.c)
add_subdirectory(../../../ hpm_apps/build_tmp)
generate_ide_projects()
