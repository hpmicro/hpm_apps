# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.11.0\"")

set(CONFIG_HPM_PANEL 1)
set(CONFIG_A_LVGL 1)

if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)

set(CONFIG_FREERTOS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(CONFIG_A_CHERRYECAT 1)
set(CONFIG_CHERRYECAT 1)
set(CONFIG_CHERRYECAT_OSAL "freertos")

set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 1)
set(APP_USE_ENET_PHY_RTL8201 1)

set(CONFIG_CHERRYSH 1)
set(CONFIG_CHERRYSH_INTERFACE "uart")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ecat_master_motor_control)

# LVGL CONF
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_compile_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE=1)
sdk_compile_definitions(-DLV_USE_SYSMON=1) 
# sdk_compile_definitions(-DLV_USE_OS=LV_OS_FREERTOS)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_compile_options("-O2")
sdk_ld_options("-lm")


# Include directories
sdk_inc(inc)
sdk_inc(ui)
sdk_inc(ui/components)
sdk_inc(ui/screens)

# Main application source
sdk_app_src(src/main.c)
sdk_app_src(src/motor_control.c)

# LVGL ui main files
sdk_app_src(ui/ui.c)
sdk_app_src(ui/ui_gen.c)

# Screen sources
sdk_app_src(ui/screens/home_gen.c)

#font sources
sdk_app_src(ui/fonts/SemiBold_Font_16_data.c)
sdk_app_src(ui/fonts/SemiBold_Font_20_data.c)
sdk_app_src(ui/fonts/SemiBold_Font_24_data.c)
sdk_app_src(ui/fonts/SemiBold_Font_28_data.c)
sdk_app_src(ui/fonts/SemiBold_Font_32_data.c)
sdk_app_src(ui/fonts/SemiBold_Font_36_data.c)
sdk_app_src(ui/fonts/SemiBold_Font_44_data.c)
sdk_app_src(ui/fonts/NotoSerifCJKsc_SemiBold_28_data.c)



    
  

# Image sources
sdk_app_src(ui/images/image_start_data.c)
sdk_app_src(ui/images/image_HPMicro_Logo_data.c)
sdk_app_src(ui/images/image_device_data.c)
sdk_app_src(ui/images/image_state_data.c)
sdk_app_src(ui/images/image_address_data.c)
sdk_app_src(ui/images/image_input_data.c)
sdk_app_src(ui/images/image_output_data.c)
sdk_app_src(ui/images/image_monitor_data.c)
sdk_app_src(ui/images/image_speed_data.c)
sdk_app_src(ui/images/image_gear_data.c)
sdk_app_src(ui/images/image_rotate_left_data.c)
sdk_app_src(ui/images/image_rotate_right_data.c)





# Components - base
sdk_app_src(ui/components/row/row_gen.c)
sdk_app_src(ui/components/column/column_gen.c)
sdk_app_src(ui/components/header/header_gen.c)
sdk_app_src(ui/components/monitor/monitor_gen.c)
sdk_app_src(ui/components/speed_slider/speed_slider_gen.c)
sdk_app_src(ui/components/in_output/in_output_gen.c)



sdk_app_src(${SOURCE_FILES})

add_subdirectory(../../../ hpm_apps/build_tmp)

generate_ide_projects()
