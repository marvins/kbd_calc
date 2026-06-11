/**
 * @file    font_selector.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Font selection utilities for display rendering
 */
#pragma once

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/core/action_code.hpp>

namespace ovb::font {

/**
 * @brief Check if action code requires custom math font
 * @param code Action code
 * @return True if the display text requires lv_font_superscript (math symbols)
 */
bool requires_custom_font(core::Action_Code code);

/**
 * @brief Check if display text requires custom math font
 * @param display_text Display string (UTF-8 encoded)
 * @return True if the text contains math symbols (superscripts, radicals, etc.) requiring custom font
 */
bool requires_custom_font(const std::string& display_text);

} // namespace ovb::font
