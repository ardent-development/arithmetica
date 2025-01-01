cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")

project(arithmetica_fw C CXX ASM) # C++ is not used but everything breaks without this.
set(PICO_PLATFORM rp2040)
set(PICO_BOARD pico_w)
set(PICO_NO_FLASH 0) # Run from flash, not SRAM

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug") # Debug flags
  set(CMAKE_C_FLAGS " \
    ${CMAKE_C_FLAGS} \
    -save-temps \
    -time \
    -Og \
    -ggdb3 \
  ")
  message(STATUS "Debug build: Adding debug flags (-save-temps -time -Og "
                 "-ggdb3)")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release") # Release flags
  set(CMAKE_C_FLAGS " \
    ${CMAKE_C_FLAGS} \
    -time \
    -save-temps \
    -Oz \
    -fconserve-stack \
    -fpeephole \
    -fpeephole2 \
    -mtune=cortex-m0plus \
    -march=armv6-m \
  ")
  message(STATUS "Release build: Adding release optimization flags (-time "
                 "-save-temps -Oz -fconserve-stack -fpeephole -fpeephole2 "
                 "-march=armv6-m -mtune=cortex-m0plus)")
endif()

pico_sdk_init()

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(arithmetica_fw
  main.c
)

pico_set_program_name(arithmetica_fw "arithmetica_fw")
pico_set_program_version(arithmetica_fw "0.1")

pico_enable_stdio_uart(arithmetica_fw 1) # Default serial output configuration
pico_enable_stdio_usb(arithmetica_fw 1) # Plain UART at 8n1/115200b and USB is
                                        # possible for serial comms

pico_add_extra_outputs(arithmetica_fw)

target_link_libraries(arithmetica_fw
  pico_stdlib
  pico_unique_id
  hardware_gpio
  hardware_uart
  hardware_clocks
  pico_time
  pico_cyw43_arch_none
 # pico_cyw43_arch_lwip_threadsafe_background
 # tinyusb_device
 # tinyusb_board
)