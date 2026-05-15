#include <SDL2/SDL.h>
#include <iostream>

#include "hal/sdl/sdl_display.hpp"
#include "hal/sdl/sdl_input.hpp"
#include "core/keymap.hpp"
#include "core/layer_manager.hpp"
#include "core/calc_engine.hpp"
#include "core/display_controller.hpp"

static constexpr int KBD_W  = 360;
static constexpr int KBD_H  = 480;
static constexpr int LCD_W  = 480;
static constexpr int LCD_H  = 240;
static constexpr int HDR_H       = 22;
static constexpr int MARGIN_LEFT = 20;
static constexpr int MARGIN_TOP  = 16;

int main(int /*argc*/, char* /*argv*/[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return 1;
    }

    try {
        SDL_Display kbd_display("Keyboard",   100,  100, KBD_W, KBD_H);
        SDL_Display lcd_display("Calculator", 480,  100, LCD_W, LCD_H);

        lcd_display.raise_to_front();
        kbd_display.raise_to_front();

        Keymap        keymap;
        Layer_Manager layers(keymap);
        Calc_Engine   engine;

        SDL_Input        input(kbd_display, GRID_COLS, GRID_ROWS, HDR_H, MARGIN_LEFT, MARGIN_TOP);
        Display_Controller controller(kbd_display, lcd_display, layers, engine);

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
                        case Key_Code::LAYER_HOME:
                            layers.set_layer(0);
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
