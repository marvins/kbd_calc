/**
 * @file    display_config.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Unified display configuration for all targets
 *
 * Single source of truth for LCD, keyboard, and merged display
 * dimensions across SDL and hardware targets.
 */
#pragma once

namespace ovb::hal {

/// Merged display dimensions
inline constexpr int FULL_WIDTH  { 400 };
inline constexpr int FULL_HEIGHT { 800 };

/// Keyboard display dimensions
inline constexpr int KBD_WIDTH  { 400 };
inline constexpr int KBD_HEIGHT { 300 };

/// LCD display dimensions
inline constexpr int LCD_WIDTH  { FULL_WIDTH };
inline constexpr int LCD_HEIGHT { FULL_HEIGHT - KBD_HEIGHT };

/// Math layout preview area (within LCD section)
inline constexpr int PREVIEW_MAX_WIDTH  { 160 };  ///< Fits within LCD_WIDTH
inline constexpr int PREVIEW_MAX_HEIGHT { 100 };
inline constexpr int PREVIEW_OFFSET_Y   { 80 };   ///< Vertical offset from bottom

/// Keyboard layout margins and spacing
inline constexpr int KEY_PAD      { 2 };   ///< Gap between keys
inline constexpr int KBD_HEADER_H { 20 };  ///< Keyboard header bar height
inline constexpr int KBD_MARGIN_L { 8 };   ///< Left margin
inline constexpr int KBD_MARGIN_T { 6 };   ///< Top margin (below header)

} // namespace ovb::hal
