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
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/math/layout/box.hpp>

namespace ovb::gui {

namespace {

/*****************************/
/*         Make Area         */
/*****************************/
inline lv_area_t make_area(int x1, int y1, int x2, int y2) {
    return { static_cast<int32_t>(x1),
             static_cast<int32_t>(y1),
             static_cast<int32_t>(x2),
             static_cast<int32_t>(y2) };
}

/*****************************/
/*        Draw Cursor        */
/*****************************/
static void draw_cursor( lv_layer_t&                      layer,
                         const math::layout::Layout_Box&  b,
                         int                              x,
                         int                              y,
                         uint32_t                         color )
{
    lv_draw_rect_dsc_t hl_dsc;
    lv_draw_rect_dsc_init(&hl_dsc);
    hl_dsc.bg_color = lvgl_color(color);
    hl_dsc.bg_opa   = LV_OPA_COVER;
    hl_dsc.radius   = 2;
    const lv_area_t area = make_area(x - 1, y - 1, x + b.size.x + 1, y + b.size.y + 1);
    lv_draw_rect(&layer, &hl_dsc, &area);
}

/*****************************/
/*         Draw Atom         */
/*****************************/
static void draw_atom( lv_layer_t&                      layer,
                       const math::layout::Layout_Box&  b,
                       int                              x,
                       int                              y )
{
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color      = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
    label_dsc.font       = LVGL_FONT_DEFAULT;
    label_dsc.opa        = LV_OPA_COVER;
    label_dsc.text       = b.text.c_str();
    label_dsc.text_local = 1;
    lv_area_t coords = make_area(x, y, x + b.size.x, y + b.size.y);
    lv_draw_label(&layer, &label_dsc, &coords);
}

/*********************************/
/*      Draw Placeholder         */
/*********************************/
static void draw_placeholder( lv_layer_t& layer,
                               int         x,
                               int         y,
                               int         w,
                               int         h )
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color     = lvgl_color(LVGL_COLOR_BG_CANVAS);
    rect_dsc.bg_opa       = LV_OPA_COVER;
    rect_dsc.border_color = lvgl_color(LVGL_COLOR_BORDER_MEDIUM);
    rect_dsc.border_opa   = LV_OPA_COVER;
    rect_dsc.border_width = 1;
    lv_area_t coords = make_area(x, y, x + w, y + h);
    lv_draw_rect(&layer, &rect_dsc, &coords);
}

/*********************************/
/*      Draw Fraction Bar        */
/*********************************/
static void draw_fraction_bar( lv_layer_t&                      layer,
                               const math::layout::Layout_Box&  b,
                               int                              x,
                               int                              y )
{
    const auto& num   = b.children[0];
    int bar_y         = y + num.size.y + 2;
    int bar_width     = std::max(num.size.x, b.children[1].size.x);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
    rect_dsc.bg_opa   = LV_OPA_COVER;
    lv_area_t bar_coords = make_area( x + (b.size.x - bar_width) / 2,
                                      bar_y,
                                      x + (b.size.x + bar_width) / 2,
                                      bar_y + 2 );
    lv_draw_rect(&layer, &rect_dsc, &bar_coords);
}

/*****************************/
/*        Draw Sqrt          */
/*****************************/
static void draw_sqrt( lv_layer_t&                      layer,
                       const math::layout::Layout_Box&  b,
                       int                              x,
                       int                              y )
{
    const int symbol_width = 2 * static_cast<int>(b.scale);
    const int top_pad      = 2 * static_cast<int>(b.scale);
    const int text_offset  = 2;

    // Horizontal bar over the argument
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
    rect_dsc.bg_opa   = LV_OPA_COVER;
    lv_area_t bar_coords = make_area( x + symbol_width, y + top_pad,
                                      x + b.size.x,     y + top_pad + 2 );
    lv_draw_rect(&layer, &rect_dsc, &bar_coords);

    // √ symbol — vertical line and diagonal tick
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
    line_dsc.width = 1;
    line_dsc.opa   = LV_OPA_COVER;

    line_dsc.p1 = { static_cast<lv_value_precise_t>(x + 1), static_cast<lv_value_precise_t>(y + top_pad) };
    line_dsc.p2 = { static_cast<lv_value_precise_t>(x + 1), static_cast<lv_value_precise_t>(y + b.size.y - text_offset) };
    lv_draw_line(&layer, &line_dsc);

    line_dsc.p1 = { static_cast<lv_value_precise_t>(x + 1),            static_cast<lv_value_precise_t>(y + b.size.y - text_offset) };
    line_dsc.p2 = { static_cast<lv_value_precise_t>(x + symbol_width), static_cast<lv_value_precise_t>(y + top_pad) };
    lv_draw_line(&layer, &line_dsc);
}

/*****************************/
/*       Box Renderer        */
/*****************************/
struct Box_Renderer {
    lv_layer_t&            layer;
    const math::ast::Node* cursor_node;
    uint32_t               cursor_color;
    int                    scroll_x;
    int                    scroll_y;

    void draw(const math::layout::Layout_Box& b) const {
        const int x = b.pos.x + scroll_x;
        const int y = b.pos.y + scroll_y;

        if (cursor_node && b.node_ptr == cursor_node && b.size.x > 0 && b.size.y > 0) {
            draw_cursor(layer, b, x, y, cursor_color);
        }

        switch (b.kind) {
            case math::layout::Box_Kind::ATOM:
                if (!b.text.empty()) {
                    draw_atom(layer, b, x, y);
                } else {
                    draw_placeholder(layer, x, y, b.size.x, b.size.y);
                }
                break;

            case math::layout::Box_Kind::FRACTION:
                if (b.children.size() == 2) {
                    draw_fraction_bar(layer, b, x, y);
                }
                break;

            case math::layout::Box_Kind::SQRT:
                if (!b.children.empty()) {
                    draw_sqrt(layer, b, x, y);
                }
                break;

            default:
                break;
        }

        for (const auto& child : b.children) {
            draw(child);
        }
    }
};

} // anonymous namespace

/************************************/
/*       Draw Math To Canvas        */
/************************************/
bool draw_math_to_canvas( lv_obj_t*                     canvas,
                          int                           width,
                          int                           height,
                          math::layout::Layout_Engine&  layout_engine,
                          const math::ast::Node::ptr_t& ast,
                          const std::string&            result_str,
                          const math::ast::Node*        cursor_node,
                          uint32_t                      cursor_highlight_color )
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
    const lv_area_t bg_area = make_area(0, 0, width - 1, height - 1);
    lv_draw_rect(&layer, &bg_dsc, &bg_area);

    if (!ast) {
        lv_canvas_finish_layer(canvas, &layer);
        return false;
    }

    // Build layout from AST and position within canvas
    auto box = layout_engine.build(ast.get());
    layout_engine.prepare(box, {width, height});

    // Calculate scroll offset to keep cursor in view
    int scroll_x = 0;
    int scroll_y = 0;
    if (cursor_node) {
        auto cursor_pos = layout_engine.find_node_position(box, cursor_node);
        if (cursor_pos) {
            scroll_x = (width  / 2) - cursor_pos->x;
            scroll_y = (height / 2) - cursor_pos->y;
        }
    }

    Box_Renderer{ layer, cursor_node, cursor_highlight_color, scroll_x, scroll_y }.draw(box);

    // Draw result in lower right if present
    if (!result_str.empty()) {
        lv_draw_label_dsc_t result_dsc;
        lv_draw_label_dsc_init(&result_dsc);
        result_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
        result_dsc.font  = LVGL_FONT_DEFAULT;
        result_dsc.opa   = LV_OPA_COVER;
        result_dsc.text  = result_str.c_str();

        lv_area_t result_coords = make_area( width / 2, height / 2,
                                             width - 10, height - 10 );
        lv_draw_label(&layer, &result_dsc, &result_coords);
    }

    lv_canvas_finish_layer(canvas, &layer);
    return true;
}

} // namespace ovb::gui
