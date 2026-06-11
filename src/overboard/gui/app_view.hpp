/**
 * @file    app_view.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LVGL application view — panel system host + optional keyboard display
 *
 * Owns a Panel_Manager and an optional Keyboard_Display. The Panel_Manager
 * starts with Status_Page and switches to Calculator_App on first keypress.
 * Platform-agnostic: works on both SDL simulator and embedded targets.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <functional>
#include <memory>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/panel_manager.hpp>
#include <overboard/hal/i_display.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::gui {

/**
 * @brief LVGL application view
 *
 * Attaches LCD_Section (top) and Keyboard_Display (bottom) to a provided
 * LVGL root object. The root is typically the active screen supplied by
 * the platform HAL after window creation.
 */
class App_View : public hal::I_Display {
    public:

        /**
         * @brief Construct and build the application view
         *
         * @param root   LVGL root object to attach widgets to
         * @param layout Keyboard grid layout for key positioning
         * @param engine Calculation engine for expression state and history
         * @param layers Layer manager for key label management
         */
        App_View( lv_obj_t*                      root,
                  const ovb::core::Grid_Layout&  layout,
                  ovb::math::Calc_Engine&        engine,
                  ovb::core::Layer_Manager&      layers );

        ~App_View() override;

        /**
         * @brief Route an action through the active panel
         * @param action The action to dispatch
         */
        void handle_input(core::Action_Code action);

        /**
         * @brief Forward a resolved text codepoint to the active panel
         * @param codepoint UTF-32 character from platform text input
         * @return true if text was consumed and panel refreshed itself
         */
        bool handle_text(char32_t codepoint);

        /**
         * @brief Forward an Input_Key to the active panel for context-dependent handling
         * @param key Input_Key from standard keyboard
         */
        void handle_input_key(core::Input_Key key);

        /// @brief Refresh the active panel from current state
        void refresh()      override;

        /// @brief Re-render keyboard display for the current layer
        void update_layer() override;

        /**
         * @brief Register a callback fired when an on-screen key button is clicked
         *
         * Forwards to Keyboard_Display::set_click_callback. Must be called
         * after construction. Has no effect if the keyboard UI is not present.
         *
         * @param cb Callable receiving the logical key index
         */
        void set_key_click_callback(std::function<void(int)> cb);

        /// @brief Drive the LVGL render loop (call once per frame)
        void render()       override;

        /**
         * @brief Get custom label for a key from the active panel
         * @param key_index The key index to query
         * @return Custom label string, or empty if none
         */
        std::string get_active_panel_label(int key_index) const;

        /**
         * @brief Register a callback fired when the active panel changes
         * @param cb Callback function to invoke on panel change
         */
        void set_panel_change_callback(std::function<void(I_Panel*)> cb);

    private:

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
