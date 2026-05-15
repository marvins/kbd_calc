#pragma once

#include "../i_display.hpp"
#include <SDL2/SDL.h>
#include <string>


class SDL_Display : public I_Display {
    public:
        SDL_Display(const std::string& title, int x, int y, int width, int height);
        ~SDL_Display() override;

        int  width()  const override;
        int  height() const override;

        void clear(Color c = Color::black()) override;
        void draw_pixel(int x, int y, Color c) override;
        void draw_rect(int x, int y, int w, int h, Color c, bool filled = true) override;
        void draw_text(int x, int y, const std::string& text, Color fg, Color bg, int scale = 1) override;
        void flush() override;

        SDL_Window* window() const;
        void        raise_to_front();

    private:
        SDL_Window*   m_window   = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        int           m_width;
        int           m_height;

        void set_color(Color c);
};
