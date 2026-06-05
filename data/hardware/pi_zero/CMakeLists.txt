# Pi Zero hardware configuration
# This file contains DRM/KMS-specific build settings for Pi Zero target.
# It is included by the main CMakeLists.txt when TARGET_DEVICE=ZERO.
# Uses direct HDMI output via DRM without X11.

# Find required libraries
find_package(PNG REQUIRED)

# DRM library for direct display access
find_library(DRM_LIBRARY drm REQUIRED)
find_library(GBM_LIBRARY gbm REQUIRED)

# Set up Pi Zero-specific compile definitions
set(HARDWARE_COMPILE_DEFINITIONS
    TARGET_ZERO=1
    SIMULATOR=0
    SHOW_KEYBOARD_UI=0
    DEFAULT_CONFIG_PATH="data/configs/mf/"
    EMBEDDED_JSON=1
    LV_USE_LINUX_DRM=1
)

# Pi Zero-specific libraries
set(HARDWARE_LIBRARIES
    ${DRM_LIBRARY}
    ${GBM_LIBRARY}
    PNG::PNG
    pthread
)

# Pi Zero-specific include directories
set(HARDWARE_INCLUDE_DIRS
    ${PNG_INCLUDE_DIRS}
)

# Display dimensions for Pi Zero (portrait mode - matches rotated display)
# Portrait: 480x800 (physical rotated from landscape)
set(DISPLAY_FULL_WIDTH  480)
set(DISPLAY_FULL_HEIGHT 800)
set(DISPLAY_KBD_WIDTH   480)
set(DISPLAY_KBD_HEIGHT  300)
set(DISPLAY_KBD_WIN_HEIGHT 300)
set(DISPLAY_PREVIEW_MAX_WIDTH  464)
set(DISPLAY_PREVIEW_MAX_HEIGHT 80)

set(DISPLAY_KEY_PAD       2)
set(DISPLAY_KBD_HEADER_H 20)
set(DISPLAY_KBD_MARGIN_L  8)
set(DISPLAY_KBD_MARGIN_T  6)
set(DISPLAY_FONT_SIZE     10)

configure_file(
    ${CMAKE_SOURCE_DIR}/templates/display_config.hpp.in
    ${CMAKE_BINARY_DIR}/src/overboard/hal/display_config.hpp
    @ONLY
)

# Embed keyboard.json as C++ resource
file(READ ${CMAKE_SOURCE_DIR}/data/configs/mf/keyboard.json JSON_CONTENT)
configure_file(
    ${CMAKE_SOURCE_DIR}/templates/embedded_json.hpp.in
    ${CMAKE_BINARY_DIR}/src/overboard/resources/embedded_json.hpp
    @ONLY
)

# Pi Zero-specific source files
set(HARDWARE_SOURCES
    src/overboard/hal/pi_zero/app.cpp
    src/overboard/hal/pi_zero/display_drm.cpp
    src/overboard/hal/pi_zero/input_linux.cpp
    src/overboard/gui/app_view.cpp
    src/overboard/gui/app_menu.cpp
    src/overboard/gui/calculator_app.cpp
    src/overboard/gui/footer_bar.cpp
    src/overboard/gui/function_key_popup.cpp
    src/overboard/gui/header_bar.cpp
    src/overboard/gui/key_mapping_info.cpp
    src/overboard/gui/lcd_section.cpp
    src/overboard/gui/lv_font_superscript.c
    src/overboard/gui/math_canvas.cpp
    src/overboard/gui/panel_manager.cpp
    src/overboard/gui/settings_page.cpp
    src/overboard/gui/status_page.cpp
)
