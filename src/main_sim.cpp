// C++ Standard Libraries
#include <iostream>

// Third-Party Libraries
#include <SDL2/SDL.h>

// Project Libraries
#include <overboard/core/calc_engine.hpp>
#include <overboard/core/display_controller.hpp>
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/lcd_config.hpp>
#include <overboard/hal/sdl/sdl_display.hpp>
#include <overboard/hal/sdl/sdl_input.hpp>


using namespace ovb;

static constexpr int KBD_W  = 480;  // Wider for 8-column SK30 layout
static constexpr int KBD_H  = 480;
static constexpr int HDR_H       = 22;
static constexpr int MARGIN_LEFT = 20;
static constexpr int MARGIN_TOP  = 16;

int main(int /*argc*/, char* /*argv*/[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return 1;
    }

    try {
        hal::sdl::SDL_Display kbd_display( "Keyboard",
                                           core::Point2i(100, 100),
                                           core::Point2i(KBD_W, KBD_H));

        hal::sdl::SDL_Display lcd_display( "Calculator",
                                           core::Point2i(480, 100),
                                           core::Point2i(hal::LCD_WIDTH, hal::LCD_HEIGHT));

        lcd_display.raise_to_front();
        kbd_display.raise_to_front();

        Keymap        keymap;
        Layer_Manager layers(keymap);
        Calc_Engine   engine;

        // Use Womier SK30 asymmetric layout (8×6 grid with split sections)
        core::Grid_Layout sk30_layout = core::Grid_Layout::womier_sk30();

        hal::sdl::SDL_Input input( kbd_display,
                                   sk30_layout,
                                   HDR_H,
                                   MARGIN_LEFT,
                                   MARGIN_TOP );

        core::Display_Controller controller( kbd_display,
                                             lcd_display,
                                             layers,
                                             engine,
                                             sk30_layout );

        controller.render();

        while (!input.should_quit()) {
            input.pump();

            Key_Event ev;
            while (input.poll(ev)) {
                const Key_Def& key = layers.key_at(ev.key_index);

                if (ev.type == Key_Event_Type::Press) {
                    controller.set_pressed_key(ev.key_index);

                    switch (key.code) {
                        case Key_Code::LAYER_NEXT:
                            layers.next_layer();
                            break;
                        case Key_Code::LAYER_PREV:
                            layers.prev_layer();
                            break;
                        case Key_Code::CONST_LAYER:
                            layers.set_layer(2);
                            break;
                        case Key_Code::ALGEBRA_LAYER:
                            layers.set_layer(4);
                            break;
                        case Key_Code::LAYER_HOME:
                            layers.set_layer(0);
                            break;
                        case Key_Code::TOGGLE_MATH_LAYOUT:
                            engine.toggle_math_layout();
                            break;
                        default:
                            engine.handle_key(key.code);
                            break;
                    }
                    controller.render();
                }

                if (ev.type == Key_Event_Type::Release) {
                    controller.clear_pressed_key();
                    controller.render();
                }
            }

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
