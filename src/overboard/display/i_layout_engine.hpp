/**
 * @file    i_layout_engine.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Abstract layout engine interface
 *
 * Converts AST nodes to layout box trees and performs
 * measurement and positioning passes.
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Project Libraries
#include <overboard/core/point.hpp>
#include <overboard/font/font_metrics.hpp>
#include <overboard/math/ast/node.hpp>
#include <overboard/math/layout/box.hpp>

namespace ovb::display {

/**
 * @brief Layout engine interface for AST to box conversion
 *
 * Converts mathematical AST nodes to a tree of Layout_Box objects
 * suitable for rendering. Performs two-pass layout:
 *
 * 1. Measure pass (bottom-up): Computes w, h, baseline
 * 2. Position pass (top-down): Computes x, y coordinates
 */
class I_Layout_Engine {

    public:

        /**
         * @brief Virtual destructor
         */
        virtual ~I_Layout_Engine() = default;

        /**
         * @brief Build layout box tree from AST
         * @param node AST root node
         * @return Layout_Box Root of layout tree
         */
        virtual math::layout::Layout_Box build(const math::ast::Node* node) = 0;

        /**
         * @brief Build layout with explicit scale
         * @param node AST node
         * @param scale Font scale for this subtree
         * @return Layout_Box Layout representation
         */
        virtual math::layout::Layout_Box build(const math::ast::Node* node,
                                               float                  scale) = 0;

        /**
         * @brief Measure box sizes (bottom-up pass)
         * @param box Root box to measure
         */
        virtual void measure(math::layout::Layout_Box& box) = 0;

        /**
         * @brief Position boxes (top-down pass)
         * @param box Root box to position
         * @param pos Container-relative position
         */
        virtual void layout(math::layout::Layout_Box& box,
                            core::Point<int>          pos) = 0;

        /**
         * @brief Combined measure + center + layout
         * @param box Box tree to prepare
         * @param container_size Container dimensions
         */
        virtual void prepare(math::layout::Layout_Box& box,
                             core::Point<int>          container_size) = 0;

        /**
         * @brief Find box at cursor position
         * @param box Root of box tree
         * @param cursor_pos Cursor position in glyph units
         * @return Pointer to box at cursor, or nullptr
         */
        virtual const math::layout::Layout_Box* find_box_at_cursor(
            const math::layout::Layout_Box& box,
            std::size_t                     cursor_pos) const = 0;
};

} // namespace ovb::display

