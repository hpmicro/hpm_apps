# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(sinad)

sdk_app_src(src/sinad.c)

add_subdirectory(../../../../ hpm_apps/build_tmp)

generate_ses_project()
