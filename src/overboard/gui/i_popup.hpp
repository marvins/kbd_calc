/**
 * @file    i_popup.hpp
 * @author  Marvin Smith
 * @date    2026-06-03
 *
 * @brief   Popup panel interface
 */

#pragma once

// Overboard Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/core/input_key.hpp>

namespace ovb::gui {

/**
 * @brief Popup panel interface
 *
 * Popups are transient panels that appear over the main interface,
 * typically anchored to a specific UI element (e.g., footer slots).
 */
class I_Popup {
    public:
        virtual ~I_Popup() = default;

        /**
         * @brief Show the popup
         */
        virtual void show() = 0;

        /**
         * @brief Hide the popup
         */
        virtual void hide() = 0;

        /**
         * @brief Handle input key press
         * @param key The input key that was pressed
         * @return true if the input was handled, false otherwise
         */
        virtual bool handle_input(core::Input_Key key) = 0;

        /**
         * @brief Handle action code
         * @param action The action code to handle
         * @return true if the action was handled, false otherwise
         */
        virtual bool handle_input(core::Action_Code action) = 0;

        /**
         * @brief Render the popup
         */
        virtual void render() = 0;
};

} // namespace ovb::gui
