/**
 * @file    i_canvas.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Abstract canvas interface for display rendering
 *
 * Platform-agnostic drawing surface. Implementations handle
 * the actual pixel operations (LVGL, SDL, frame buffer, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <string>

// Project Libraries
#include <overboard/core/point.hpp>
#include <overboard/core/rect.hpp>

namespace ovb::display {

/**
 * @brief Abstract canvas for drawing operations
 *
 * Provides platform-agnostic drawing primitives:
 * - Text rendering at (x,y)
 * - Line drawing
 * - Rectangle fill/outline
 * - Image blitting
 * - Clipping regions
 *
 * Implementations: LVGL_Canvas, SDL_Canvas, Framebuffer_Canvas
 */
class I_Canvas {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_Canvas() = default;

        /**
         * @brief Get canvas dimensions
         * @return Width and height in pixels
         */
        virtual core::Point<int> size() const = 0;

        /**
         * @brief Clear entire canvas to background color
         */
        virtual void clear() = 0;

        /**
         * @brief Set clipping rectangle for subsequent drawing
         * @param clip Clipping region, or empty to disable clipping
         */
        virtual void set_clip(const core::Rect<int>& clip) = 0;

        /**
         * @brief Remove clipping rectangle
         */
        virtual void clear_clip() = 0;

        /**
         * @brief Draw text at specified position
         * @param text String to draw
         * @param pos Position in pixels (top-left of text)
         * @param font_size Font size in pixels
         * @param color RGB color value
         */
        virtual void draw_text(const std::string& text,
                               core::Point<int>   pos,
                               int                font_size,
                               uint32_t           color) = 0;

        /**
         * @brief Draw horizontal or vertical line
         * @param start Start position
         * @param end End position
         * @param color RGB color value
         * @param thickness Line thickness in pixels
         */
        virtual void draw_line(core::Point<int> start,
                               core::Point<int> end,
                               uint32_t         color,
                               int              thickness = 1) = 0;

        /**
         * @brief Fill rectangle with solid color
         * @param rect Rectangle to fill
         * @param color RGB color value
         */
        virtual void fill_rect(const core::Rect<int>& rect,
                               uint32_t               color) = 0;

        /**
         * @brief Draw rectangle outline
         * @param rect Rectangle to outline
         * @param color RGB color value
         * @param thickness Border thickness in pixels
         */
        virtual void draw_rect(const core::Rect<int>& rect,
                               uint32_t               color,
                               int                    thickness = 1) = 0;

        /**
         * @brief Mark a region as dirty (needs redraw)
         * @param region Dirty rectangle
         */
        virtual void mark_dirty(const core::Rect<int>& region) = 0;

        /**
         * @brief Flush pending drawing operations to screen
         */
        virtual void flush() = 0;
};

} // namespace ovb::display

