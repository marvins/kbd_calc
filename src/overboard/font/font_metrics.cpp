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
    ascent = static_cast<int>(ascent * factor);
    descent = static_cast<int>(descent * factor);
    em = static_cast<int>(em * factor);
    ex = static_cast<int>(ex * factor);
    for (auto& adv : advances) {
        adv = static_cast<int>(adv * factor);
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

} // namespace ovb::font
