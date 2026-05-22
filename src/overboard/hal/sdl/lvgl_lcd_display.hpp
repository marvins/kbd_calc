/**
 * @file lvgl_lcd_display.hpp
 * @author Marvin Smith
 * @date 2026-05-21
 * @brief LVGL-based LCD display with typeset math rendering
 *
 * Creates an LVGL window with bezel styling, a scrollable history table,
 * and custom canvas widgets for rendering typeset math expressions.
 * Replaces SDL_Display for the calculator LCD view.
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
#include <overboard/math/calc_engine.hpp>

namespace ovb::hal::sdl {

/**
 * @brief LVGL LCD display manager
 *
 * Owns the LCD window with:
 * - Bezel container with rounded border
 * - Scrollable history table (expression | result)
 * - Typeset expression preview area
 */
class LVGL_LCD_Display {
    public:
        /**
         * @brief Create LVGL LCD window
         * @param title     Window title
         * @param pos       Window position (x, y)
         * @param size      Window size (width, height)
         * @param engine    Calculation engine for state/history
         * @param layers    Layer manager for mode display
         */
        LVGL_LCD_Display( const std::string&            title,
                          int                           x,
                          int                           y,
                          int                           width,
                          int                           height,
                          const ovb::math::Calc_Engine& engine,
                          const ovb::core::Layer_Manager& layers );

        ~LVGL_LCD_Display();

        /// @brief Window dimensions
        int  width()  const;
        int  height() const;

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
