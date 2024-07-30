# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

# set(CONFIG_CHERRYUSB 1)
# set(CONFIG_USB_DEVICE 1)
# set(CONFIG_USB_DEVICE_MSC 1)

sdk_inc(../../../config)
sdk_src(src/device.c)
sdk_src(src/msc_ram.c)
