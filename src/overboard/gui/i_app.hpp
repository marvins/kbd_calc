/**
 * @file    i_app.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Application interface for user-facing apps
 *
 * Extends I_Panel to add application metadata used by the
 * App_Menu and App_Registry for navigation and display.
 */
#pragma once

// C++ Standard Libraries
#include <optional>
#include <string_view>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/i_panel.hpp>

namespace ovb::gui {

/**
 * @brief Application interface for user-facing panels
 *
 * All user-facing applications (Calculator, Status, Settings)
 * implement this interface to provide metadata for the menu
 * system and app registry.
 */
class I_App : public I_Panel {

    public:

        /**
         * @brief Destructor
         */
        ~I_App() override = default;

        /**
         * @brief Get the LVGL menu icon symbol
         * @return LVGL symbol constant (e.g., LV_SYMBOL_KEYBOARD)
         */
        virtual const char* menu_icon() const = 0;

        /**
         * @brief Get optional mnemonic hotkey character for main menu
         * @return Character (e.g., 'c' for Calculator, 's' for Settings) or '\0' if no hotkey
         */
        virtual char menu_hotkey() const {
            return '\0';
        }

        /**
         * @brief Get menu display priority (lower = earlier in menu)
         * @return Priority value for sorting in menu
         */
        virtual int menu_priority() const {
            return 0;
        }
};

} // namespace ovb::gui
