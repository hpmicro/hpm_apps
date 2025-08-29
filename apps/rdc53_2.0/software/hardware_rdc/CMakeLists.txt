# Copyright (c) 2023 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(APP_VERSION_STRING "\"1.10.0\"")
#set(CONFIG_SEGGER_RTT 1)
set(RV_ABI  "ilp32d")
set(RV_ARCH "rv32gc")
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

#sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1)
#sdk_ses_opt_lib_io_type(RTT)
sdk_ses_opt_debug_connection(J-Link)

sdk_nds_compile_options("-mext-dsp")
sdk_ld_options("-lm")
project(rdc_hardware)
sdk_app_inc(inc)
sdk_app_inc(src)
sdk_app_src(src/rdc.c)
sdk_app_src(src/sei_init.c)
sdk_app_src(src/uart_init.c)
sdk_app_src(src/spi_init.c)
sdk_app_src(src/qeo_init.c)
sdk_app_src(src/qei_init.c)
sdk_app_src(src/pll_init.c)
sdk_app_src(src/rdc_cfg.c)
sdk_app_src(src/gpio_led.c)
sdk_app_src(src/tamagawa_slave_init.c)
sdk_app_src(src/bissc_slave_init.c)



add_subdirectory(../../../../ hpm_apps/build_tmp)
generate_ide_projects()

cmake_minimum_required(VERSION 3.13)


