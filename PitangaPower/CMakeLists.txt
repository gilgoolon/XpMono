cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(PitangaPower C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_executable(PitangaPower
    main.c
)

pico_enable_stdio_usb(PitangaPower 1)
pico_enable_stdio_uart(PitangaPower 0)

target_link_libraries(PitangaPower pico_stdlib tinyusb_device tinyusb_board)

pico_add_extra_outputs(PitangaPower)
