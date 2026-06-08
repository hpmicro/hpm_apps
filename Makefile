# avoid 'Entering|Leaving directory' messages
ifndef VERBOSE
MAKEFLAGS += --no-print-directory
endif

COM ?= COM10
HPM_BOARD_SEARCH_PATH ?= /home/sakumisu/repo/hpmicro/hpm_app/boards
HPM_DEMO ?= samples/hello_world
HPM_BOARD ?= hpm6800evk
HPM_CMAKE_BUILD_TYPE ?= flash_sdram_xip
HPM_PROBE ?= ft2232
HPM_SOC ?= hpm6880

PS_PROGRAM = /mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
FLASH_PROGRAM = X:\\Tools\\HPMicro_Manufacturing_Tool_v0.3.1\\hpm_manufacturing_cmd
MAKE_PROGRAM = X:\\Tools\\make\\make

command_flash = $(FLASH_PROGRAM) -c -p $(COM) -r "write-memory 0x0 0x200 [[0xfcf90002,0x00000006,0x00001000,0x00000000]]" -r "config-memory 0x10000 0x200" -r "write-memory 0x10000 0x80000400 $(HPM_DEMO)/build/output/demo.bin"

ifeq ($(patsubst %WSL2,WSL2,$(shell uname -r)), WSL2)
	command_flash = $(PS_PROGRAM) $(MAKE_PROGRAM) flash COM=$(COM) HPM_DEMO=$(HPM_DEMO) HPM_BOARD=$(HPM_BOARD) HPM_CMAKE_BUILD_TYPE=$(HPM_CMAKE_BUILD_TYPE)
endif

#cmake definition config
cmake_definition+= -DBOARD_SEARCH_PATH=${HPM_BOARD_SEARCH_PATH}
cmake_definition+= -DBOARD=${HPM_BOARD}
cmake_definition+= -DHPM_BUILD_TYPE=${HPM_CMAKE_BUILD_TYPE}
cmake_definition+= -DCMAKE_BUILD_TYPE=debug

build:Makefile
	cd $(HPM_DEMO) && \
	cmake -S . -B build -G "MinGW Makefiles" $(cmake_definition) && \
	cmake --build build -j 16
	cd $(HPM_DEMO) && cp build/output/demo.elf $(HPM_SDK_BASE)/demo.elf
	cd $(HPM_DEMO) && cp build/output/demo.bin $(HPM_SDK_BASE)/demo.bin
	cd $(HPM_DEMO) && cp build/output/demo.map $(HPM_SDK_BASE)/demo.map

openocd:
	sudo openocd -f ${HPM_BOARD_SEARCH_PATH}/openocd/probes/$(HPM_PROBE).cfg -f ${HPM_BOARD_SEARCH_PATH}/openocd/soc/$(HPM_SOC).cfg -f ${HPM_BOARD_SEARCH_PATH}/openocd/boards/$(HPM_BOARD).cfg

gdb:
	cd $(HPM_DEMO) && \
	$(GNURISCV_TOOLCHAIN_PATH)/bin/riscv32-unknown-elf-gdb ./build/output/demo.elf -ex "target remote :3333"

clean:
	cd $(HPM_DEMO) && \
	rm -rf build && \
	rm -rf .cache

emStudio:
	cd $(HPM_DEMO) && \
	cmake -S . -B build -G "MinGW Makefiles" $(cmake_definition) && \
	/opt/SEGGER/segger_embedded_studio_8.10d/bin/emStudio build/segger_embedded_studio/*.emProject

.PHONY:build flash clean emStudio