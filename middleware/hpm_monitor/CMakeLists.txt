## Script for hpmicro ##
sdk_inc(core)
sdk_src(core/monitor_profile.c)
sdk_src(core/monitor_task.c)

sdk_inc(port/hpm)
sdk_src(port/hpm/monitor_ticktime.c)

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")
    sdk_inc(port/hpm/barebone_uart)
    sdk_src(port/hpm/barebone_uart/monitor.c)
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    sdk_inc(port/hpm/barebone_usb)
    sdk_src(port/hpm/barebone_usb/monitor.c)
else()
    message(FATAL_ERROR "monitor currently not ported ${CONFIG_MONITOR_INTERFACE} with barebone")
endif()

