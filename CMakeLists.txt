cmake_minimum_required(VERSION 4.0)

# Load version configuration
include(cmake/scripts/version.cmake)

#  Configure the CMake Project
project( calc_simulator
         LANGUAGES C CXX
         VERSION ${PROJECT_VERSION}
         DESCRIPTION "Calculator with keyboard-driven interface"
         HOMEPAGE_URL https://github.com/marvins/kbd_calc)

#  C++ Standard Configuration
set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

#  Options Configuration
option( SIMULATOR     "Build simulator target (requires SDL2)"  ON )
option( BUILD_TESTS   "Build unit tests"                        ON )

# Target device selection: SDL, PICOSDL, RP2350, or PICOCALC
set(TARGET_DEVICE "SDL" CACHE STRING "Target device (SDL, PICOSDL, RP2350, PICOCALC)")
set_property(CACHE TARGET_DEVICE PROPERTY STRINGS SDL PICOSDL RP2350 PICOCALC)

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

target_compile_definitions(${APP_TARGET} PRIVATE ${HARDWARE_COMPILE_DEFINITIONS})

if(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC")
    pico_add_extra_outputs(${APP_TARGET})
endif()

# ── Utility Apps (SDL-only) ─────────────────────────────────────────────────────────
if(TARGET_DEVICE STREQUAL "SDL" OR TARGET_DEVICE STREQUAL "PICOSDL")
    # Keylogger utility for mapping physical keys
    add_executable(keylogger
        src/tools/keylogger.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
    )
    target_include_directories(keylogger PRIVATE src)
    target_include_directories(keylogger SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(keylogger PRIVATE ${SDL2_LIBRARIES} project_warnings)

    # Test math rendering tool
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
if( BUILD_TESTS )
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
