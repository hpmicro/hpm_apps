# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(CONFIG_SPI_NOR_FLASH 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(spi_nor_flash_demo)

sdk_inc(common/port)
sdk_inc(common/port/${BOARD})

sdk_app_src(common/port/hpm_serial_nor_host_port.c)
sdk_app_src(src/main.c)

add_subdirectory(../.. hpm_apps/build_tmp)

generate_ses_project()
