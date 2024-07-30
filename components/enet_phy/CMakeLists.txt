# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

add_subdirectory_ifdef(CONFIG_ENET_PHY_KSZ8081 ksz8081)