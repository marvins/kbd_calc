# RP2350 hardware configuration
# This file contains RP2350-specific build settings for the embedded target.
# It is included by the main CMakeLists.txt when TARGET_DEVICE=RP2350.
#
# Requires PICO_SDK_PATH environment variable to be set, pointing to the
# Raspberry Pi Pico SDK (e.g., /Users/marvin/Desktop/Projects/pico-sdk).

# Check for PICO_SDK_PATH environment variable
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable not set. "
                        "Please set it to the path of the Raspberry Pi Pico SDK.")
endif()

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH} CACHE PATH "Path to Raspberry Pi Pico SDK")

# Import the Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Initialize the Pico SDK
pico_sdk_init()

# Set up RP2350-specific compile definitions
set(HARDWARE_COMPILE_DEFINITIONS
    TARGET_RP2350=1
)

# RP2350-specific libraries (from Pico SDK)
set(HARDWARE_LIBRARIES
    pico_stdlib
    hardware_gpio
    hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_spi
    hardware_uart
    hardware_dma
    hardware_flash
    hardware_clocks
    hardware_watchdog
    hardware_xosc
    hardware_pll
    hardware_vreg
    hardware_exception
)

# RP2350-specific include directories
set(HARDWARE_INCLUDE_DIRS
    ${PICO_SDK_PATH}/src
    ${PICO_SDK_PATH}/src/rp2350/hardware_regs/include
    ${PICO_SDK_PATH}/src/rp2350/hardware_structs/include
)

# RP2350-specific source files (to be added as HAL is implemented)
set(HARDWARE_SOURCES
    # src/overboard/hal/rp2350/rp2350_app.cpp
    # src/overboard/hal/rp2350/rp2350_input.cpp
    # src/overboard/hal/rp2350/rp2350_display.cpp
    # src/overboard/hal/rp2350/rp2350_keymap.cpp
)
