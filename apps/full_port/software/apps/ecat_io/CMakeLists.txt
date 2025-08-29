# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(APP_VERSION_STRING "\"1.10.0\"")

set(CONFIG_EEPROM_EMULATION 1) # using eeprom emulation compenent

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ecat_io)

sdk_inc(SSC/Src)
sdk_src(SSC/Src/coeappl.c)
sdk_src(SSC/Src/ecatappl.c)
sdk_src(SSC/Src/ecatcoe.c)
sdk_src(SSC/Src/ecatslv.c)
sdk_src(SSC/Src/mailbox.c)
sdk_src(SSC/Src/objdef.c)
sdk_src(SSC/Src/sdoserv.c)

sdk_inc(../../drivers/src/full_ecat_port)
sdk_src(../../drivers/src/full_ecat_port/hpm_ecat_hw.c)
sdk_src(../../drivers/src/full_ecat_port/hpm_ecat_phy.c)

sdk_inc(./)
sdk_inc(../../drivers/inc)
sdk_inc(../../drivers/inc/tsn)
sdk_app_src(../../drivers/src/full_port_key_drv.c)
sdk_app_src(../../drivers/src/full_port_led_drv.c)
sdk_app_src(../../drivers/src/full_port_log_drv.c)
sdk_app_src(../../drivers/src/full_port_tsn_drv.c)
sdk_app_src(../../drivers/src/full_port_lcd_drv.c)

if(CONFIG_EEPROM_EMULATION)
sdk_src(../../drivers/src/full_ecat_port/hpm_ecat_e2p_emulation.c)
endif()

sdk_src(application/digital_io.c)

sdk_inc(.)
sdk_app_src(main.c)

generate_ide_projects()
