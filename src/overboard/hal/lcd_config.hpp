#pragma once

/**
 * @brief LCD display configuration constants
 *
 * Hardware-specific dimensions and layout parameters for the
 * calculator LCD display. These values are used throughout the
 * codebase for positioning and sizing UI elements.
 */

namespace ovb::hal {

/// LCD display width in pixels
inline constexpr int LCD_WIDTH  = 480;

/// LCD display height in pixels
inline constexpr int LCD_HEIGHT = 240;

/// Math layout preview area dimensions
inline constexpr int PREVIEW_MAX_WIDTH  = 200;  ///< Maximum width for expression preview
inline constexpr int PREVIEW_MAX_HEIGHT = 100;  ///< Maximum height for expression preview

/// Preview positioning (bottom center of display)
inline constexpr int PREVIEW_OFFSET_Y = 80;     ///< Vertical offset from bottom edge

} // namespace ovb::hal
