# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

if($ENV{CONFIG_TSW_PHY_YT8531C})
    add_subdirectory(yt8531c)
    message(STATUS "TSW PHY Enabled: YT8531C")
endif()

