# Copyright (c) 2021-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions_ifdef(BUILD_FOR_SECONDARY_CORE BOARD_RUNNING_CORE=HPM_CORE1)

sdk_inc(.)
sdk_src(pinmux.c)
sdk_src(board.c)

set(ENV{CONFIG_ENET_PHY_RTL8211F} 1)
sdk_compile_definitions(-DRGMII=1)
sdk_compile_definitions(-DHPM_ENET_RGMII=1)