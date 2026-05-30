/**
 * @file    app_view.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LVGL application view — LCD + keyboard widget manager
 *
 * Owns the LCD_Section and Keyboard_View widgets on a given LVGL root
 * object. Implements I_Display so it can be returned through I_App.
 * Platform-agnostic: works on both SDL simulator and embedded targets.
 */
#pragma once

// C++ Standard Libraries
#include <memory>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/i_display.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::gui {

/**
 * @brief LVGL application view
 *
 * Attaches LCD_Section (top) and Keyboard_View (bottom) to a provided
 * LVGL root object. The root is typically the active screen supplied by
 * the platform HAL after window creation.
 */
class App_View : public hal::I_Display {
    public:

        /**
         * @brief Construct and build the application view
         *
         * @param root   LVGL root object to attach widgets to
         * @param layout Keyboard grid layout for button positioning
         * @param engine Calculation engine for expression state and history
         * @param layers Layer manager for key label management
         */
        App_View( lv_obj_t*                       root,
                  const ovb::core::Grid_Layout&   layout,
                  const ovb::math::Calc_Engine&   engine,
                  const ovb::core::Layer_Manager& layers );

        ~App_View() override = default;

        /// @brief Refresh the LCD section from current engine state
        void refresh()      override;

        /// @brief Re-render keyboard buttons for the current layer
        void update_layer() override;

        /// @brief Drive the LVGL render loop (call once per frame)
        void render()       override;

        /**
         * @brief Wire a callback for keyboard button clicks
         * @param cb        Callback to invoke with the key index
         * @param user_data Passed through to the callback
         */
        void set_key_callback( void (*cb)(int key_index, void* user_data),
                               void* user_data );

    private:

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
