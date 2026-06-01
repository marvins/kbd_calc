/**
 * @file    result_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Result display implementation
 */
#pragma once

// Project Libraries
#include <overboard/display/i_result_display.hpp>

namespace ovb::display {

/**
 * @brief Standard result display
 *
 * Renders symbolic and numeric results with configurable
 * alignment and width limits.
 */
class Result_Display : public I_Result_Display {
    public:
        /**
         * @brief Default constructor
         */
        Result_Display();

        /**
         * @brief Display a result string
         */
        void show(I_Canvas&          canvas,
                  const std::string& result,
                  bool               is_symbolic) override;

        /**
         * @brief Clear the result display
         */
        void clear(I_Canvas& canvas) override;

        /**
         * @brief Set result alignment
         */
        void set_alignment(Result_Align align) override;

        /**
         * @brief Get current alignment
         */
        Result_Align alignment() const override;

        /**
         * @brief Set maximum display width
         */
        void set_max_width(std::size_t max_chars) override;

        /**
         * @brief Get maximum display width
         */
        std::size_t max_width() const override;

    private:
        Result_Align align_;       ///< Current alignment
        std::size_t  max_chars_;   ///< Maximum characters
        std::string  last_result_; ///< Last displayed result

        /**
         * @brief Truncate result if too long
         * @param result Original result string
         * @return Truncated string with ellipsis if needed
         */
        std::string truncate(const std::string& result) const;
};

} // namespace ovb::display

