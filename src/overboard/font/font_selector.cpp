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
        case core::Action_Code::CUBE_ROOT:  // ∛ (cube root symbol)
        case core::Action_Code::NTH_ROOT:   // nroot (ASCII)
        case core::Action_Code::POWER_2:    // ² (superscript)
        case core::Action_Code::POWER_3:    // ³ (superscript)
        case core::Action_Code::POWER_N:    // ⁿ (superscript)
        case core::Action_Code::PI:         // π (U+03C0)
        case core::Action_Code::EULER:      // e (ASCII — still safe)
        case core::Action_Code::PHI:        // φ (U+03C6)
        case core::Action_Code::TAU:        // τ (U+03C4)
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
    // ² (U+00B2): \xC2\xB2   - superscript two
    // ³ (U+00B3): \xC2\xB3   - superscript three
    // ⁿ (U+207F): \xE2\x81\xBF - superscript n
    // √ (U+221A): \xE2\x88\x9A - square root / radical sign
    // ∛ (U+221B): \xE2\x88\x9B - cube root
    // ∜ (U+221C): \xE2\x88\x9C - fourth root
    // π (U+03C0): \xCF\x80   - pi
    // φ (U+03C6): \xCF\x86   - phi
    // τ (U+03C4): \xCF\x84   - tau

    return display_text.find("\xC2\xB2") != std::string::npos ||      // ²  (U+00B2)
           display_text.find("\xC2\xB3") != std::string::npos ||      // ³  (U+00B3)
           display_text.find("\xE2\x81\xBF") != std::string::npos ||  // ⁿ  (U+207F)
           display_text.find("\xE2\x88\x9A") != std::string::npos ||  // √  (U+221A)
           display_text.find("\xE2\x88\x9B") != std::string::npos ||  // ∛  (U+221B)
           display_text.find("\xE2\x88\x9C") != std::string::npos ||  // ∜  (U+221C)
           display_text.find("\xCF\x80") != std::string::npos ||      // π  (U+03C0)
           display_text.find("\xCF\x86") != std::string::npos ||      // φ  (U+03C6)
           display_text.find("\xCF\x84") != std::string::npos;        // τ  (U+03C4)
}

} // namespace ovb::font
