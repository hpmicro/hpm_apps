# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(HPM_SDK_LD_NO_NANO_SPECS 1)
set(CONFIG_FREERTOS 1)
set(CONFIG_CMSIS_RTOS2 1)
#Set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR to use GPTMR as system's tick source
set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
set(CONFIG_HPM_LOG 1)
set(CONFIG_CHERRYSH 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_CDC_ACM 1)
set(CONFIG_CHERRYSH_INTERFACE "usb")
set(APP_USE_ENET_PORT_COUNT 1)
set(APP_USE_ENET_ITF_RGMII 1)
#set(APP_USE_ENET_ITF_RMII 1)
#set(APP_USE_ENET_PHY_DP83867 1)
#set(APP_USE_ENET_PHY_RTL8211 1)
#set(APP_USE_ENET_PHY_DP83848 1)
#set(APP_USE_ENET_PHY_RTL8201 1)
#set(APP_USE_ENET_PHY_YT8531C 1)
set(CONFIG_CHERRYECAT 1)
set(CONFIG_A_CHERRYECAT 1)
set(CONFIG_CHERRYECAT_OSAL "freertos")
set(CONFIG_LVGL 1)
set(CONFIG_LVGL_CUSTOM_PORTABLE 1)


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ecat_master)

sdk_compile_definitions(-D__freertos_irq_stack_top=_stack)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DHPM_LOG_CONFIG_ENABLE_TIMESTAMP=0)
sdk_compile_definitions(-DHPM_LOG_CONFIG_ENABLE_FUNC_NAME=0)
sdk_compile_definitions(-DHPM_LOG_CONFIG_LEVEL=HPM_LOG_LEVEL_DEBUG)
# LVGL CONF
sdk_compile_definitions(-DLV_USE_OS=LV_OS_CMSIS_RTOS2)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_compile_definitions(-DLV_LOG_LEVEL=LV_LOG_LEVEL_ERROR)

sdk_compile_options("-O2")

sdk_inc(drivers)
sdk_inc(apps)
sdk_inc(.)

sdk_app_src(drivers/drv_log.c)
sdk_app_src(drivers/drv_led.c)
sdk_app_src(drivers/drv_lcd.c)
sdk_app_src(drivers/drv_key.c)

sdk_app_src(apps/gw_thread.c)
sdk_app_src(apps/gw_mq.c)
sdk_app_src(apps/gw_misc.c)
sdk_app_src(apps/gw_usb.c)
sdk_app_src(apps/gw_ecat_master.c)
sdk_app_src(apps/gw_lcd.c)


sdk_app_src(main.c)

add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ide_projects()


