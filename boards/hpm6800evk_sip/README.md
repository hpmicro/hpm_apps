HPM6880 DDR SIP support

- board.c: sdk board support source file, init_ddr_sip() is used for DDR initialization.
- board.h: sdk board support header file, updated DDR memory size
- hpm6800evk_sip.cfg: openocd config file, issue "monitor reset init" in gdb client will call the initialization routine implemented in this file
- hpm6800evk_xip.jlinkscript: it will initialize clock and DDR for jlink.