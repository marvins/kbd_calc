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
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_panel.hpp>

namespace ovb::gui {

/**
 * @brief Application menu panel
 *
 * Displays a scrollable list of panels. Arrow keys navigate,
 * Enter/Return opens the selected panel. ESCAPE from other panels
 * returns to this menu.
 */
class App_Menu : public I_Panel {

    public:

        using Select_Cb = std::function<void(const std::string& panel_name)>;

        /**
         * @brief Construct the menu panel
         * @param on_select Callback fired when user selects a panel
         * @param initial_selection Index of panel to pre-select (0=Status, 1=Calculator, 2=Settings)
         */
        explicit App_Menu(Select_Cb on_select, int initial_selection = 0);

        /**
         * @brief Destructor
         */
        ~App_Menu() override;

        /**
         * @bri
         */
        void        activate(lv_obj_t* parent) override;
        void        deactivate()               override;
        bool        handle_input(core::Action_Code action) override;
        bool        handle_text(char32_t codepoint) override;
        bool        handle_input_key(core::Input_Key key) override;
        void        refresh()                  override;
        std::string name()   const             override;

        /**
         * @brief Get custom label for keys in menu context
         * @param key_index The key index
         * @return "Sel" for key 28 (Enter key), empty for others
         */
        std::string get_custom_label(int key_index) const override;

    private:

        void update_selection();
        void select_current();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
