/**
 * @file    i_display_manager.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Display manager interface
 *
 * Orchestrates the rendering pipeline:
 * - Receives updates from Expression (AST changes)
 * - Receives updates from Calc_Engine (result changes)
 * - Manages cursor position tracking
 * - Coordinates layout engine + renderer + canvas
 * - Handles dirty region optimization
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/display/i_canvas.hpp>
#include <overboard/display/i_layout_engine.hpp>
#include <overboard/display/i_renderer.hpp>
#include <overboard/math/ast/node.hpp>

namespace ovb::display {

/**
 * @brief Display state for a single calculation entry
 */
struct Display_Entry {
    math::ast::Node::ptr_t    ast;           ///< Expression AST
    std::string               result;        ///< Result string (empty if not evaluated)
    bool                      has_result;    ///< Whether result is available
};

/**
 * @brief Display manager interface
 *
 * Central coordinator for the display pipeline:
 *
 * Input:
 *   - set_expression(): New AST from Expression class
 *   - set_cursor(): Cursor position from Expression
 *   - add_history_entry(): Completed calculation
 *
 * Processing:
 *   - build(): AST → Layout_Box tree
 *   - measure(): Compute box sizes
 *   - layout(): Position boxes
 *   - render(): Draw to canvas
 *
 * Output:
 *   - Updates to I_Canvas
 *   - Dirty region tracking for efficiency
 */
class I_Display_Manager {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_Display_Manager() = default;

        /**
         * @brief Initialize with canvas and renderer
         * @param canvas Canvas for drawing
         * @param renderer Renderer for layout boxes
         * @param layout_engine Layout engine for AST conversion
         */
        virtual void initialize(I_Canvas*       canvas,
                                I_Renderer*     renderer,
                                I_Layout_Engine* layout_engine) = 0;

        /**
         * @brief Set current expression AST
         * @param ast Expression AST root
         */
        virtual void set_expression(const math::ast::Node::ptr_t& ast) = 0;

        /**
         * @brief Set cursor position for highlighting
         * @param cursor_pos Cursor position in glyph units
         */
        virtual void set_cursor(std::size_t cursor_pos) = 0;

        /**
         * @brief Clear cursor highlighting
         */
        virtual void clear_cursor() = 0;

        /**
         * @brief Set evaluation result string
         * @param result Result to display (empty to clear)
         */
        virtual void set_result(const std::string& result) = 0;

        /**
         * @brief Add completed entry to history
         * @param entry Display entry with AST and result
         */
        virtual void add_history_entry(const Display_Entry& entry) = 0;

        /**
         * @brief Clear all history entries
         */
        virtual void clear_history() = 0;

        /**
         * @brief Refresh the entire display
         *
         * Forces complete re-render of expression, result, and history.
         * Use sparingly - prefer update() for incremental updates.
         */
        virtual void refresh() = 0;

        /**
         * @brief Update dirty regions only
         *
         * Efficient incremental update. Only redraws regions
         * marked as dirty since last update.
         */
        virtual void update() = 0;

        /**
         * @brief Check if display needs refresh
         * @return true if dirty regions exist
         */
        virtual bool needs_update() const = 0;

        /**
         * @brief Set canvas dimensions (e.g., on resize)
         * @param width Canvas width in pixels
         * @param height Canvas height in pixels
         */
        virtual void set_size(int width, int height) = 0;
};

} // namespace ovb::display

