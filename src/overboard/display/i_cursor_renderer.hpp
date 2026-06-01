/**
 * @file    i_cursor_renderer.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Cursor renderer interface
 *
 * Renders a visual cursor indicator at the current
 * cursor position within a layout box tree.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>

// Project Libraries
#include <overboard/display/i_canvas.hpp>
#include <overboard/math/layout/box.hpp>

namespace ovb::display {

/**
 * @brief Cursor rendering style
 */
enum class Cursor_Style {
    Block,      ///< Filled rectangle covering the character
    Underline,  ///< Line beneath the character
    Vertical_Bar, ///< Vertical line before the character
};

/**
 * @brief Cursor renderer interface
 *
 * Draws a visual cursor indicator at the specified
 * layout box position. Supports multiple cursor styles.
 */
class I_Cursor_Renderer {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_Cursor_Renderer() = default;

        /**
         * @brief Render cursor at box position
         * @param canvas Target canvas
         * @param box Box containing the cursor
         * @param color Cursor color (RGB)
         */
        virtual void render(I_Canvas&                       canvas,
                            const math::layout::Layout_Box& box,
                            uint32_t                        color) = 0;

        /**
         * @brief Set cursor style
         * @param style Visual style for cursor
         */
        virtual void set_style(Cursor_Style style) = 0;

        /**
         * @brief Get current cursor style
         * @return Current style
         */
        virtual Cursor_Style style() const = 0;

        /**
         * @brief Set cursor blink state
         * @param visible true for visible, false for hidden
         */
        virtual void set_visible(bool visible) = 0;

        /**
         * @brief Check if cursor is currently visible
         * @return Blink visibility state
         */
        virtual bool visible() const = 0;
};

} // namespace ovb::display

