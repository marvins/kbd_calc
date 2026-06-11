/**
 * @file    font_selector.cpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Font selection utilities for display rendering
 */

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/font/font_selector.hpp>
#include <overboard/core/action_code.hpp>

namespace ovb::font {

/********************************************************/
/*      Check if Action Requires Custom Math Font      */
/********************************************************/
bool requires_custom_font(core::Action_Code code) {
    switch (code) {
        case core::Action_Code::SQRT:       // √ (radical symbol)
        case core::Action_Code::POWER_2:    // ² (superscript)
        case core::Action_Code::POWER_3:    // ³ (superscript)
        case core::Action_Code::POWER_N:    // ⁿ (superscript)
            return true;
        default:
            return false;
    }
}

/********************************************************/
/*    Check if Display Text Requires Custom Font       */
/********************************************************/
bool requires_custom_font(const std::string& display_text) {
    // Check for UTF-8 sequences of math symbols (superscripts, radicals, etc.)
    // ² (U+00B2): \xC2\xB2 - superscript two
    // ³ (U+00B3): \xC2\xB³ - superscript three
    // ⁿ (U+207F): \xE2\x81\xBF - superscript n
    // √ (U+221A): \xE2\x88\x9A - square root / radical sign
    
    return display_text.find("\xC2\xB2") != std::string::npos ||      // ²
           display_text.find("\xC2\xB3") != std::string::npos ||      // ³
           display_text.find("\xE2\x81\xBF") != std::string::npos ||  // ⁿ
           display_text.find("\xE2\x88\x9A") != std::string::npos;    // √
}

} // namespace ovb::font
