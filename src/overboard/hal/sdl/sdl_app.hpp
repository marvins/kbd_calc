/**
 * @file sdl_app.hpp
 * @brief SDL platform application implementation
 *
 * SDL-based application for desktop simulator. Creates keyboard
 * and LCD windows, handles SDL event loop, and provides
 * display and input interfaces.
 */

#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_app.hpp>
#include <overboard/hal/sdl/lvgl_keyboard_display.hpp>
#include <overboard/hal/sdl/sdl_display.hpp>
#include <overboard/hal/sdl/sdl_input.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL application implementation
 *
 * Manages SDL window lifecycle and event processing for the
 * desktop simulator. Provides keyboard and LCD display windows
 * via SDL_Display, and input handling via SDL_Input.
 */
class SDL_App : public I_App {
    public:
        explicit SDL_App(const core::Grid_Layout& layout);

        bool init() override;
        void run() override;
        bool should_quit() const override;
        I_Display& get_keyboard_display() override;
        I_Display& get_lcd_display() override;
        I_Input& get_input() override;

    private:
        static constexpr int KBD_W = 480;  // Width for 8-column SK30 layout
        static constexpr int KBD_H = 480;
        static constexpr int HDR_H = 22;
        static constexpr int MARGIN_LEFT = 20;
        static constexpr int MARGIN_TOP = 16;

        core::Grid_Layout       m_layout;
        LVGL_Keyboard_Display  m_kbd_display;
        SDL_Display            m_lcd_display;
        SDL_Input              m_input;
        bool m_quit = false;
};

} // namespace ovb::hal::sdl
