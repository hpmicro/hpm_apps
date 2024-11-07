# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
add_subdirectory_ifdef(CONFIG_A_LIBMOTORCTRL hpm_motor)
add_subdirectory_ifdef(CONFIG_A_HPMMONITOR hpm_monitor)