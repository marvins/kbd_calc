/**
 * @file    i_footer_bar.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Abstract footer bar interface
 *
 * Displayed at the bottom of the LCD panel. Contains 5 labelled
 * slots corresponding to function keys F1–F5.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <string>

// Project Libraries
#include <overboard/core/action_code.hpp>

namespace ovb::gui {

/**
 * @brief Footer bar interface — 5 function-key slots
 *
 * Each slot has a label and fires a callback when clicked.
 * Slot indices are 0-based (slot 0 = F1, slot 4 = F5).
 */
class I_Footer_Bar {

    public:

        /// @brief Number of function-key slots
        static constexpr int SLOT_COUNT { 5 };

        /// @brief Slot click callback type
        using Slot_Cb = std::function<void(int slot)>;

        /**
         * @brief Virtual destructor
         */
        virtual ~I_Footer_Bar() = default;

        /**
         * @brief Set the label for a function-key slot
         * @param slot  0-based slot index (0=F1 … 4=F5)
         * @param label Text to display; empty string hides the label
         */
        virtual void set_label(int slot, const std::string& label) = 0;

        /**
         * @brief Register callback fired when any slot button is clicked
         * @param cb Receives the 0-based slot index
         */
        virtual void set_click_callback(Slot_Cb cb) = 0;

        /**
         * @brief Refresh the rendered bar (call after set_label changes)
         */
        virtual void refresh() = 0;
};

} // namespace ovb::gui
