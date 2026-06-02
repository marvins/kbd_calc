/**
 * @file    keyboard_window.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   SDL keyboard window driver
 *
 * Creates a separate SDL window for the on-screen keyboard display.
 * Used by PICOSDL target when SHOW_KEYBOARD_UI is enabled.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::hal::sdl {

/**
 * @brief SDL keyboard window driver
 *
 * Creates a separate SDL window with its own LVGL screen for the
 * on-screen keyboard display. Used for targets where the keyboard
 * should be in a separate window from the main LCD display.
 */
class Keyboard_Window {

    public:

        /**
         * @brief Create the SDL keyboard window and initialise LVGL screen
         *
         * @param title  Window title bar text
         * @param width  Window width in pixels
         * @param height Window height in pixels
         */
        Keyboard_Window( const std::string& title, int width, int height );

        ~Keyboard_Window();

        /// @brief SDL window ID for event filtering
        uint32_t window_id() const;

        /// @brief LVGL active screen — attach keyboard widgets here
        lv_obj_t* screen() const;

    private:

        /// @brief PImpl (Pointer to Implementation) structure
        struct Impl;

        /// @brief Pointer to implementation
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::hal::sdl
