/**
 * @file    sdl_lcd_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-21
 * @brief   SDL LCD display with typeset math rendering
 *
 * Creates an LVGL window with bezel styling, a scrollable history table,
 * and custom canvas widgets for rendering typeset math expressions.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
struct SDL_Window;

// Project Libraries
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/i_display.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL LCD display manager
 *
 * Owns the LCD window with:
 * - Bezel container with rounded border
 * - Scrollable history table (expression | result)
 * - Typeset expression preview area
 */
class SDL_LCD_Display : public I_Display {
    public:
        /**
         * @brief Create LVGL LCD window
         * @param title     Window title
         * @param pos       Window position (x, y)
         * @param size      Window size (width, height)
         * @param engine    Calculation engine for state/history
         * @param layers    Layer manager for mode display
         */
        SDL_LCD_Display( const std::string&            title,
                          int                           x,
                          int                           y,
                          int                           width,
                          int                           height,
                          const ovb::math::Calc_Engine& engine,
                          const ovb::core::Layer_Manager& layers );

        /**
         * Destructor
         */
        ~SDL_LCD_Display() override;

        // I_Display interface
        int  width()  const override;
        int  height() const override;
        void clear(Color c = Color::black()) override;
        void draw_pixel(ovb::core::Point<int> pos, Color c) override;
        void draw_rect(ovb::core::Point<int> pos, ovb::core::Point<int> size, Color c, bool filled = true) override;
        void draw_text(ovb::core::Point<int> pos, const std::string& text, Color fg, Color bg, int scale = 1) override;
        void flush() override;

        /// @brief SDL window ID for event filtering
        uint32_t window_id() const;

        /// @brief Refresh display from current engine state
        void refresh();

        /// @brief Drive LVGL render (call in main loop)
        void render();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::hal::sdl
