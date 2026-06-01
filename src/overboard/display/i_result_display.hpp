/**
 * @file    i_result_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Result display interface
 *
 * Displays the evaluation result of the current expression.
 * Handles both symbolic results (e.g., "pi", "sqrt(2)") and
 * numeric approximations.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <string>

// Project Libraries
#include <overboard/display/i_canvas.hpp>

namespace ovb::display {

/**
 * @brief Result alignment options
 */
enum class Result_Align {
    Left,   ///< Align to left edge
    Center, ///< Center horizontally
    Right,  ///< Align to right edge (traditional calculator style)
};

/**
 * @brief Result display interface
 *
 * Renders the calculation result in the LCD section.
 * Supports symbolic and numeric output with configurable
 * alignment and truncation.
 */
class I_Result_Display {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_Result_Display() = default;

        /**
         * @brief Display a result string
         * @param canvas Target canvas
         * @param result Result string to display
         * @param is_symbolic true if symbolic (e.g., "pi"), false if numeric
         */
        virtual void show(I_Canvas&          canvas,
                          const std::string& result,
                          bool               is_symbolic = false) = 0;

        /**
         * @brief Clear the result display
         * @param canvas Target canvas
         */
        virtual void clear(I_Canvas& canvas) = 0;

        /**
         * @brief Set result alignment
         * @param align Alignment mode
         */
        virtual void set_alignment(Result_Align align) = 0;

        /**
         * @brief Get current alignment
         * @return Current alignment
         */
        virtual Result_Align alignment() const = 0;

        /**
         * @brief Set maximum display width for truncation
         * @param max_chars Maximum characters to display
         */
        virtual void set_max_width(std::size_t max_chars) = 0;

        /**
         * @brief Get maximum display width
         * @return Maximum character count
         */
        virtual std::size_t max_width() const = 0;
};

} // namespace ovb::display

