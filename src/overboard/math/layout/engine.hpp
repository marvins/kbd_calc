/**
 * @file    engine.hpp
 * @author  Marvin Smith
 * @date    2025-10-19
 *
 * @brief Layout engine for math expression typesetting
 *
 * Converts AST nodes to layout box trees and performs measurement
 * and positioning passes.
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Project Libraries
#include <overboard/core/point.hpp>
#include <overboard/font/font_metrics.hpp>
#include <overboard/math/ast/binary_op_node.hpp>
#include <overboard/math/ast/function_node.hpp>
#include <overboard/math/ast/node.hpp>
#include <overboard/math/ast/unary_op_node.hpp>
#include <overboard/math/layout/box.hpp>

namespace ovb::math::layout {

/**
 * @brief Converts AST to layout boxes and performs layout
 *
 * The Layout_Engine builds a tree of Layout_Box objects from an AST,
 * then performs two passes:
 *
 * 1. measure(): Computes size (w, h) and baseline for each box bottom-up
 * 2. layout():  Computes position (x, y) for each box top-down
 *
 * Usage:
 * @code
 *   Layout_Engine engine(2);  // scale 2
 *   auto box = engine.build(ast_root);
 *   engine.prepare(box, 480, 240);  // measure + center in container
 *   // Now render the box tree...
 * @endcode
 */
class Layout_Engine {

    public:

        /**
         * @brief Construct with font metrics
         *
         * Font metrics drive all box sizing so layout matches rendering.
         *
         * @param metrics  Font metrics from the actual font being used
         * @param default_scale Font scale for top-level boxes (default 2)
         */
        explicit Layout_Engine( const font::Font_Metrics& metrics,
                                float                     default_scale = 2.0f );

        /**
         * @brief Build layout box tree from AST
         *
         * Recursively converts the AST to a Layout_Box tree.
         * The returned box has zero positions and uncomputed sizes.
         * Call measure() and layout() before rendering.
         *
         * @param node AST root node
         * @return Layout_Box Root of layout tree
         */
        Layout_Box build( const ast::Node* node );

        /**
         * @brief Build layout with explicit scale
         *
         * @param node AST node
         * @param scale Font scale for this subtree
         * @return Layout_Box Root of layout tree
         */
        Layout_Box build( const ast::Node* node, float scale );

        /**
         * @brief Measure box sizes (bottom-up pass)
         *
         * Computes w, h, and baseline for box and all descendants.
         * Must be called before layout().
         *
         * @param box Root box to measure
         */
        void measure(Layout_Box& box);

        /**
         * @brief Position boxes (top-down pass)
         *
         * Computes x, y for box and all descendants.
         * measure() must be called first.
         *
         * @param box Root box to position
         * @param pos Container-relative position as Point
         */
        void layout(Layout_Box& box, core::Point<int> pos);

        /**
         * @brief Combined measure + center + layout
         *
         * Convenience method that:
         * 1. Calls measure()
         * 2. Centers the box in the container
         * 3. Calls layout() with centered position
         *
         * @param box Box tree to prepare
         * @param container_size Container dimensions as Point
         */
        void prepare(Layout_Box& box, core::Point<int> container_size);

    private:
        font::Font_Metrics m_metrics;       ///< Font metrics for box sizing
        float              m_default_scale; ///< Default font scale

        /**
         * @brief Build layout for binary operation node
         *
         * Handles special cases:
         * - DIVIDE: Creates FRACTION box
         * - POWER: Creates POWER box
         * - Others: Creates SEQUENCE with operator
         *
         * @param node Binary operation AST node
         * @param scale Font scale
         * @return Layout_Box Layout representation
         */
        Layout_Box build_binary_op( const ast::Binary_Op_Node* node, float scale );

        /**
         * @brief Build layout for unary operation node
         *
         * Creates SEQUENCE with operator prefix for negation.
         *
         * @param node Unary operation AST node
         * @param scale Font scale
         * @return Layout_Box Layout representation
         */
        Layout_Box build_unary_op( const ast::Unary_Op_Node* node, float scale );

        /**
         * @brief Build layout for function node
         *
         * Creates SEQUENCE with "name(args...)" structure.
         * Arguments are recursively laid out.
         *
         * @param node Function AST node
         * @param scale Font scale
         * @return Layout_Box Layout representation
         */
        Layout_Box build_function( const ast::Function_Node* node, float scale );
};

} // namespace ovb::math::layout
