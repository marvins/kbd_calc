/**
 * @file    app_menu.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Application menu panel
 *
 * Shows a list of available panels (Status, Calculator, Settings)
 * with arrow key navigation. ESCAPE returns to this menu from
 * any panel; Enter/Return opens the selected panel.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/gui/app_registry.hpp>
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_app.hpp>
#include <overboard/gui/i_panel.hpp>

namespace ovb::gui {

// Forward declaration
class Panel_Manager;

/**
 * @brief Application menu panel
 *
 * Displays a scrollable list of panels. Arrow keys navigate,
 * Enter/Return opens the selected panel. ESCAPE from other panels
 * returns to this menu.
 */
class App_Menu : public I_Panel {

    public:

        /// @brief Callback when user selects an app
        using Select_Cb = std::function<void(int panel_index)>;

        /**
         * @brief Construct the menu panel
         * @param apps Apps to own and register with Panel_Manager
         * @param panels Panel_Manager to register apps with
         * @param system_info System info provider
         * @param items List of menu items from App_Registry
         * @param on_select Callback fired when user selects an app (passes panel_index)
         * @param initial_selection Index of item to pre-select
         */
        App_Menu( std::vector<std::shared_ptr<I_App>> apps,
                  Panel_Manager& panels,
                  hal::I_System_Info& system_info,
                  std::vector<Menu_Item> items,
                  Select_Cb on_select,
                  int initial_selection = 0 );

        /**
         * @brief Destructor
         */
        ~App_Menu() override;

        /**
         * @brief Activate the menu panel
         * @param parent Parent LVGL object
         */
        void        activate(lv_obj_t* parent) override;

        /**
         * @brief Deactivate the menu panel
         */
        void        deactivate()               override;

        /**
         * @brief Handle input actions
         * @param action The input action
         * @return true if handled, false otherwise
         */
        bool        handle_input(core::Action_Code action) override;

        /**
         * @brief Handle text input
         * @param codepoint The Unicode codepoint
         * @return true if handled, false otherwise
         */
        bool        handle_text(char32_t codepoint) override;

        /**
         * @brief Handle input keys
         * @param key The input key
         * @return true if handled, false otherwise
         */
        bool        handle_input_key(core::Input_Key key) override;

        /**
         * @brief Refresh the menu display
         */
        void        refresh()                  override;

        /**
         * @brief Get the menu name
         * @return "Menu"
         */
        std::string name()   const             override;

        /**
         * @brief Get custom label for keys in menu context
         * @param key_index The key index
         * @return "Sel" for key 28 (Enter key), empty for others
         */
        std::string get_custom_label( int key_index ) const override;

    private:

        void update_selection();
        void select_current();

        /// @brief Owned apps
        std::vector<std::shared_ptr<I_App>> m_apps;

        /// @brief Panel indices from Panel_Manager
        std::vector<int>                   m_panel_indices;

        /// @brief Menu items
        std::vector<Menu_Item>             m_menu_items;

        /// @brief Select callback
        Select_Cb                          m_on_select;

        /// @brief Initial selection index
        int                                m_initial_selection;

        /// @brief System info provider
        hal::I_System_Info&                m_system_info;

        /// @brief Container object
        lv_obj_t*                          m_container  = nullptr;

        /// @brief Header bar
        std::unique_ptr<Header_Bar>        m_header;

        /// @brief Footer bar
        std::unique_ptr<Footer_Bar>        m_footer;

        /// @brief List object
        lv_obj_t*                          m_list       = nullptr;

        /// @brief Selected index
        int                                m_selected   = 0;
};

} // namespace ovb::gui
