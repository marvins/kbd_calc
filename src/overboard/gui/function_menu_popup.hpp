/**
 * @file    function_menu_popup.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Function menu popup for calculator operations
 */

#pragma once

// C++ Standard Libraries
#include <functional>
#include <memory>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/core/input_key.hpp>
#include <overboard/gui/i_popup.hpp>

namespace ovb::gui {

/**
 * @brief Menu item for function popup
 */
struct Menu_Item {
    std::string         label;        ///< Display label
    core::Action_Code   action;       ///< Action to execute
    int                 key_index;    ///< Optional hotkey (keyboard display overlay), -1 = none
    
    /**
     * @brief Construct a menu item
     * @param lbl Display label
     * @param act Action to execute
     * @param key_idx Optional hotkey (keyboard display overlay), -1 = none
     */
    Menu_Item(std::string lbl, core::Action_Code act, int key_idx = -1)
        : label(std::move(lbl)), action(act), key_index(key_idx) {}
};

/**
 * @brief Function menu popup
 *
 * Displays a scrollable list of calculator operations.
 * User can navigate with arrow keys and select with ENTER.
 */
class Function_Menu_Popup : public I_Popup {

    public:

        /**
         * @brief Callback type for menu selection
         */
        using Select_Cb = std::function<void(core::Action_Code)>;

        /**
         * @brief Construct a function menu popup
         * @param parent Parent LVGL object
         * @param title Menu title (e.g., "Alg", "Trig")
         * @param items List of menu items
         * @param on_select Callback when item is selected
         */
        Function_Menu_Popup(lv_obj_t*                   parent,
                           const std::string&           title,
                           const std::vector<Menu_Item>& items,
                           Select_Cb                    on_select);

        /**
         * @brief Destructor
         */
        ~Function_Menu_Popup() override;

        /**
         * @brief Show the popup
         */
        void show() override;

        /**
         * @brief Hide the popup
         */
        void hide() override;
        
        /**
         * @brief Handle input key
         */
        bool handle_input(core::Input_Key key) override;
        
        /**
         * @brief Render the popup
         */
        void render() override;
        
        /**
         * @brief Get menu items (for hotkey overlay)
         */
        const std::vector<Menu_Item>& items() const;

    private:

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
