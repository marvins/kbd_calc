/**
 * @file    math_canvas.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LVGL canvas renderer for typeset mathematical expressions
 *
 * Parses an expression string, builds a layout box hierarchy via the
 * Layout_Engine, and renders it onto an LVGL canvas object.
 */
#pragma once

// C++ Standard Libraries
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/math/layout/engine.hpp>

namespace ovb::gui {

/**
 * @brief Render a typeset mathematical expression to an LVGL canvas
 *
 * Builds a layout box tree from @p ast and draws it onto @p canvas.
 * On build failure, draws nothing.
 *
 * @param canvas        Target LVGL canvas object
 * @param width         Canvas width in pixels
 * @param height        Canvas height in pixels
 * @param layout_engine Layout engine for box construction and positioning
 * @param ast           AST node to render (upper left)
 * @param result_str    Result string to render (lower right), empty if no result
 */
void draw_math_to_canvas( lv_obj_t*                     canvas,
                          int                           width,
                          int                           height,
                          math::layout::Layout_Engine&  layout_engine,
                          const math::ast::Node::ptr_t& ast,
                          const std::string&            result_str = "" );

} // namespace ovb::gui
