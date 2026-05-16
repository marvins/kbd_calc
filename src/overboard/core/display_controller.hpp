#pragma once

#include <overboard/hal/i_display.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/core/calc_engine.hpp>

class Display_Controller {
    public:
        Display_Controller(I_Display& kbd_display, I_Display& lcd_display,
                           const Layer_Manager& layers, const Calc_Engine& engine);

        void render();

        void set_pressed_key(int key_index);
        void clear_pressed_key();

    private:
        I_Display&           m_kbd_display;
        I_Display&           m_lcd_display;
        const Layer_Manager& m_layers;
        const Calc_Engine&   m_engine;
        int                  m_pressed_key = -1;

        static constexpr int COLS       = GRID_COLS;
        static constexpr int KEY_PAD    = 4;
        static constexpr int MARGIN_LEFT = 20; // space for row IDs
        static constexpr int MARGIN_TOP  = 16; // space for col IDs

        void render_keyboard();
        void render_lcd();
        void draw_key(int index, const Key_Def& key, bool pressed);
};
