# SDL Simulator hardware configuration
# This file contains SDL-specific build settings for the simulator target.
# It is included by the main CMakeLists.txt when TARGET_DEVICE=SDL.

# SDL2 and PNG are required for the simulator
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)

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

# SDL-specific source files
set(HARDWARE_SOURCES
    src/overboard/hal/sdl/app.cpp
    src/overboard/hal/sdl/display.cpp
    src/overboard/hal/sdl/input.cpp
    src/overboard/hal/sdl/keymap.cpp
    src/overboard/gui/keyboard_view.cpp
    src/overboard/gui/lcd_section.cpp
)
