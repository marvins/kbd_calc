# RP2350 hardware configuration (bare Pico stub)
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

# Bare RP2350 compile definitions
set(HARDWARE_COMPILE_DEFINITIONS
    TARGET_RP2350=1
    DEFAULT_CONFIG_PATH="data/configs/mf/"
)

# RP2350 libraries (from Pico SDK)
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

# RP2350 include directories
set(HARDWARE_INCLUDE_DIRS
    ${PICO_SDK_PATH}/src
    ${PICO_SDK_PATH}/src/rp2350/hardware_regs/include
    ${PICO_SDK_PATH}/src/rp2350/hardware_structs/include
)

# GUI sources
set(GUI_SOURCES
    src/overboard/gui/app_view.cpp
    src/overboard/gui/lcd_section.cpp
    src/overboard/gui/math_canvas.cpp
    src/overboard/gui/lv_font_superscript.c
)

# Bare RP2350 Pico HAL sources (stub)
set(HARDWARE_SOURCES
    src/overboard/hal/pico/app.cpp
    src/overboard/hal/pico/display.cpp
    src/overboard/hal/pico/input.cpp
    ${GUI_SOURCES}
)

# Bare Pico: placeholder dimensions, update when display is chosen
set(DISPLAY_FULL_WIDTH  320)
set(DISPLAY_FULL_HEIGHT 320)
set(DISPLAY_KBD_WIDTH   320)
set(DISPLAY_KBD_HEIGHT     100)
set(DISPLAY_KBD_WIN_HEIGHT   0)
set(DISPLAY_PREVIEW_MAX_WIDTH   120)
set(DISPLAY_PREVIEW_MAX_HEIGHT   60)

set(DISPLAY_KEY_PAD       1)
set(DISPLAY_KBD_HEADER_H 14)
set(DISPLAY_KBD_MARGIN_L  4)
set(DISPLAY_KBD_MARGIN_T  3)
set(DISPLAY_FONT_SIZE     10)

configure_file(
    ${CMAKE_SOURCE_DIR}/templates/display_config.hpp.in
    ${CMAKE_BINARY_DIR}/src/overboard/hal/display_config.hpp
    @ONLY
)

