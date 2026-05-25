/**
 * @file lvgl_theme.hpp
 * @author Marvin Smith
 * @date 2026-05-22
 * @brief LVGL theme constants and color definitions
 *
 * Centralized theme configuration for light mode LVGL displays.
 * Future: Load from config profile on boot.
 */
#pragma once

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::hal::sdl {

// Background colors
constexpr uint32_t LVGL_COLOR_BG_SCREEN      = 0xF0F0F0;  // Main screen background
constexpr uint32_t LVGL_COLOR_BG_BEZEL       = 0xFFFFFF;  // Bezel/container background
constexpr uint32_t LVGL_COLOR_BG_PREVIEW     = 0xF8F8F8;  // Preview area background
constexpr uint32_t LVGL_COLOR_BG_TABLE       = 0xFFFFFF;  // Table background
constexpr uint32_t LVGL_COLOR_BG_CANVAS      = 0xF8F8F8;  // Canvas clear color

// Text colors
constexpr uint32_t LVGL_COLOR_TEXT_PRIMARY   = 0x333333;  // Main text (dark gray)
constexpr uint32_t LVGL_COLOR_TEXT_SECONDARY = 0x666666;  // Header/secondary text
constexpr uint32_t LVGL_COLOR_TEXT_MUTED     = 0x808080;  // Muted/disabled text

// Accent colors
constexpr uint32_t LVGL_COLOR_ACCENT_GREEN   = 0x32C832;  // Mode indicator (MATH)
constexpr uint32_t LVGL_COLOR_ACCENT_BLUE    = 0x6496FF;  // Layer indicator, pressed keys

// Border colors
constexpr uint32_t LVGL_COLOR_BORDER_LIGHT   = 0xE0E0E0;  // Subtle borders
constexpr uint32_t LVGL_COLOR_BORDER_MEDIUM  = 0xD0D0D0;  // Bezel border
constexpr uint32_t LVGL_COLOR_BORDER_DARK    = 0xCCCCCC;  // Button borders

// Shadow
constexpr uint32_t LVGL_COLOR_SHADOW         = 0x808080;  // Shadow color
constexpr uint8_t  LVGL_SHADOW_OPA            = LV_OPA_20;   // Shadow opacity

// Default fonts
#define LVGL_FONT_DEFAULT       &lv_font_montserrat_14
#define LVGL_FONT_SMALL         &lv_font_montserrat_12
#define LVGL_FONT_LARGE         &lv_font_montserrat_48

// Keyboard colors (light mode)
constexpr uint32_t LVGL_COLOR_KBD_BG         = 0xF5F5F5;  // Keyboard background
constexpr uint32_t LVGL_COLOR_KBD_HEADER     = 0xE8E8F0;  // Header bar
constexpr uint32_t LVGL_COLOR_KBD_BUTTON        = 0xFFFFFF;  // Default button
constexpr uint32_t LVGL_COLOR_KBD_BUTTON_HOVER   = 0xEAEEFF;  // Hovered button (subtle tint)
constexpr uint32_t LVGL_COLOR_KBD_BUTTON_PRESSED = 0x6496FF;  // Pressed button

// Helper to convert hex to lv_color_t
inline lv_color_t lvgl_color(uint32_t hex) {
    return lv_color_hex(hex);
}

} // namespace ovb::hal::sdl
