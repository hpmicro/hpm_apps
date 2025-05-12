# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.8.0\"")

# set(CONFIG_HPM_MATH 1)
set(CONFIG_A_LIBMOTORCTRL 1)
# set(HPM_MATH_SES_LIB "libdspf")
#set(SES_TOOLCHAIN_VARIANT "Andes")



find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
# sdk_compile_options("-Os")
project(libtest)



sdk_app_inc(src)
sdk_app_src(src/libcmdtest.c)



sdk_ld_options("-lm")
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)
# sdk_compile_options("-mabi=ilp32f")
# sdk_compile_options("-march=rv32imafc")
# sdk_compile_options("-mabi=ilp32d")
add_subdirectory(../../../ hpm_apps/build_tmp)
generate_ses_project()