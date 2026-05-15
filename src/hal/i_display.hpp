#pragma once

#include <cstdint>
#include <string>

struct Color {
    uint8_t r, g, b;

    static constexpr Color black()   { return {0,   0,   0  }; }
    static constexpr Color white()   { return {255, 255, 255}; }
    static constexpr Color gray()    { return {128, 128, 128}; }
    static constexpr Color red()     { return {220, 50,  50 }; }
    static constexpr Color green()   { return {50,  200, 50 }; }
    static constexpr Color blue()    { return {50,  100, 220}; }
    static constexpr Color yellow()  { return {230, 200, 50 }; }
};

class I_Display {
    public:
        virtual ~I_Display() = default;

        virtual int  width()  const = 0;
        virtual int  height() const = 0;

        virtual void clear(Color c = Color::black()) = 0;
        virtual void draw_pixel(int x, int y, Color c) = 0;
        virtual void draw_rect(int x, int y, int w, int h, Color c, bool filled = true) = 0;
        virtual void draw_text(int x, int y, const std::string& text, Color fg, Color bg, int scale = 1) = 0;
        virtual void flush() = 0;
};
