cmake_minimum_required(VERSION 4.0)
project(calc_simulator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SIMULATOR "Build simulator target (requires SDL2)" ON)
option(BUILD_TESTS "Build unit tests" ON)

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

set(CORE_SOURCES
    src/overboard/core/keymap.cpp
    src/overboard/core/layer_manager.cpp
    src/overboard/core/keyboard_layout.cpp
    src/overboard/core/ast/ast.cpp
    src/overboard/core/expression.cpp
    src/overboard/core/calc_engine.cpp
    src/overboard/core/parser.cpp
    src/overboard/core/display_controller.cpp
    src/overboard/core/layout/engine.cpp
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
        src/overboard/core/ast/ast.cpp
        src/overboard/core/expression.cpp
        src/overboard/core/calc_engine.cpp
        src/overboard/core/parser.cpp
        src/overboard/core/layout/engine.cpp
    )

    target_include_directories(calc_tests PRIVATE src)
    target_link_libraries(calc_tests PRIVATE GTest::gtest_main project_warnings)
    include(GoogleTest)
    gtest_discover_tests(calc_tests)

    # Standalone math render test (outputs PNG from equation string)
    add_executable(math_render_test
        test/test_math_render.cpp
        src/overboard/core/ast/ast.cpp
        src/overboard/core/parser.cpp
        src/overboard/core/layout/engine.cpp
    )
    target_include_directories(math_render_test PRIVATE src)
    target_include_directories(math_render_test SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})
    target_link_libraries(math_render_test PRIVATE ${SDL2_LIBRARIES} PNG::PNG)
    # Disable strict warnings for this test to avoid SDL header issues
    target_compile_options(math_render_test PRIVATE -Wno-old-style-cast -Wno-sign-conversion)
endif()

if(SIMULATOR)
    add_executable(calc_sim
        src/main_sim.cpp
        src/overboard/hal/sdl/sdl_display.cpp
        src/overboard/hal/sdl/sdl_input.cpp
        src/overboard/hal/sdl/stdout_logger.cpp
        ${CORE_SOURCES}
    )

    target_include_directories(calc_sim PRIVATE src)
    target_include_directories(calc_sim SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})

    target_link_libraries(calc_sim PRIVATE
        ${SDL2_LIBRARIES}
        project_warnings
    )

    target_compile_definitions(calc_sim PRIVATE SIMULATOR=1)

    set_source_files_properties(
        src/main_sim.cpp
        src/overboard/hal/sdl/sdl_display.cpp
        src/overboard/hal/sdl/sdl_input.cpp
        PROPERTIES COMPILE_OPTIONS "-Wno-old-style-cast"
    )
endif()
