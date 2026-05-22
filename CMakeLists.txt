cmake_minimum_required(VERSION 4.0)
project(calc_simulator C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SIMULATOR "Build simulator target (requires SDL2)" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Target device selection: SDL or SK30
set(TARGET_DEVICE "SDL" CACHE STRING "Target device (SDL, SK30)")
set_property(CACHE TARGET_DEVICE PROPERTY STRINGS SDL SK30)

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

find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)

# ── LVGL ──────────────────────────────────────────────────────────────────────
set(LV_BUILD_CONF_PATH ${CMAKE_SOURCE_DIR}/thirdparty/lv_conf.h CACHE PATH "LVGL config file")
add_subdirectory(thirdparty/lvgl)

set(CORE_SOURCES
    src/overboard/core/keymap.cpp
    src/overboard/core/layer_manager.cpp
    src/overboard/core/keyboard_layout.cpp
    src/overboard/core/display_controller.cpp
    src/overboard/math/ast/ast.cpp
    src/overboard/math/expression.cpp
    src/overboard/math/calc_engine.cpp
    src/overboard/math/parser.cpp
    src/overboard/math/layout/engine.cpp
)

# ── GoogleTest ────────────────────────────────────────────────────────────────
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(calc_tests
        test/test_ast.cpp
        test/test_parser.cpp
        test/test_render.cpp
        test/test_calc_engine.cpp
        test/test_expression.cpp
        test/test_layout.cpp
        src/overboard/core/keymap.cpp
        src/overboard/core/layer_manager.cpp
        src/overboard/math/ast/ast.cpp
        src/overboard/math/expression.cpp
        src/overboard/math/calc_engine.cpp
        src/overboard/math/parser.cpp
        src/overboard/math/layout/engine.cpp
    )

    target_include_directories(calc_tests PRIVATE src)
    target_link_libraries(calc_tests PRIVATE GTest::gtest_main project_warnings)
    include(GoogleTest)
    gtest_discover_tests(calc_tests)

    # Standalone math render test (outputs PNG from equation string)
    add_executable(math_render_test
        test/test_math_render.cpp
        src/overboard/math/ast/ast.cpp
        src/overboard/math/parser.cpp
        src/overboard/math/layout/engine.cpp
    )
    target_include_directories(math_render_test PRIVATE src)
    target_include_directories(math_render_test SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})
    target_link_libraries(math_render_test PRIVATE ${SDL2_LIBRARIES} PNG::PNG)
    # Disable strict warnings for this test to avoid SDL header issues
    target_compile_options(math_render_test PRIVATE -Wno-old-style-cast -Wno-sign-conversion)
endif()

if(SIMULATOR)
    # SDL Simulator target
    add_executable(calc_sim
        src/main.cpp
        src/overboard/hal/sdl/sdl_keyboard.cpp
        src/overboard/hal/sdl/sdl_keyboard_display.cpp
        src/overboard/hal/sdl/sdl_keyboard_view.cpp
        src/overboard/hal/sdl/sdl_lcd_display.cpp
        src/overboard/hal/sdl/sdl_input.cpp
        src/overboard/hal/sdl/sdl_keymap.cpp
        src/overboard/log/stdout_logger.cpp
        ${CORE_SOURCES}
    )

    target_include_directories(calc_sim PRIVATE src)

    #  Treating 3rd party as "SYSTEM" so we ignore compiler warnings.
    target_include_directories(calc_sim SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(calc_sim SYSTEM PRIVATE thirdparty/lvgl)

    target_link_libraries(calc_sim PRIVATE
        ${SDL2_LIBRARIES}
        lvgl
        project_warnings
    )

    target_compile_definitions(calc_sim PRIVATE
        SIMULATOR=1
        TARGET_SDL=1
    )

    set_source_files_properties(
        src/main.cpp
        src/overboard/hal/app_factory.cpp
        src/overboard/hal/sdl/sdl_keyboard.cpp
        src/overboard/hal/sdl/sdl_keyboard_display.cpp
        src/overboard/hal/sdl/sdl_keyboard_view.cpp
        src/overboard/hal/sdl/sdl_lcd_display.cpp
        src/overboard/hal/sdl/sdl_input.cpp
        src/overboard/hal/sdl/sdl_keymap.cpp
        PROPERTIES COMPILE_OPTIONS "-Wno-old-style-cast"
    )
endif()

# SK30 Embedded target (when not building simulator)
if(NOT SIMULATOR AND TARGET_DEVICE STREQUAL "SK30")
    add_executable(calc_firmware
        src/main.cpp
        src/overboard/hal/app_factory.cpp
        src/overboard/hal/sk30/sk30_app.cpp
        src/overboard/hal/sk30/sk30_input.cpp
        ${CORE_SOURCES}
    )

    target_include_directories(calc_firmware PRIVATE src)

    target_link_libraries(calc_firmware PRIVATE
        project_warnings
    )

    target_compile_definitions(calc_firmware PRIVATE
        TARGET_SK30=1
    )
endif()
