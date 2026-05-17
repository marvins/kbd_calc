// C++ Standard Libraries
#include <cstdint>
#include <stdexcept>

// Project Libraries
#include <overboard/hal/font_5x7.hpp>
#include <overboard/hal/sdl/sdl_display.hpp>

using ovb::hal::FONT_5X7;
using ovb::hal::FONT_FIRST;
using ovb::hal::FONT_LAST;
using ovb::hal::UNICODE_GLYPHS;
using ovb::hal::UNICODE_GLYPH_COUNT;
using ovb::hal::utf8_decode;

SDL_Display::SDL_Display(const std::string& title, int x, int y, int width, int height)
    : m_width(width), m_height(height)
{
    m_window = SDL_CreateWindow(title.c_str(), x, y, width, height, SDL_WINDOW_SHOWN);
    if (!m_window) throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) throw std::runtime_error(std::string("SDL_CreateRenderer: ") + SDL_GetError());
}

SDL_Display::~SDL_Display() {
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window)   SDL_DestroyWindow(m_window);
}

int SDL_Display::width()  const { return m_width;  }
int SDL_Display::height() const { return m_height; }

SDL_Window* SDL_Display::window() const { return m_window; }

void SDL_Display::set_color(Color c) {
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, 255);
}

void SDL_Display::clear(Color c) {
    set_color(c);
    SDL_RenderClear(m_renderer);
}

void SDL_Display::draw_pixel(int x, int y, Color c) {
    set_color(c);
    SDL_RenderDrawPoint(m_renderer, x, y);
}

void SDL_Display::draw_rect(int x, int y, int w, int h, Color c, bool filled) {
    set_color(c);
    SDL_Rect r{x, y, w, h};
    if (filled) SDL_RenderFillRect(m_renderer, &r);
    else        SDL_RenderDrawRect(m_renderer, &r);
}

void SDL_Display::draw_text(int x, int y, const std::string& text, Color fg, Color bg, int scale) {
    size_t pos = 0;
    while (pos < text.size()) {
        uint32_t cp = utf8_decode(text, pos);

        const uint8_t* glyph = nullptr;

        // Check ASCII range first
        if (cp >= static_cast<uint32_t>(FONT_FIRST) &&
            cp <= static_cast<uint32_t>(FONT_LAST)) {
            glyph = FONT_5X7[cp - FONT_FIRST].data();
        } else {
            // Search Unicode glyph table
            for (int i = 0; i < UNICODE_GLYPH_COUNT; ++i) {
                if (UNICODE_GLYPHS[i].codepoint == cp) {
                    glyph = UNICODE_GLYPHS[i].rows.data();
                    break;
                }
            }
        }

        if (!glyph) {
            // Unknown glyph — render a small question box
            SDL_Rect r{x + scale, y + scale, 3 * scale, 5 * scale};
            set_color(fg);
            SDL_RenderDrawRect(m_renderer, &r);
            x += 6 * scale;
            continue;
        }

        for (int row = 0; row < 7; ++row) {
            uint8_t bits = glyph[row];
            for (int col = 0; col < 5; ++col) {
                Color px = (bits & (0x10 >> col)) ? fg : bg;
                SDL_Rect r{x + col * scale, y + row * scale, scale, scale};
                set_color(px);
                SDL_RenderFillRect(m_renderer, &r);
            }
        }
        x += 6 * scale;
    }
}

void SDL_Display::flush() {
    SDL_RenderPresent(m_renderer);
}

void SDL_Display::raise_to_front() {
    SDL_ShowWindow(m_window);
    SDL_RaiseWindow(m_window);
    SDL_SetWindowInputFocus(m_window);
}
