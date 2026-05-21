/**
 * @file sdl_app.cpp
 * @author Marvin Smith
 * @date 2025-10-18
 * @brief SDL application implementation
 */

// C++ Standard Libraries
#include <iostream>

// Third-Party Libraries
#include <SDL2/SDL.h>

// Project Libraries
#include <overboard/hal/sdl/sdl_app.hpp>
#include <overboard/hal/lcd_config.hpp>

namespace ovb::hal::sdl {

SDL_App::SDL_App(const core::Grid_Layout& layout)
    : m_layout(layout),
      m_kbd_display("Keyboard", core::Point2i(100, 100), core::Point2i(KBD_W, KBD_H)),
      m_lcd_display("Calculator", core::Point2i(480, 100),
                    core::Point2i(hal::LCD_WIDTH, hal::LCD_HEIGHT)),
      m_input(m_kbd_display, layout, HDR_H, MARGIN_LEFT, MARGIN_TOP) {}

bool SDL_App::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return false;
    }

    m_lcd_display.raise_to_front();
    m_kbd_display.raise_to_front();

    return true;
}

void SDL_App::run() {
    while (!should_quit()) {
        m_input.pump();

        // TODO: Integrate with Display_Controller and Calc_Engine
        // For now, just pump events
        SDL_Delay(16);
    }
}

bool SDL_App::should_quit() const {
    return m_quit;
}

I_Display& SDL_App::get_keyboard_display() {
    return m_kbd_display;
}

I_Display& SDL_App::get_lcd_display() {
    return m_lcd_display;
}

I_Input& SDL_App::get_input() {
    return m_input;
}

} // namespace ovb::hal::sdl
