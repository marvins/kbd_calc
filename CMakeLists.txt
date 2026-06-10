cmake_minimum_required(VERSION 4.0)

# Load version configuration
include(cmake/scripts/version.cmake)


#  Configure the CMake Project
project( calc_simulator
         LANGUAGES C CXX ASM
         VERSION ${PROJECT_VERSION}
         DESCRIPTION "Calculator with keyboard-driven interface"
         HOMEPAGE_URL https://github.com/marvins/kbd_calc)

# Initialize Pico SDK for embedded targets (after project(), before other setup)
if(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC")
    if(NOT DEFINED ENV{PICO_SDK_PATH})
        message(FATAL_ERROR "PICO_SDK_PATH environment variable not set for embedded target")
    endif()
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    # Set platform/board BEFORE pico_sdk_init()
    set(PICO_PLATFORM rp2350 CACHE STRING "Pico platform" FORCE)
    set(PICO_BOARD pico2 CACHE STRING "Pico board" FORCE)
    set(PICO_NO_PICOTOOL 1 CACHE BOOL "" FORCE)
    include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
    pico_sdk_init()
endif()

#  C++ Standard Configuration
set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

#  Options Configuration
option( SIMULATOR     "Build simulator target (requires SDL2)"  ON )
option( BUILD_TESTS   "Build unit tests"                        ON )
option( SHOW_KEYBOARD_UI "Show on-screen keyboard UI (SDL only)" ON )

# Target device selection: SDL, PICOSDL, RP2350, PICOCALC, or ZERO
if(NOT DEFINED TARGET_DEVICE OR TARGET_DEVICE STREQUAL "")
    set(TARGET_DEVICE "SDL" CACHE STRING "Target device (SDL, PICOSDL, RP2350, PICOCALC, ZERO)" FORCE)
else()
    set(TARGET_DEVICE ${TARGET_DEVICE} CACHE STRING "Target device (SDL, PICOSDL, RP2350, PICOCALC, ZERO)" FORCE)
endif()
set_property(CACHE TARGET_DEVICE PROPERTY STRINGS SDL PICOSDL RP2350 PICOCALC ZERO)

# Set keyboard UI default based on target
if(TARGET_DEVICE STREQUAL "SDL")
    set(SHOW_KEYBOARD_UI ON CACHE BOOL "Show on-screen keyboard UI" FORCE)
elseif(TARGET_DEVICE STREQUAL "PICOSDL")
    set(SHOW_KEYBOARD_UI ON CACHE BOOL "Show on-screen keyboard UI" FORCE)
elseif(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC")
    set(SHOW_KEYBOARD_UI OFF CACHE BOOL "Show on-screen keyboard UI" FORCE)
elseif(TARGET_DEVICE STREQUAL "ZERO")
    set(SHOW_KEYBOARD_UI ON CACHE BOOL "Show on-screen keyboard UI" FORCE)
endif()

# ── Shared Dependencies ───────────────────────────────────────────────────────────
include(cmake/dependencies.cmake)

# ── Hardware Configuration ────────────────────────────────────────────────────────
if(TARGET_DEVICE STREQUAL "SDL")
    include(data/hardware/sdl/CMakeLists.txt)
elseif(TARGET_DEVICE STREQUAL "PICOSDL")
    include(data/hardware/picosdl/CMakeLists.txt)
elseif(TARGET_DEVICE STREQUAL "RP2350")
    include(data/hardware/rp2350/CMakeLists.txt)
elseif(TARGET_DEVICE STREQUAL "PICOCALC")
    include(data/hardware/picocalc/CMakeLists.txt)
elseif(TARGET_DEVICE STREQUAL "ZERO")
    include(data/hardware/pi_zero/CMakeLists.txt)
else()
    message(FATAL_ERROR "Unknown TARGET_DEVICE: ${TARGET_DEVICE}")
endif()

# ── Common Sources ────────────────────────────────────────────────────────────────
set(CORE_SOURCES
    src/overboard/core/config.cpp
    src/overboard/core/keymap.cpp
    src/overboard/core/action_code.cpp
    src/overboard/core/input_key.cpp
    src/overboard/core/keyboard_layout.cpp
    src/overboard/core/layer_manager.cpp
    src/overboard/display/cursor_renderer.cpp
    src/overboard/display/dirty_region_tracker.cpp
    src/overboard/display/history_display.cpp
    src/overboard/display/result_display.cpp
    src/overboard/font/font_metrics.cpp
    src/overboard/io/keyboard_config.cpp
    src/overboard/io/via_layout.cpp
    src/overboard/math/ast/binary_op_node.cpp
    src/overboard/math/ast/function_node.cpp
    src/overboard/math/ast/number_node.cpp
    src/overboard/math/ast/unary_op_node.cpp
    src/overboard/math/calc_engine.cpp
    src/overboard/math/expression.cpp
    src/overboard/math/layout/engine.cpp
    src/overboard/math/parser.cpp
)

# ── Build Main Application ─────────────────────────────────────────────────────────
if(TARGET_DEVICE STREQUAL "SDL" OR TARGET_DEVICE STREQUAL "PICOSDL")
    set(APP_TARGET calc_sim)
    set(APP_LIBRARIES ${HARDWARE_LIBRARIES} lvgl)
elseif(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC")
    set(APP_TARGET calc_firmware)
    set(APP_LIBRARIES ${HARDWARE_LIBRARIES} lvgl)
elseif(TARGET_DEVICE STREQUAL "ZERO")
    set(APP_TARGET calc_app)
    set(APP_LIBRARIES lvgl ${HARDWARE_LIBRARIES})
endif()

#  Build the main executable
add_executable(${APP_TARGET}
    src/main.cpp
    src/overboard/hal/app_factory.cpp
    ${HARDWARE_SOURCES}
    src/overboard/log/stdout_logger.cpp
    src/overboard/log/log_level.cpp
    ${CORE_SOURCES}
)

target_include_directories( ${APP_TARGET} PRIVATE src )
target_include_directories( ${APP_TARGET} PRIVATE ${CMAKE_BINARY_DIR}/src )
target_include_directories( ${APP_TARGET} SYSTEM PRIVATE thirdparty/lvgl )
target_include_directories( ${APP_TARGET} SYSTEM PRIVATE thirdparty/json/include )
target_include_directories( ${APP_TARGET} SYSTEM PRIVATE ${HARDWARE_INCLUDE_DIRS} )

target_link_libraries(${APP_TARGET} PRIVATE
    ${APP_LIBRARIES}
    project_warnings
    nlohmann_json
)

if(SHOW_KEYBOARD_UI)
    target_compile_definitions(${APP_TARGET} PRIVATE ${HARDWARE_COMPILE_DEFINITIONS} SHOW_KEYBOARD_UI=1)
else()
    target_compile_definitions(${APP_TARGET} PRIVATE ${HARDWARE_COMPILE_DEFINITIONS} SHOW_KEYBOARD_UI=0)
endif()

if(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC")
    pico_add_extra_outputs(${APP_TARGET})
endif()

# ── Keylogger Utility ────────────────────────────────────────────────────────────────
# Enabled per-target via hardware-specific CMakeLists.txt
if(BUILD_KEYLOGGER)
    set(KEYLOGGER_SOURCES
        src/tools/keylogger.cpp
        src/overboard/core/input_key.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
    )

    add_executable(keylogger ${KEYLOGGER_SOURCES})
    target_include_directories(keylogger PRIVATE src)

    # Add target-specific sources and defines
    if(KEYLOGGER_USE_SDL)
        target_sources(keylogger PRIVATE
            src/overboard/hal/sdl/input.cpp
            src/overboard/hal/sdl/input_key_mapping.cpp
            src/overboard/hal/sdl/keymap.cpp
        )
        target_include_directories(keylogger SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(keylogger PRIVATE ${SDL2_LIBRARIES})
        target_compile_definitions(keylogger PRIVATE TARGET_SDL=1)
    else()
        # Pi Zero / evdev version
        target_sources(keylogger PRIVATE src/overboard/hal/pi_zero/input_linux.cpp)
        target_compile_definitions(keylogger PRIVATE TARGET_ZERO=1)
    endif()

    target_link_libraries(keylogger PRIVATE project_warnings)
endif()

# ── Test Math Rendering Tool (SDL-only) ───────────────────────────────────────────────
if(TARGET_DEVICE STREQUAL "SDL" OR TARGET_DEVICE STREQUAL "PICOSDL")
    add_executable(test_math_render
        src/overboard/apps/test_math_render/test_math_render.cpp
        src/overboard/apps/test_math_render/config.cpp
        src/overboard/font/font_metrics.cpp
        src/overboard/math/ast/binary_op_node.cpp
        src/overboard/math/ast/function_node.cpp
        src/overboard/math/ast/number_node.cpp
        src/overboard/math/ast/unary_op_node.cpp
        src/overboard/math/parser.cpp
        src/overboard/math/layout/engine.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
    )
    target_include_directories(test_math_render PRIVATE src)
    target_include_directories(test_math_render PRIVATE thirdparty/stb)
    target_include_directories(test_math_render SYSTEM PRIVATE ${PNG_INCLUDE_DIRS})
    target_link_libraries(test_math_render PRIVATE PNG::PNG)
    target_compile_options(test_math_render PRIVATE -Wno-old-style-cast -Wno-sign-conversion)

    # VIA layout renderer tool
    add_executable(render_layout
        src/overboard/core/keyboard_layout.cpp
        src/overboard/io/via_layout.cpp
        src/overboard/log/log_level.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/apps/ovt_layout_util/config.cpp
        src/overboard/apps/ovt_layout_util/main.cpp
    )
    target_include_directories(render_layout PRIVATE src)
    target_include_directories(render_layout SYSTEM PRIVATE thirdparty/json/include ${PNG_INCLUDE_DIRS})
    target_link_libraries(render_layout PRIVATE nlohmann_json PNG::PNG project_warnings)
endif()

# ── Tests (shared) ────────────────────────────────────────────────────────────────
# Tests only for simulator targets (gtest doesn't build for embedded ARM or Pi Zero)
if( BUILD_TESTS AND NOT (TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC" OR TARGET_DEVICE STREQUAL "ZERO") )
    add_executable(calc_tests
        test/test_main.cpp
        test/math/ast/test_ast.cpp
        test/math/test_parser.cpp
        test/math/layout/test_render.cpp
        test/math/test_calc_engine.cpp
        test/math/test_expression.cpp
        test/core/test_layout.cpp
        test/core/test_action_code.cpp
        test/core/test_input_key.cpp
        src/overboard/core/keymap.cpp
        src/overboard/core/action_code.cpp
        src/overboard/core/input_key.cpp
        src/overboard/core/layer_manager.cpp
        src/overboard/font/font_metrics.cpp
        src/overboard/math/ast/binary_op_node.cpp
        src/overboard/math/ast/function_node.cpp
        src/overboard/math/ast/number_node.cpp
        src/overboard/math/ast/unary_op_node.cpp
        src/overboard/math/expression.cpp
        src/overboard/math/calc_engine.cpp
        src/overboard/math/parser.cpp
        src/overboard/math/layout/engine.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
    )

    target_include_directories(calc_tests PRIVATE src test)
    target_include_directories(calc_tests PRIVATE ${CMAKE_BINARY_DIR}/src)
    target_include_directories(calc_tests SYSTEM PRIVATE thirdparty/lvgl)
    target_include_directories(calc_tests SYSTEM PRIVATE thirdparty/json/include)
    target_link_libraries(calc_tests PRIVATE GTest::gtest project_warnings lvgl)
    include(GoogleTest)
    gtest_discover_tests(calc_tests)
endif()
