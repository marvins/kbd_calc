# SDL Simulator hardware configuration
# This file contains SDL-specific build settings for the simulator target.
# It is included by the main CMakeLists.txt when TARGET_DEVICE=SDL.

# SDL2 and PNG are required for the simulator
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)

# LVGL SDL2 include fix
if(NOT SDL2_INCLUDE_DIRS)
    get_target_property(SDL2_INCLUDE_DIRS SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
endif()
if(SDL2_INCLUDE_DIRS)
    target_include_directories(lvgl SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# Set up SDL-specific compile definitions
set(HARDWARE_COMPILE_DEFINITIONS
    TARGET_SDL=1
    SIMULATOR=1
)

# SDL-specific libraries
set(HARDWARE_LIBRARIES
    ${SDL2_LIBRARIES}
    PNG::PNG
)

# SDL-specific include directories
set(HARDWARE_INCLUDE_DIRS
    ${SDL2_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

# Display dimensions for the SDL simulator (480x800, keyboard 480x300)
set(DISPLAY_FULL_WIDTH  480)
set(DISPLAY_FULL_HEIGHT 800)
set(DISPLAY_KBD_WIDTH   480)
set(DISPLAY_KBD_HEIGHT  300)
set(DISPLAY_PREVIEW_MAX_WIDTH  464)  # LCD_WIDTH - 16 (padding)
set(DISPLAY_PREVIEW_MAX_HEIGHT 80)   # Standard entry height
set(DISPLAY_PREVIEW_OFFSET_Y    0)    # No offset, aligned to bottom
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

# SDL-specific source files
set(HARDWARE_SOURCES
    src/overboard/hal/sdl/app.cpp
    src/overboard/hal/sdl/display.cpp
    src/overboard/hal/sdl/input.cpp
    src/overboard/hal/sdl/keymap.cpp
    src/overboard/gui/app_view.cpp
    src/overboard/gui/keyboard_display.cpp
    src/overboard/gui/lcd_section.cpp
    src/overboard/gui/math_canvas.cpp
    src/overboard/gui/lv_font_superscript.c
)

