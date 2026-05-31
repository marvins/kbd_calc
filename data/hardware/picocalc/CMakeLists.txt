# ClockworkPi PicoCalc hardware configuration
# This file contains PicoCalc-specific build settings for the embedded target.
# It is included by the main CMakeLists.txt when TARGET_DEVICE=PICOCALC.
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

# PicoCalc compile definitions
set(HARDWARE_COMPILE_DEFINITIONS
    TARGET_RP2350=1
    BOARD_PICOCALC=1
)

# PicoCalc libraries (from Pico SDK)
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

# PicoCalc include directories
set(HARDWARE_INCLUDE_DIRS
    ${PICO_SDK_PATH}/src
    ${PICO_SDK_PATH}/src/rp2350/hardware_regs/include
    ${PICO_SDK_PATH}/src/rp2350/hardware_structs/include
)

# GUI sources
set(GUI_SOURCES
    src/overboard/gui/app_view.cpp
    src/overboard/gui/keyboard_display.cpp
    src/overboard/gui/lcd_section.cpp
    src/overboard/gui/math_canvas.cpp
    src/overboard/gui/lv_font_superscript.c
)

# PicoCalc HAL sources
set(HARDWARE_SOURCES
    src/overboard/hal/picocalc/app.cpp
    src/overboard/hal/picocalc/display.cpp
    src/overboard/hal/picocalc/input.cpp
    ${GUI_SOURCES}
)

# PicoCalc: 320x320 physical display, logical split ~320x220 LCD / 320x100 kbd
set(DISPLAY_FULL_WIDTH  320)
set(DISPLAY_FULL_HEIGHT 320)
set(DISPLAY_KBD_WIDTH   320)
set(DISPLAY_KBD_HEIGHT  100)
set(DISPLAY_PREVIEW_MAX_WIDTH   120)
set(DISPLAY_PREVIEW_MAX_HEIGHT   60)
set(DISPLAY_PREVIEW_OFFSET_Y     40)
set(DISPLAY_KEY_PAD       1)
set(DISPLAY_KBD_HEADER_H 12)
set(DISPLAY_KBD_MARGIN_L  2)
set(DISPLAY_KBD_MARGIN_T  2)
set(DISPLAY_FONT_SIZE     7)

configure_file(
    ${CMAKE_SOURCE_DIR}/templates/display_config.hpp.in
    ${CMAKE_BINARY_DIR}/src/overboard/hal/display_config.hpp
    @ONLY
)

