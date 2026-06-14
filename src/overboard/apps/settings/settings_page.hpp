/**
 * @file    settings_page.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Settings page panel
 *
 * Stub panel for application settings. ESCAPE returns to menu.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_app.hpp>

namespace ovb::gui {

/**
 * @brief Settings page panel
 *
 * Stub panel for application settings. ESCAPE returns to menu.
 */
class Settings_Page : public I_App {

    public:

        /// @brief Callback fired when user navigates back (ESCAPE)
        using Back_Cb = std::function<void()>;

        /**
         * @brief Construct the settings panel
         * @param on_back Callback fired when user navigates back (ESCAPE)
         */
        explicit Settings_Page(Back_Cb on_back = {});

        /**
         * @brief Destroy the settings page
         */
        ~Settings_Page() override;

        /**
         * @brief Activate the settings page
         * @param parent Parent LVGL object
         */
        void        activate(lv_obj_t* parent) override;

        /**
         * @brief Deactivate the settings page
         */
        void        deactivate()               override;

        /**
         * @brief Handle input events
         * @param action Action code
         * @return True if the action was handled
         */
        bool        handle_input(core::Action_Code action) override;

        /**
         * @brief Refresh the settings page
         */
        void        refresh()                  override;

        /**
         * @brief Get the name of the settings page
         * @return Name of the settings page
         */
        std::string name() const override { return "Settings"; }

        /**
         * @brief Get LVGL menu icon symbol
         * @return LVGL symbol constant
         */
        const char* menu_icon() const override { return LV_SYMBOL_SETTINGS; }

        /**
         * @brief Get menu display priority
         * @return Priority value (2 = third in menu)
         */
        int menu_priority() const override { return 2; }

        /**
         * @brief Get menu mnemonic hotkey
         * @return 'g' for Settings (g for configuration)
         */
        char menu_hotkey() const override { return 'g'; }

    private:

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
