/**
 * @file    font_metrics.cpp
 * @author  Marvin Smith
 * @date    5/25/2026
 *
 * @brief   Font metrics implementation
 */
#include <overboard/font/font_metrics.hpp>

namespace ovb::font {

/*****************************************************************/
/*          Scale all metrics by a factor                        */
/*****************************************************************/
bool Font_Metrics::scale(float factor) {
    base_px *= factor;
    ascent  = static_cast<int>(static_cast<float>(ascent)  * factor);
    descent = static_cast<int>(static_cast<float>(descent) * factor);
    em      = static_cast<int>(static_cast<float>(em)      * factor);
    ex      = static_cast<int>(static_cast<float>(ex)      * factor);
    for (auto& adv : advances) {
        adv = static_cast<int>(static_cast<float>(adv) * factor);
    }
    return true;
}

/*****************************************************************/
/*          Construct default font metrics                       */
/*****************************************************************/
Font_Metrics Font_Metrics::make_default() {
    Font_Metrics m;
    m.base_px  = 10.0f;
    m.ascent   = 8;
    m.descent  = 2;
    m.em       = 8;
    m.ex       = 5;
    m.line_gap = 0;
    m.advances.fill(5);  // Reasonable monospace fallback
    return m;
}

/*****************************************************************/
/*          Construct 5x7 bitmap font metrics                     */
/*****************************************************************/
Font_Metrics Font_Metrics::make_5x7() {
    Font_Metrics m;
    m.base_px  = 7.0f;  // 5x7 font is 7 pixels tall
    m.ascent   = 5;     // 5 pixels above baseline
    m.descent  = 2;     // 2 pixels below baseline
    m.em       = 5;     // 5 pixels wide (monospace)
    m.ex       = 5;     // x-height approx 5
    m.line_gap = 0;

    // All characters are 5 pixels wide (monospace)
    m.advances.fill(5);
    return m;
}

/*****************************************************************/
/*          Construct metrics for specific font size             */
/*****************************************************************/
Font_Metrics Font_Metrics::make_for_size(int font_size) {
    // Start with 10px base metrics and scale to requested size
    Font_Metrics m = make_default();
    float scale_factor = static_cast<float>(font_size) / 10.0f;
    m.scale(scale_factor);
    return m;
}

/*****************************************************************/
/*      Construct metrics from an LVGL font descriptor          */
/*****************************************************************/
Font_Metrics Font_Metrics::make_from_lv_font(const lv_font_t* font) {
    Font_Metrics m;
    m.base_px  = static_cast<float>(font->line_height);
    m.ascent   = font->line_height - font->base_line;
    m.descent  = font->base_line;
    m.line_gap = 0;

    // Query advance width for every printable ASCII character
    lv_font_glyph_dsc_t dsc{};
    for (int c = ASCII_FIRST; c <= ASCII_LAST; ++c) {
        const size_t idx = static_cast<size_t>(c - ASCII_FIRST);
        if (lv_font_get_glyph_dsc(font, &dsc, static_cast<uint32_t>(c), 0)) {
            m.advances[idx] = static_cast<int>(dsc.adv_w);
        } else {
            m.advances[idx] = font->line_height / 2;  // fallback
        }
    }

    // Derive em and ex from measured glyphs
    lv_font_glyph_dsc_t em_dsc{};
    m.em = lv_font_get_glyph_dsc(font, &em_dsc, 'M', 0)
           ? static_cast<int>(em_dsc.adv_w) : font->line_height;
    lv_font_glyph_dsc_t ex_dsc{};
    m.ex = lv_font_get_glyph_dsc(font, &ex_dsc, 'x', 0)
           ? static_cast<int>(ex_dsc.box_h) : m.ascent * 2 / 3;

    return m;
}

} // namespace ovb::font
