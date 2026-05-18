// C++ Standard Libraries
#include <cstdint>
#include <stdexcept>

// Project Libraries
#include <overboard/hal/font_5x7.hpp>
#include <overboard/hal/sdl/sdl_display.hpp>

namespace ovb::hal::sdl {

/****************************/
/*   Constructor/Destructor */
/****************************/
SDL_Display::SDL_Display(const std::string& title, core::Point2i pos, core::Point2i size)
    : m_width(size.x), m_height(size.y)
{
    m_window = SDL_CreateWindow(title.c_str(), pos.x, pos.y, size.x, size.y, SDL_WINDOW_SHOWN);
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

/****************************/
/*      Color Helper        */
/****************************/
void SDL_Display::set_color(Color c) {
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, 255);
}

void SDL_Display::clear(Color c) {
    set_color(c);
    SDL_RenderClear(m_renderer);
}

/****************************/
/*       Drawing Prims      */
/****************************/
void SDL_Display::draw_pixel(core::Point2i pos, Color c) {
    set_color(c);
    SDL_RenderDrawPoint(m_renderer, pos.x, pos.y);
}

void SDL_Display::draw_rect(core::Point2i pos, core::Point2i size, Color c, bool filled) {
    set_color(c);
    SDL_Rect r{pos.x, pos.y, size.x, size.y};
    if (filled) SDL_RenderFillRect(m_renderer, &r);
    else        SDL_RenderDrawRect(m_renderer, &r);
}

/****************************/
/*       Text Rendering     */
/****************************/
void SDL_Display::draw_text(core::Point2i pos, const std::string& text, Color fg, Color bg, int scale) {
    int cursor_x = pos.x;
    size_t byte_pos = 0;
    while (byte_pos < text.size()) {
        uint32_t cp = utf8_decode(text, byte_pos);

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
            SDL_Rect r{cursor_x + scale, pos.y + scale, 3 * scale, 5 * scale};
            set_color(fg);
            SDL_RenderDrawRect(m_renderer, &r);
            cursor_x += 6 * scale;
            continue;
        }

        for (int row = 0; row < 7; ++row) {
            uint8_t bits = glyph[row];
            for (int col = 0; col < 5; ++col) {
                Color px = (bits & (0x10 >> col)) ? fg : bg;
                SDL_Rect r{cursor_x + col * scale, pos.y + row * scale, scale, scale};
                set_color(px);
                SDL_RenderFillRect(m_renderer, &r);
            }
        }
        cursor_x += 6 * scale;
    }
}

/****************************/
/*       Presentation       */
/****************************/
void SDL_Display::flush() {
    SDL_RenderPresent(m_renderer);
}

void SDL_Display::raise_to_front() {
    SDL_ShowWindow(m_window);
    SDL_RaiseWindow(m_window);
    SDL_SetWindowInputFocus(m_window);
}

} // namespace ovb::hal::sdl
