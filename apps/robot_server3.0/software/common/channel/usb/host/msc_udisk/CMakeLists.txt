# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

# set(CONFIG_TINYUSB 1)
# set(CONFIG_USB_HOST 1)
# set(CONFIG_TINYUSB_HOST 1)
# set(CONFIG_FATFS 1)
# set(CONFIG_USB_FATFS 1)
# set(CONFIG_USB_FATFS_TINYUSB 1)

# sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
# sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
# sdk_compile_definitions(-DUSB_FATFS_ENABLE)
#sdk_compile_options("-Os")

sdk_inc(inc)
sdk_src(src/host.c)
sdk_src(src/msc_app.c)
sdk_src(src/file_op.c)
