/**
 * @file    main.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Main entry point for the calculator application.
 */

// C++ Standard Libraries
#include <cstdint>
#include <iostream>

// Third-Party Libraries
#include <SDL2/SDL.h>
#include <lvgl.h>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/lcd_config.hpp>
#include <overboard/hal/sdl/lvgl_keyboard.hpp>
#include <overboard/hal/sdl/lvgl_lcd_display.hpp>
#include <overboard/hal/sdl/sdl_input.hpp>
#include <overboard/math/calc_engine.hpp>


using namespace ovb;

static constexpr int KBD_W  = 480;  // Wider for 8-column SK30 layout
static constexpr int KBD_H  = 480;
static constexpr int HDR_H       = 22;
static constexpr int MARGIN_LEFT = 20;
static constexpr int MARGIN_TOP  = 16;

int main(int /*argc*/, char* /*argv*/[]) {
    // Initialise LVGL before SDL so the SDL driver is registered
    lv_init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return 1;
    }

    try {
        // Keyboard window — rendered via LVGL widgets
        Keymap        keymap;
        core::Layer_Manager layers(keymap);

        // Use Womier SK30 asymmetric layout (8×6 grid with split sections)
        core::Grid_Layout sk30_layout = core::Grid_Layout::womier_sk30();

        hal::sdl::LVGL_Keyboard kbd_manager( "Keyboard", KBD_W, KBD_H,
                                             sk30_layout, layers );

        math::Calc_Engine engine;

        // LCD display — LVGL-based with typeset math rendering
        hal::sdl::LVGL_LCD_Display lcd_display( "Calculator",
                                                KBD_W + 120, 100,
                                                hal::LCD_WIDTH, hal::LCD_HEIGHT,
                                                engine, layers );

        hal::sdl::SDL_Input input( kbd_manager.window_id(),
                                   kbd_manager.width(),
                                   kbd_manager.height(),
                                   sk30_layout,
                                   HDR_H,
                                   MARGIN_LEFT,
                                   MARGIN_TOP );

        kbd_manager.render();
        lcd_display.render();

        while (!input.should_quit()) {
            input.pump();

            Key_Event ev;
            while (input.poll(ev)) {
                const Key_Def& key = layers.key_at(ev.key_index);

                if (ev.type == Key_Event_Type::Press) {
                    kbd_manager.set_pressed(ev.key_index);

                    switch (key.code) {
                        case Key_Code::LAYER_NEXT:
                            layers.next_layer();
                            kbd_manager.update_layer();
                            break;
                        case Key_Code::LAYER_PREV:
                            layers.prev_layer();
                            kbd_manager.update_layer();
                            break;
                        case Key_Code::LAYER_CONST:
                            layers.set_layer(2);
                            kbd_manager.update_layer();
                            break;
                        case Key_Code::LAYER_ALG:
                            layers.set_layer(4);
                            kbd_manager.update_layer();
                            break;
                        case Key_Code::LAYER_HOME:
                            layers.set_layer(0);
                            kbd_manager.update_layer();
                            break;
                        case Key_Code::TOGGLE_MATH_LAYOUT:
                            engine.toggle_math_layout();
                            break;
                        default:
                            engine.handle_key(key.code);
                            break;
                    }
                    kbd_manager.render();
                    lcd_display.refresh();
                    lcd_display.render();
                }

                if (ev.type == Key_Event_Type::Release) {
                    kbd_manager.clear_pressed();
                    kbd_manager.render();
                    lcd_display.render();
                }
            }

            // Drive LVGL tick and timer handler every frame
            lv_tick_inc(16);
            lv_timer_handler();

            SDL_Delay(16);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Quit();
    return 0;
}
