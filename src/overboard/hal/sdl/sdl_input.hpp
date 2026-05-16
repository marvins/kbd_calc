#pragma once

#include <overboard/hal/i_input.hpp>
#include <overboard/hal/sdl/sdl_display.hpp>
#include <SDL2/SDL.h>
#include <queue>

class SDL_Input : public I_Input {
    public:
        explicit SDL_Input(SDL_Display& kbd_display, int cols, int rows,
                           int header_height, int margin_left, int margin_top);

        bool poll(Key_Event& out_event) override;
        bool should_quit() const override;
        void pump() override;

        int  hit_test(int mouse_x, int mouse_y) const;

    private:
        SDL_Display&          m_kbd_display;
        int                   m_cols;
        int                   m_rows;
        int                   m_header_height;
        int                   m_margin_left;
        int                   m_margin_top;
        bool                  m_quit = false;
        std::queue<Key_Event> m_event_queue;

        static constexpr int KEY_PAD = 4;
};
