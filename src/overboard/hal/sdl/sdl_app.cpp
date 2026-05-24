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
#include <overboard/core/keymap.hpp>
#include <overboard/io/via_layout.hpp>

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

/************************************/
/*          Create the app          */
/************************************/
std::unique_ptr<SDL_App> SDL_App::create(const core::Grid_Layout& layout,
                                         const std::filesystem::path& layout_path,
                                         const std::filesystem::path& keymap_path) {
    auto app = std::unique_ptr<SDL_App>(new SDL_App(layout));

    // Load keymap from JSON
    try {
        auto layers = core::load_layers_from_json(keymap_path.string());
        app->m_keymap = core::Keymap(layers);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load keymap from " << keymap_path << ": " << e.what() << "\n";
        return nullptr;
    }

    app->m_layout_path  = layout_path;
    app->m_keymap_path  = keymap_path;

    if (!app->init()) {
        return nullptr;
    }
    return app;
}

/****************************************************/
/*          Initialize the SDL application          */
/****************************************************/
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

        // Load scancode bindings from keymap JSON into the input keymap
        if (!m_layout_path.empty() && !m_keymap_path.empty()) {
            try {
                auto via_layout = io::parse_via_layout(m_layout_path);
                io::apply_scancodes_from_json(via_layout, m_keymap_path);
                auto sc_map = io::build_scancode_index_map(via_layout);
                if (!sc_map.empty()) {
                    m_input->keymap().load_from_map(sc_map);
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: failed to load scancodes: " << e.what() << "\n";
            }
        }

        m_keyboard->render();
        m_lcd_display->render();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "SDL_App init error: " << e.what() << "\n";
        return false;
    }
}

/************************************/
/*          Run the app             */
/************************************/
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

/****************************************/
/*      Check if the app should quit    */
/****************************************/
bool SDL_App::should_quit() const {
    return m_should_quit;
}

/****************************************/
/*      Get the Keyboard Display        */
/****************************************/
I_Display& SDL_App::get_keyboard_display() {
    return m_keyboard->get_display();
}

/****************************************/
/*          Get the LCD Display         */
/****************************************/
I_Display& SDL_App::get_lcd_display() {
    return *m_lcd_display;
}

/************************************/
/*      Get Input Device Handle     */
/************************************/
I_Input& SDL_App::get_input() {
    return *m_input;
}

} // namespace ovb::hal::sdl
