# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
add_subdirectory_ifdef(CONFIG_ENET_CHANNEL enet)
add_subdirectory_ifdef(CONFIG_UART_CHANNEL uart)
add_subdirectory_ifdef(CONFIG_USB_DEVICE_CHANNEL usb/device/msc/ram_disk)
add_subdirectory_ifdef(CONFIG_USB_HOST_CHANNEL usb/host/msc_udisk)