cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(PitangaPower C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)


set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format
        -Wno-unused-function
        )

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

set(FAMILY rp2040)
set(BOARD pico_sdk)

add_subdirectory(pico-vfs)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")

add_executable(pitanga_power_firmware)

target_sources(pitanga_power_firmware PUBLIC ${SOURCES} ${HEADERS})
target_include_directories(pitanga_power_firmware PUBLIC src src/Config external pico-vfs/include)
target_link_libraries(pitanga_power_firmware PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

pico_enable_filesystem(pitanga_power_firmware)
pico_add_extra_outputs(pitanga_power_firmware)
