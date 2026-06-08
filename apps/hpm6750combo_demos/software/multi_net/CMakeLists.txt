# Copyright (c) 2024-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

# Enable FreeRTOS
set(CONFIG_FREERTOS 1)
set(CONFIG_FREERTOS_HEAP "heap_5")

# Set the Heap size
set(HEAP_SIZE 4096)

# Enable LWIP stack
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_HTTPSRV 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_SOCKET_API 1)
set(CONFIG_LWIP_NETDB 1)

# Enable SDMMC stack
set(CONFIG_SDMMC 1)
set(CONFIG_CMSIS_RTOS2 1)

# Enable WiFi Stack
set(CONFIG_MHD_4345X 1)

# Enable Cherry Shell
set(CONFIG_CHERRYSH 1)
set(CONFIG_CHERRYSH_INTERFACE "uart")

# Select the customized linker file
set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker_file/gcc/mhd_wifi_flash_xip.ld)
set(CUSTOM_SES_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker_file/ses/mhd_wifi_flash_xip.icf)

set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 1)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_HOST_RNDIS 1)
set(CONFIG_USB_HOST_CDC_ECM 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

# FreeRTOS definitions
sdk_compile_definitions(-D__ENABLE_FREERTOS=1)
sdk_compile_definitions(-D__freertos_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_FREERTOS=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

# SDMMC stack definitions
sdk_compile_definitions(-DHPM_SDMMC_ENABLE_CACHE_MAINTENANCE=1)
sdk_compile_definitions(-DHPM_SDMMC_USE_CMSIS_OS2=1)
sdk_compile_definitions(-DHPM_SDMMC_HOST_ENABLE_IRQ=1)
sdk_compile_definitions(-DHPM_SDMMC_USE_ADMA2=0)
sdk_compile_definitions(-DHPM_SDMMC_XFER_CALLBACK_ENABLE=0)

# LWIP options
sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-DLWIP_DHCP=1)
sdk_compile_definitions(-DLWIP_TIMEVAL_PRIVATE=0)
sdk_compile_definitions(-DMEM_SIZE=256*1024)
sdk_compile_definitions(-DMEMP_NUM_PBUF=128)
sdk_compile_definitions(-DPBUF_POOL_SIZE=128)
sdk_compile_definitions(-DPBUF_POOL_BUFSIZE=1600)
sdk_compile_definitions(-DTCP_WND=64*1024)
sdk_compile_definitions(-DTCP_SND_BUF=64*1024)
sdk_compile_definitions(-DMEMP_NUM_TCPIP_MSG_INPKT=256)
sdk_compile_definitions(-DLWIP_SUPPORT_CUSTOM_PBUF=1)

# Enable the external RAM initialization logic
#sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)

# Set default optimization level
sdk_compile_options(-O2)

# Override the malloc and free functions to use the FreeRTOS heap
sdk_compile_definitions(-Dmalloc=pvPortMalloc)
sdk_compile_definitions(-Dfree=vPortFree)

project(mhd_wifi_demo)
sdk_inc(lwip_port)
sdk_inc(lwip_port/arch)
sdk_inc(inc)
sdk_inc(common)

sdk_app_src(lwip_port/arch/sys_arch.c)
sdk_app_src(lwip_port/ethernetif.c)
sdk_app_src(common/common.c)
sdk_app_src(common/netconf.c)
sdk_app_src(src/main.c)
sdk_app_src(src/app/ping_cmd.c)
sdk_app_src(src/app/iperf_cmd.c)

add_subdirectory(rtl8211f)

generate_ses_project()
