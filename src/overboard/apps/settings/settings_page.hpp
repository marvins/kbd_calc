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
#include <overboard/core/settings_manager.hpp>
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_app.hpp>

namespace ovb::gui {

/**
 * @brief Setting entry for key-value display
 */
struct Setting_Entry {
    std::string key;
    std::string value;
    lv_obj_t*   row       { nullptr };
    lv_obj_t*   key_label { nullptr };
    lv_obj_t*   val_input { nullptr };
    bool        modified  { false };
};

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
         * @param settings Application settings manager
         * @param on_back Callback fired when user navigates back (ESCAPE)
         */
        Settings_Page(std::shared_ptr<core::Settings_Manager> settings, Back_Cb on_back = {});

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

        /**
         * @brief Load settings from manager
         */
        void load_settings();
        
        /**
         * @brief Create a setting row
         */
        void create_setting_row(Setting_Entry& entry, lv_obj_t* parent);
        
        void save_all();
        
        void reload_all();
        
        void update_save_button_state();

        /// @brief Settings manager
        std::shared_ptr<core::Settings_Manager> m_settings;
        /// @brief Callback fired when user navigates back (ESCAPE)
        Back_Cb                                 m_on_back;
        /// @brief Container object
        lv_obj_t*                               m_container   { nullptr };
        /// @brief Scroll area object
        lv_obj_t*                               m_scroll_area { nullptr };
        /// @brief Save button object
        lv_obj_t*                               m_save_btn    { nullptr };
        /// @brief Reload button object
        lv_obj_t*                               m_reload_btn  { nullptr };
        /// @brief Header bar object
        std::unique_ptr<Header_Bar>             m_header;
        /// @brief Footer bar object
        std::unique_ptr<Footer_Bar>             m_footer;
        /// @brief Setting entries
        std::vector<Setting_Entry>              m_entries;
};

} // namespace ovb::gui
