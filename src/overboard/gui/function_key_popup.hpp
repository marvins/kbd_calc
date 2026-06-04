/**
 * @file    function_key_popup.hpp
 * @author  Marvin Smith
 * @date    2026-06-03
 *
 * @brief   Function key popup panel
 */

#pragma once

#include <overboard/gui/i_popup.hpp>
#include <overboard/core/input_key.hpp>

namespace ovb::gui {

/**
 * @brief Function key popup panel
 *
 * Displays a popup anchored to a footer slot when an F-key is pressed.
 * Currently blank - will be populated with scrollable item list in the future.
 */
class Function_Key_Popup : public I_Popup {

    public:

        /**
         * @brief Construct a new Function_Key_Popup
         * @param func_index The function key index (1-5)
         */
        explicit Function_Key_Popup(int func_index);

        /**
         * @brief Destructor
         */
        ~Function_Key_Popup() override = default;

        /**
         * @brief Show the popup
         */
        void show() override;

        /**
         * @brief Hide the popup
         */
        void hide() override;

        /**
         * @brief Handle input key press
         * @param key The input key that was pressed
         * @return true if the input was handled, false otherwise
         */
        bool handle_input(core::Input_Key key) override;

        /**
         * @brief Render the popup
         */
        void render() override;

    private:

        /**
         * @brief Function key index (1-5)
         */
        [[maybe_unused]] int m_func_index;

        /**
         * @brief Whether popup is currently visible
         */
        bool m_visible;
};

} // namespace ovb::gui
