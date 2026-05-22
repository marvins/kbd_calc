/**
 * @file    sdl_app.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   SDL simulator application implementation
 */

#include <overboard/hal/sdl/sdl_app.hpp>

// C++ Standard Libraries
#include <iostream>
#include <stdexcept>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>

// Project Libraries
#include <overboard/hal/lcd_config.hpp>

namespace ovb::hal::sdl {

SDL_App::SDL_App(const core::Grid_Layout& layout)
    : m_layout(layout),
      m_layers(m_keymap) {
}

SDL_App::~SDL_App() {
    if (m_initialized) {
        SDL_Quit();
    }
}

std::unique_ptr<SDL_App> SDL_App::create(const core::Grid_Layout& layout) {
    auto app = std::unique_ptr<SDL_App>(new SDL_App(layout));
    if (!app->init()) {
        return nullptr;
    }
    return app;
}

bool SDL_App::init() {
    // Initialise LVGL before SDL so the SDL driver is registered
    lv_init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return false;
    }

    m_initialized = true;

    try {
        // Keyboard window — rendered via LVGL widgets
        m_keyboard = std::make_unique<SDL_Keyboard>("Keyboard", KBD_W, KBD_H,
                                                     m_layout, m_layers);

        // LCD display — LVGL-based with typeset math rendering
        m_lcd_display = std::make_unique<SDL_LCD_Display>("Calculator",
                                                            KBD_W + 120, 100,
                                                            LCD_WIDTH, LCD_HEIGHT,
                                                            m_engine, m_layers);

        // Input handler
        m_input = std::make_unique<SDL_Input>(m_keyboard->window_id(),
                                               m_keyboard->width(),
                                               m_keyboard->height(),
                                               m_layout,
                                               HDR_H,
                                               MARGIN_LEFT,
                                               MARGIN_TOP);

        m_keyboard->render();
        m_lcd_display->render();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "SDL_App init error: " << e.what() << "\n";
        return false;
    }
}

void SDL_App::run() {
    while (!m_should_quit) {
        m_input->pump();

        Key_Event ev;
        while (m_input->poll(ev)) {
            const Key_Def& key = m_layers.key_at(ev.key_index);

            if (ev.type == Key_Event_Type::Press) {
                m_keyboard->set_pressed(ev.key_index);

                switch (key.code) {
                    case Key_Code::LAYER_NEXT:
                        m_layers.next_layer();
                        m_keyboard->update_layer();
                        break;
                    case Key_Code::LAYER_PREV:
                        m_layers.prev_layer();
                        m_keyboard->update_layer();
                        break;
                    case Key_Code::LAYER_CONST:
                        m_layers.set_layer(2);
                        m_keyboard->update_layer();
                        break;
                    case Key_Code::LAYER_ALG:
                        m_layers.set_layer(4);
                        m_keyboard->update_layer();
                        break;
                    case Key_Code::LAYER_HOME:
                        m_layers.set_layer(0);
                        m_keyboard->update_layer();
                        break;
                    case Key_Code::TOGGLE_MATH_LAYOUT:
                        m_engine.toggle_math_layout();
                        break;
                    default:
                        m_engine.handle_key(key.code);
                        break;
                }
                m_keyboard->render();
                m_lcd_display->refresh();
                m_lcd_display->render();
            }

            if (ev.type == Key_Event_Type::Release) {
                m_keyboard->clear_pressed();
                m_keyboard->render();
                m_lcd_display->render();
            }
        }

        // Drive LVGL tick and timer handler every frame
        lv_tick_inc(16);
        lv_timer_handler();

        SDL_Delay(16);
    }
}

bool SDL_App::should_quit() const {
    return m_should_quit;
}

I_Display& SDL_App::get_keyboard_display() {
    return m_keyboard->get_display();
}

I_Display& SDL_App::get_lcd_display() {
    return *m_lcd_display;
}

I_Input& SDL_App::get_input() {
    return *m_input;
}

} // namespace ovb::hal::sdl
