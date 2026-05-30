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
#include <stdexcept>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>
#include <overboard/math/layout/box.hpp>
#include <overboard/math/parser.hpp>

namespace ovb::gui {

static log::Stdout_Logger s_math_logger(log::Log_Level::Debug);

/************************************/
/*       Draw Math To Canvas        */
/************************************/
void draw_math_to_canvas( lv_obj_t*              canvas,
                          int                    width,
                          int                    height,
                          math::layout::Layout_Engine& layout_engine,
                          const std::string&     expr_str )
{
    // Set canvas buffer if not already set
    static std::vector<uint32_t> canvas_buf;
    if (canvas_buf.size() != static_cast<size_t>(width * height)) {
        canvas_buf.resize(static_cast<size_t>(width * height));
        lv_canvas_set_buffer(canvas, canvas_buf.data(), width, height, LV_COLOR_FORMAT_ARGB8888);
    }

    // Clear canvas to light background
    lv_canvas_fill_bg(canvas, lvgl_color(LVGL_COLOR_BG_CANVAS), LV_OPA_COVER);

    if (expr_str.empty()) return;

    // Parse and layout the expression
    try {
        math::Parser parser(expr_str);
        auto ast = parser.parse();
        auto box = layout_engine.build(ast.get());
        layout_engine.prepare(box, {width, height});

        // Create draw layer for canvas
        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        // Draw boxes recursively
        std::function<void(const math::layout::Layout_Box&, int, int)> draw_box;
        draw_box = [&](const math::layout::Layout_Box& b, int offset_x, int offset_y) {
            int x = b.pos.x + offset_x;
            int y = b.pos.y + offset_y;

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
                    label_dsc.text = b.text.c_str();
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

            for (const auto& child : b.children) {
                draw_box(child, offset_x, offset_y);
            }
        };

        draw_box(box, 0, 0);
        lv_canvas_finish_layer(canvas, &layer);

    } catch (const std::exception& e) {
        s_math_logger.error("Math render error: " + std::string(e.what()) +
                            " | Expression: " + expr_str);

        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
        label_dsc.font  = LVGL_FONT_DEFAULT;
        label_dsc.opa   = LV_OPA_COVER;

        lv_area_t coords = {
            10,
            static_cast<int32_t>(height / 2 - 10),
            static_cast<int32_t>(width - 10),
            static_cast<int32_t>(height / 2 + 20)
        };
        label_dsc.text = expr_str.c_str();
        lv_draw_label(&layer, &label_dsc, &coords);

        lv_canvas_finish_layer(canvas, &layer);
    }
}

} // namespace ovb::gui
