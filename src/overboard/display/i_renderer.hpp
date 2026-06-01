/**
 * @file    i_renderer.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Abstract renderer interface for layout boxes
 *
 * Renders a layout box tree to a canvas. Implementations handle
 * the specific drawing commands for different platforms.
 */
#pragma once

// C++ Standard Libraries
#include <memory>

// Project Libraries
#include <overboard/display/i_canvas.hpp>
#include <overboard/math/layout/box.hpp>

namespace ovb::display {

/**
 * @brief Layout box renderer interface
 *
 * Renders a tree of Layout_Box objects to an I_Canvas.
 * The renderer traverses the box tree and calls appropriate
 * canvas drawing primitives for each box type.
 *
 * Box types handled:
 * - GLYPH: Single character/text
 * - SEQUENCE: Horizontal list of boxes
 * - FRACTION: Numerator over denominator
 * - POWER: Base with superscript
 * - ROOT: Radical with optional index
 */
class I_Renderer {

    public:

        /**
         * @brief Virtual destructor
         */
        virtual ~I_Renderer() = default;

        /**
         * @brief Render a layout box tree to canvas
         * @param canvas Target canvas for drawing
         * @param box Root of layout box tree
         */
        virtual void render(I_Canvas& canvas, const math::layout::Layout_Box& box) = 0;

        /**
         * @brief Set the cursor position for rendering
         * @param box Cursor box (will be highlighted/drawn specially)
         */
        virtual void set_cursor(const math::layout::Layout_Box* box) = 0;

        /**
         * @brief Clear cursor highlighting
         */
        virtual void clear_cursor() = 0;

        /**
         * @brief Check if a box is the current cursor box
         * @param box Box to check
         * @return true if this box should be rendered as cursor
         */
        virtual bool is_cursor(const math::layout::Layout_Box& box) const = 0;
};

} // namespace ovb::display

