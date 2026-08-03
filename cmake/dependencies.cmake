# Shared dependencies
# This file sets up dependencies that are used across all targets

# ── Project Warnings ───────────────────────────────────────────────────────────────
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    -Wall
    -Wextra
    $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
    -Werror
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wcast-align
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    # Explicit float conversion warnings
    $<$<CXX_COMPILER_ID:GNU>:-Wfloat-conversion>
    # Clang: -Wfloat-conversion covers most cases but -Wimplicit-int-float-conversion
    # is a separate Clang-only flag not implied by -Wfloat-conversion
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wfloat-conversion>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wimplicit-int-float-conversion>
    # C++-only warnings
    $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
)

# ── nlohmann/json (header-only) ───────────────────────────────────────────────────────
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json SYSTEM INTERFACE thirdparty/json/include)

# ── toml++ (header-only) ──────────────────────────────────────────────────────────────
add_library(tomlplusplus INTERFACE)
target_include_directories(tomlplusplus SYSTEM INTERFACE thirdparty/tomlplusplus/include)

# ── LVGL ────────────────────────────────────────────────────────────────────────────
set(LV_BUILD_CONF_PATH ${CMAKE_SOURCE_DIR}/thirdparty/lv_conf.h CACHE PATH "LVGL config file")
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "Build LVGL examples" FORCE)
set(CONFIG_LV_BUILD_DEMOS    OFF CACHE BOOL "Build LVGL demos"    FORCE)

add_subdirectory(thirdparty/lvgl)

# Enable DRM driver for Pi Zero by directly setting compile definitions
if(TARGET_DEVICE STREQUAL "ZERO")
    target_compile_definitions(lvgl PUBLIC LV_USE_LINUX_DRM=1 LV_USE_SDL=0)
    # Add DRM include paths for LVGL C files
    find_path(DRM_INCLUDE_DIR drm/drm.h PATHS /usr/include /usr/local/include)
    if(DRM_INCLUDE_DIR)
        target_include_directories(lvgl SYSTEM PUBLIC ${DRM_INCLUDE_DIR} ${DRM_INCLUDE_DIR}/drm)
    endif()
endif()

# Configure LVGL for SDL-based targets (SDL, PICOSDL, WASM)
if(TARGET_DEVICE STREQUAL "SDL" OR TARGET_DEVICE STREQUAL "PICOSDL" OR TARGET_DEVICE STREQUAL "WASM")
    # Enable LVGL SDL driver
    target_compile_definitions(lvgl PUBLIC LV_USE_SDL=1 CONFIG_LV_USE_SDL=1)
endif()

# Configure LVGL for WASM/Emscripten SDL2 port
if(TARGET_DEVICE STREQUAL "WASM")
    # Add -sUSE_SDL=2 to LVGL's compile flags so Emscripten provides SDL2 headers
    target_compile_options(lvgl PUBLIC -sUSE_SDL=2)
    target_link_options(lvgl PUBLIC -sUSE_SDL=2)
endif()

# Remove SDL sources for embedded targets (LVGL doesn't respect CONFIG_LV_USE_SDL without preprocessing)
if(TARGET_DEVICE STREQUAL "PICOCALC")
    target_compile_definitions(lvgl PUBLIC TARGET_RP2350=1)
endif()

if(TARGET_DEVICE STREQUAL "PICOCALC" OR TARGET_DEVICE STREQUAL "ZERO")
    get_target_property(LVGL_SOURCES lvgl SOURCES)
    if(LVGL_SOURCES)
        list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*src/drivers/sdl/.*")
        set_target_properties(lvgl PROPERTIES SOURCES "${LVGL_SOURCES}")
    endif()
endif()

# ── GoogleTest (if tests enabled) ────────────────────────────────────────────────────
# Only build tests for native simulator targets, not embedded or WASM
if(BUILD_TESTS AND NOT (TARGET_DEVICE STREQUAL "PICOCALC" OR TARGET_DEVICE STREQUAL "ZERO" OR TARGET_DEVICE STREQUAL "WASM"))
    add_subdirectory(thirdparty/gtest)
    enable_testing()
endif()
