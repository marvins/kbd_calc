# Shared dependencies
# This file sets up dependencies that are used across all targets

# ── Project Warnings ───────────────────────────────────────────────────────────────
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wcast-align
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Wundef
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

# Remove SDL sources for embedded targets (LVGL doesn't respect CONFIG_LV_USE_SDL without preprocessing)
if(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC" OR TARGET_DEVICE STREQUAL "ZERO")
    get_target_property(LVGL_SOURCES lvgl SOURCES)
    if(LVGL_SOURCES)
        list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*src/drivers/sdl/.*")
        set_target_properties(lvgl PROPERTIES SOURCES "${LVGL_SOURCES}")
    endif()
endif()

# ── GoogleTest (if tests enabled) ────────────────────────────────────────────────────
# Only build tests for simulator targets, not embedded
if(BUILD_TESTS AND NOT (TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC" OR TARGET_DEVICE STREQUAL "ZERO"))
    add_subdirectory(thirdparty/gtest)
    enable_testing()
endif()
