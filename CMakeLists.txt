cmake_minimum_required(VERSION 4.0)
project(calc_simulator C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SIMULATOR "Build simulator target (requires SDL2)" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Target device selection: SDL or RP2350
set(TARGET_DEVICE "SDL" CACHE STRING "Target device (SDL, RP2350)")
set_property(CACHE TARGET_DEVICE PROPERTY STRINGS SDL RP2350)

# ── Strict warning flags ───────────────────────────────────────────────────────
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Woverloaded-virtual
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
)

# Include hardware-specific configuration
if(TARGET_DEVICE STREQUAL "SDL")
    include(data/hardware/sdl/CMakeLists.txt)
elseif(TARGET_DEVICE STREQUAL "RP2350")
    include(data/hardware/rp2350/CMakeLists.txt)
else()
    message(FATAL_ERROR "Unknown TARGET_DEVICE: ${TARGET_DEVICE}")
endif()

# ── nlohmann/json (header-only) ───────────────────────────────────────────────────
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json SYSTEM INTERFACE thirdparty/json/include)

# ── LVGL ──────────────────────────────────────────────────────────────────────
set(LV_BUILD_CONF_PATH ${CMAKE_SOURCE_DIR}/thirdparty/lv_conf.h CACHE PATH "LVGL config file")
add_subdirectory(thirdparty/lvgl)

if(TARGET lvgl AND SDL2_INCLUDE_DIRS)
    target_include_directories(lvgl SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

set(CORE_SOURCES
    src/overboard/core/config.cpp
    src/overboard/core/display_controller.cpp
    src/overboard/core/keymap.cpp
    src/overboard/core/keyboard_layout.cpp
    src/overboard/core/layer_manager.cpp
    src/overboard/font/font_metrics.cpp
    src/overboard/io/via_layout.cpp
    src/overboard/math/ast/ast.cpp
    src/overboard/math/calc_engine.cpp
    src/overboard/math/expression.cpp
    src/overboard/math/layout/engine.cpp
    src/overboard/math/parser.cpp
)

# ── GoogleTest ────────────────────────────────────────────────────────────────
if(BUILD_TESTS)
    add_subdirectory(thirdparty/gtest)

    enable_testing()

    add_executable(calc_tests
        test/math/ast/test_ast.cpp
        test/math/test_parser.cpp
        test/math/layout/test_render.cpp
        test/math/test_calc_engine.cpp
        test/math/test_expression.cpp
        test/core/test_layout.cpp
        test/core/test_keymap.cpp
        src/overboard/core/keymap.cpp
        src/overboard/core/layer_manager.cpp
        src/overboard/font/font_metrics.cpp
        src/overboard/math/ast/ast.cpp
        src/overboard/math/expression.cpp
        src/overboard/math/calc_engine.cpp
        src/overboard/math/parser.cpp
        src/overboard/math/layout/engine.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
    )

    target_include_directories(calc_tests PRIVATE src test)
    target_include_directories(calc_tests SYSTEM PRIVATE thirdparty/json/include)
    target_link_libraries(calc_tests PRIVATE GTest::gtest_main project_warnings)
    include(GoogleTest)
    gtest_discover_tests(calc_tests)

endif()

if(TARGET_DEVICE STREQUAL "SDL")
    # SDL Simulator target
    add_executable(calc_sim
        src/main.cpp
        src/overboard/hal/app_factory.cpp
        ${HARDWARE_SOURCES}
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
        ${CORE_SOURCES}
    )

    target_include_directories(calc_sim PRIVATE src)
    target_include_directories(calc_sim SYSTEM PRIVATE thirdparty/lvgl)
    target_include_directories(calc_sim SYSTEM PRIVATE thirdparty/json/include)
    target_include_directories(calc_sim SYSTEM PRIVATE ${HARDWARE_INCLUDE_DIRS})

    target_link_libraries(calc_sim PRIVATE
        ${HARDWARE_LIBRARIES}
        lvgl
        project_warnings
        nlohmann_json
    )

    target_compile_definitions(calc_sim PRIVATE ${HARDWARE_COMPILE_DEFINITIONS})

    # Keylogger utility for mapping physical keys (SDL only)
    add_executable(keylogger
        src/tools/keylogger.cpp
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
    )
    target_include_directories(keylogger PRIVATE src)
    target_include_directories(keylogger SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})

    # Test math rendering tool (SDL only)
    add_executable(test_math_render
        src/overboard/apps/test_math_render/test_math_render.cpp
        src/overboard/apps/test_math_render/config.cpp
        src/overboard/font/font_metrics.cpp
        src/overboard/math/ast/ast.cpp
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
    target_link_libraries(keylogger PRIVATE ${SDL2_LIBRARIES} project_warnings)

    # VIA layout renderer tool (SDL only)
    add_executable(render_layout
        src/overboard/core/keyboard_layout.cpp
        src/overboard/io/via_layout.cpp
        src/overboard/log/log_level.cpp
        src/overboard/log/stdout_logger.cpp
        src/tools/render_layout.cpp
    )
    target_include_directories(render_layout PRIVATE src)
    target_include_directories(render_layout SYSTEM PRIVATE thirdparty/json/include ${PNG_INCLUDE_DIRS})
    target_link_libraries(render_layout PRIVATE nlohmann_json PNG::PNG project_warnings)
endif()

# RP2350 Embedded target
if(TARGET_DEVICE STREQUAL "RP2350")
    add_executable(calc_firmware
        src/main.cpp
        src/overboard/hal/app_factory.cpp
        ${HARDWARE_SOURCES}
        src/overboard/log/stdout_logger.cpp
        src/overboard/log/log_level.cpp
        ${CORE_SOURCES}
    )

    target_include_directories(calc_firmware PRIVATE src)
    target_include_directories(calc_firmware SYSTEM PRIVATE ${HARDWARE_INCLUDE_DIRS})

    target_link_libraries(calc_firmware PRIVATE
        ${HARDWARE_LIBRARIES}
        project_warnings
        nlohmann_json
    )

    target_compile_definitions(calc_firmware PRIVATE ${HARDWARE_COMPILE_DEFINITIONS})

    # Enable USB booting for RP2350
    pico_add_extra_outputs(calc_firmware)
endif()
