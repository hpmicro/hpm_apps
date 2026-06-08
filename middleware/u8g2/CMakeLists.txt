file(GLOB COMPONENT_SRCS "csrc/*.c")
list(FILTER COMPONENT_SRCS EXCLUDE REGEX "u8x8_d_.*")
sdk_app_src(${COMPONENT_SRCS})
sdk_src_ifdef(CONFIG_A_SSD1306 csrc/u8x8_d_ssd1306_128x64_noname.c)
sdk_app_inc(csrc)

