# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(HPM_SDK_NO_NANO_SPECS 1)
set(CONFIG_HPM_ADC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(power_core)

sdk_inc(.)
sdk_inc(inc)
sdk_inc(../adapte/inc)

sdk_app_src(../adapte/src/mp_adc.c)
sdk_app_src(../adapte/src/mp_pwm.c)
sdk_app_src(../adapte/src/mp_api.c)
sdk_app_src(../adapte/src/mp_timer.c)
sdk_app_src(src/demo_led.c)
sdk_app_src(src/demo_key.c)
sdk_app_src(src/demo_gpio.c)
sdk_app_src(src/demo.c)

sdk_ld_options("-lm")
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)

add_subdirectory(../../../.. hpm_apps/build_tmp)

generate_ses_project()
