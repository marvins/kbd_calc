#pragma once

#include <cstdint>
#include <string>

#include <overboard/core/point.hpp>

/**
 * @brief RGB color structure
 *
 * Simple 8-bit per channel color representation used throughout the display HAL.
 */
struct Color {
    uint8_t r;  ///< Red channel (0-255)
    uint8_t g;  ///< Green channel (0-255)
    uint8_t b;  ///< Blue channel (0-255)

    /// @return Black color {0, 0, 0}
    static constexpr Color black()   { return {0,   0,   0  }; }
    /// @return White color {255, 255, 255}
    static constexpr Color white()   { return {255, 255, 255}; }
    /// @return Gray color {128, 128, 128}
    static constexpr Color gray()    { return {128, 128, 128}; }
    /// @return Red color {220, 50, 50}
    static constexpr Color red()     { return {220, 50,  50 }; }
    /// @return Green color {50, 200, 50}
    static constexpr Color green()   { return {50,  200, 50 }; }
    /// @return Blue color {50, 100, 220}
    static constexpr Color blue()    { return {50,  100, 220}; }
    /// @return Light blue color {100, 100, 200}
    static constexpr Color light_blue() { return {100, 100, 200}; }
    /// @return Yellow color {230, 200, 50}
    static constexpr Color yellow()  { return {230, 200, 50 }; }
};

/**
 * @brief Display hardware abstraction interface
 *
 * Abstract interface for rendering operations. Implementations provide
 * platform-specific display output (e.g., SDL2 window, LCD driver).
 *
 * Supports both integer coordinate and Point-based drawing methods.
 */
class I_Display {
    public:
        virtual ~I_Display() = default;

        /// @return Display width in pixels
        virtual int  width()  const = 0;

        /// @return Display height in pixels
        virtual int  height() const = 0;

        /**
         * @brief Clear entire display to specified color
         * @param c Fill color (default black)
         */
        virtual void clear(Color c = Color::black()) = 0;

        /**
         * @brief Draw single pixel
         * @param pos Pixel position
         * @param c Pixel color
         */
        virtual void draw_pixel(ovb::core::Point<int> pos, Color c) = 0;

        /**
         * @brief Draw rectangle
         * @param pos Top-left corner position
         * @param size Rectangle dimensions (x=width, y=height)
         * @param c Rectangle color
         * @param filled true for filled rect, false for outline only
         */
        virtual void draw_rect(ovb::core::Point<int> pos, ovb::core::Point<int> size, Color c, bool filled = true) = 0;

        /**
         * @brief Draw text string
         * @param pos Text origin position
         * @param text String to draw
         * @param fg Foreground (text) color
         * @param bg Background color
         * @param scale Font scale multiplier (default 1)
         */
        virtual void draw_text(ovb::core::Point<int> pos, const std::string& text, Color fg, Color bg, int scale = 1) = 0;

        /// @brief Flush pending drawing operations to display
        virtual void flush() = 0;
};
