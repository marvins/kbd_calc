/**
 * @file    math_canvas.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LVGL canvas renderer for typeset mathematical expressions
 */
#include <overboard/gui/math_canvas.hpp>

// C++ Standard Libraries
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/math/layout/box.hpp>

namespace ovb::gui {

/************************************/
/*       Draw Math To Canvas        */
/************************************/
bool draw_math_to_canvas( lv_obj_t*                     canvas,
                          int                           width,
                          int                           height,
                          math::layout::Layout_Engine&  layout_engine,
                          const math::ast::Node::ptr_t& ast,
                          const std::string&            result_str,
                          const math::ast::Node*        cursor_node )
{
    // Create draw layer for canvas
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    // Clear canvas to background through the layer so the clear and glyphs
    // are in the same draw batch (lv_canvas_fill_bg is synchronous and
    // races with the async layer flush)
    lv_draw_rect_dsc_t bg_dsc;
    lv_draw_rect_dsc_init(&bg_dsc);
    bg_dsc.bg_color = lvgl_color(LVGL_COLOR_BG_CANVAS);
    bg_dsc.bg_opa   = LV_OPA_COVER;
    const lv_area_t bg_area = { 0, 0,
        static_cast<int32_t>(width  - 1),
        static_cast<int32_t>(height - 1) };
    lv_draw_rect(&layer, &bg_dsc, &bg_area);

    if (!ast) {
        lv_canvas_finish_layer(canvas, &layer);
        return false;
    }

    // Build layout from AST (upper left)
    auto box = layout_engine.build(ast.get());
    // Measure and center in canvas (original behavior)
    layout_engine.prepare(box, {width, height});

    // Calculate scroll offset to keep cursor in view
    int scroll_x = 0;
    int scroll_y = 0;
    if (cursor_node) {
        auto cursor_pos = layout_engine.find_node_position(box, cursor_node);
        if (cursor_pos) {
            // Scroll to keep cursor in center of canvas
            scroll_x = (width / 2) - cursor_pos->x;
            scroll_y = (height / 2) - cursor_pos->y;
        }
    }

    // Draw boxes recursively with scroll offset
    std::function<void(const math::layout::Layout_Box&, int, int)> draw_box;
    draw_box = [&](const math::layout::Layout_Box& b, int offset_x, int offset_y) {
            int x = b.pos.x + offset_x + scroll_x;
            int y = b.pos.y + offset_y + scroll_y;

            if (b.kind == math::layout::Box_Kind::ATOM) {
                if (!b.text.empty()) {
                    lv_draw_label_dsc_t label_dsc;
                    lv_draw_label_dsc_init(&label_dsc);
                    label_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
                    label_dsc.font  = LVGL_FONT_DEFAULT;
                    label_dsc.opa   = LV_OPA_COVER;

                    lv_area_t coords = {
                        static_cast<int32_t>(x),
                        static_cast<int32_t>(y),
                        static_cast<int32_t>(x + b.size.x),
                        static_cast<int32_t>(y + b.size.y)
                    };
                    label_dsc.text       = b.text.c_str();
                    label_dsc.text_local = 1;
                    lv_draw_label(&layer, &label_dsc, &coords);
                } else {
                    // Draw placeholder outline box
                    lv_draw_rect_dsc_t rect_dsc;
                    lv_draw_rect_dsc_init(&rect_dsc);
                    rect_dsc.bg_color = lvgl_color(LVGL_COLOR_BG_CANVAS);
                    rect_dsc.bg_opa   = LV_OPA_COVER;
                    rect_dsc.border_color = lvgl_color(LVGL_COLOR_BORDER_MEDIUM);
                    rect_dsc.border_opa   = LV_OPA_COVER;
                    rect_dsc.border_width = 1;

                    lv_area_t coords = {
                        static_cast<int32_t>(x),
                        static_cast<int32_t>(y),
                        static_cast<int32_t>(x + b.size.x),
                        static_cast<int32_t>(y + b.size.y)
                    };
                    lv_draw_rect(&layer, &rect_dsc, &coords);
                }
            }

            if (b.kind == math::layout::Box_Kind::FRACTION && b.children.size() == 2) {
                const auto& num = b.children[0];
                const auto& den = b.children[1];
                int bar_y     = y + num.size.y + 2;
                int bar_width = std::max(num.size.x, den.size.x);

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
                rect_dsc.bg_opa   = LV_OPA_COVER;

                lv_area_t bar_coords = {
                    static_cast<int32_t>(x + (b.size.x - bar_width) / 2),
                    static_cast<int32_t>(bar_y),
                    static_cast<int32_t>(x + (b.size.x + bar_width) / 2),
                    static_cast<int32_t>(bar_y + 2)
                };
                lv_draw_rect(&layer, &rect_dsc, &bar_coords);
            }

            if (b.kind == math::layout::Box_Kind::SQRT && !b.children.empty()) {
                int symbol_width = 2 * static_cast<int>(b.scale);
                int top_pad = 2 * static_cast<int>(b.scale);
                int text_offset = 2;

                // Draw horizontal bar over the argument
                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
                rect_dsc.bg_opa   = LV_OPA_COVER;

                lv_area_t bar_coords = {
                    static_cast<int32_t>(x + symbol_width),
                    static_cast<int32_t>(y + top_pad),
                    static_cast<int32_t>(x + b.size.x),
                    static_cast<int32_t>(y + top_pad + 2)
                };
                lv_draw_rect(&layer, &rect_dsc, &bar_coords);

                // Draw √ symbol (vertical line and tick)
                lv_draw_line_dsc_t line_dsc;
                lv_draw_line_dsc_init(&line_dsc);
                line_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
                line_dsc.width = 1;
                line_dsc.opa   = LV_OPA_COVER;

                // Vertical line
                line_dsc.p1 = { static_cast<lv_value_precise_t>(x + 1), static_cast<lv_value_precise_t>(y + top_pad) };
                line_dsc.p2 = { static_cast<lv_value_precise_t>(x + 1), static_cast<lv_value_precise_t>(y + b.size.y - text_offset) };
                lv_draw_line(&layer, &line_dsc);

                // Tick mark (diagonal)
                line_dsc.p1 = { static_cast<lv_value_precise_t>(x + 1), static_cast<lv_value_precise_t>(y + b.size.y - text_offset) };
                line_dsc.p2 = { static_cast<lv_value_precise_t>(x + symbol_width), static_cast<lv_value_precise_t>(y + top_pad) };
                lv_draw_line(&layer, &line_dsc);
            }

            for (const auto& child : b.children) {
                draw_box(child, offset_x, offset_y);
            }
        };

        draw_box(box, 0, 0);

    // Draw result in lower right if present
    if (!result_str.empty()) {
        lv_draw_label_dsc_t result_dsc;
        lv_draw_label_dsc_init(&result_dsc);
        result_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
        result_dsc.font  = LVGL_FONT_DEFAULT;
        result_dsc.opa   = LV_OPA_COVER;

        // Result in lower right
        lv_area_t result_coords = {
            static_cast<int32_t>(width / 2),   // Start at 50% width
            static_cast<int32_t>(height / 2),  // Start at 50% height
            static_cast<int32_t>(width - 10),
            static_cast<int32_t>(height - 10)
        };
        result_dsc.text = result_str.c_str();
        lv_draw_label(&layer, &result_dsc, &result_coords);
    }

    lv_canvas_finish_layer(canvas, &layer);
    return true;
}

} // namespace ovb::gui
