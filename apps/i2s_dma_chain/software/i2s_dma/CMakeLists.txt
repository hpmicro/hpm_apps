# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CODEC)
# set(CONFIG_CODEC "sgtl5000")
set(CONFIG_CODEC "wm8960")

#add hpm dsp lib for fft
set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ARCH "rv32imafc")
set(RV_ABI "ilp32f")
set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_DSP 1)
set(HPM_MATH_DSP_SES_LIB "libdspf")
#end add hpm dsp lib for fft

endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2s_dma_chain)

#config for fft
sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_compile_options("-O3")
#

sdk_app_src(src/audio_codec.c)
sdk_inc(../common/inc)
sdk_app_src(../common/src/i2s_dma_chain.c)
generate_ide_projects()
