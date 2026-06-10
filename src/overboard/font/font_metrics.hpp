/**
 * @file    font_metrics.hpp
 * @author  Marvin Smith
 * @date    2026-05-25
 *
 * @brief   Font metrics for tying layout box sizes to actual font dimensions
 *
 * Font_Metrics captures the key vertical and horizontal measurements
 * from a font at a specific pixel size. These are used by the layout engine
 * to size boxes accurately, mirroring how TeX uses fontdimen values.
 */
#pragma once

// C++ Standard Libraries
#include <array>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::font {

/**
 * @brief Font metrics derived from a specific font at a specific size
 *
 * All measurements are in pixels. The layout engine uses these to
 * compute box dimensions that match what the renderer will actually draw.
 *
 * Terminology follows TeX/LaTeX conventions:
 * - em:      width of the 'M' glyph (horizontal scale unit)
 * - ex:      height of the 'x' glyph above baseline (vertical scale unit)
 * - ascent:  distance from baseline to top of tallest glyph
 * - descent: distance from baseline to bottom of deepest glyph (positive downward)
 */
struct Font_Metrics {
    static constexpr int ASCII_FIRST = 32;   ///< First measured codepoint (space)
    static constexpr int ASCII_LAST  = 126;  ///< Last measured codepoint (~)
    static constexpr int ASCII_COUNT = ASCII_LAST - ASCII_FIRST + 1;

    float base_px  = 16.0f;  ///< Font size in pixels this was measured at
    int   ascent   = 10;     ///< Pixels above baseline (scaled)
    int   descent  = 3;      ///< Pixels below baseline (scaled, positive = downward)
    int   em       = 8;      ///< Em width (advance of 'M') in pixels
    int   ex       = 7;      ///< Ex height (bbox height of 'x') in pixels
    int   line_gap = 0;      ///< Extra spacing between lines

    /// Per-glyph advance widths for printable ASCII, populated at startup.
    std::array<int, ASCII_COUNT> advances{};

    /**
     * @brief Total line height: ascent + descent
     */
    int line_height() const { return ascent + descent; }

    /**
     * @brief Advance width for a single character.
     *
     * Returns the measured advance for printable ASCII, or em/2 as fallback.
     */
    int char_advance(char c) const {
        int idx = static_cast<int>(c) - ASCII_FIRST;
        if (idx >= 0 && idx < ASCII_COUNT) {
            return advances[static_cast<size_t>(idx)];
        }
        return em / 2;
    }

    /**
     * @brief Total advance width for a string.
     */
    int string_width(const std::string& s) const {
        int w = 0;
        for (char c : s) { w += char_advance(c); }
        return w;
    }

    /**
     * @brief Fallback per-character width (em/2) used when advances not populated.
     */
    int char_width() const { return em / 2; }

    /**
     * @brief Scale all metrics by a factor
     * @param factor Scale factor (e.g., 2.0 for double size)
     * @return true if scaling succeeded, false otherwise
     */
    bool scale(float factor);

    /**
     * @brief Construct a reasonable default for use in tests or headless contexts.
     *
     * Based on rough pixel values for a ~10px font. Not tied to any real font.
     */
    static Font_Metrics make_default();

    /**
     * @brief Construct metrics for the 5x7 bitmap font.
     *
     * The 5x7 font is a fixed-size bitmap font where each character is
     * 5 pixels wide by 7 pixels tall. This is commonly used on embedded
     * displays and matches the HAL's FONT_5X7.
     */
    static Font_Metrics make_5x7();

    /**
     * @brief Construct metrics for a specific font size in pixels.
     *
     * Creates font metrics scaled to the requested pixel size.
     * @param font_size Font size in pixels
     * @return Font_Metrics scaled to the requested size
     */
    static Font_Metrics make_for_size(int font_size);

    /**
     * @brief Construct metrics by querying an LVGL font directly.
     *
     * Populates advance widths, ascent, and descent from the actual
     * lv_font_t so the layout engine always matches what lv_draw_label
     * will render — no per-font manual tuning required.
     *
     * @param font Pointer to an LVGL font descriptor
     * @return Font_Metrics populated from the font
     */
    static Font_Metrics make_from_lv_font(const lv_font_t* font);
};

} // namespace ovb::font
