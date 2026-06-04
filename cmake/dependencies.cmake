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
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Woverloaded-virtual
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Wundef
)

# ── nlohmann/json (header-only) ───────────────────────────────────────────────────────
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json SYSTEM INTERFACE thirdparty/json/include)

# ── LVGL ────────────────────────────────────────────────────────────────────────────
set(LV_BUILD_CONF_PATH ${CMAKE_SOURCE_DIR}/thirdparty/lv_conf.h CACHE PATH "LVGL config file")
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "Build LVGL examples" FORCE)
set(CONFIG_LV_BUILD_DEMOS    OFF CACHE BOOL "Build LVGL demos"    FORCE)
add_subdirectory(thirdparty/lvgl)

# Remove SDL sources for embedded targets (LVGL doesn't respect CONFIG_LV_USE_SDL without preprocessing)
if(TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC")
    get_target_property(LVGL_SOURCES lvgl SOURCES)
    if(LVGL_SOURCES)
        list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*src/drivers/sdl/.*")
        set_target_properties(lvgl PROPERTIES SOURCES "${LVGL_SOURCES}")
    endif()
endif()

# ── GoogleTest (if tests enabled) ────────────────────────────────────────────────────
# Only build tests for simulator targets, not embedded
if(BUILD_TESTS AND NOT (TARGET_DEVICE STREQUAL "RP2350" OR TARGET_DEVICE STREQUAL "PICOCALC"))
    add_subdirectory(thirdparty/gtest)
    enable_testing()
endif()
