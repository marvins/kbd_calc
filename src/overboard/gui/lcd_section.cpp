/**
 * @file    lcd_section.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LCD section implementation
 */
#include <overboard/gui/lcd_section.hpp>

// C++ Standard Libraries
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/sdl/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>
#include <overboard/math/parser.hpp>

using namespace ovb::hal::sdl;

namespace ovb::gui {

// Static logger for this module (math rendering)
static log::Stdout_Logger s_math_logger(log::Log_Level::Debug);

/**
 * @brief Draw a typeset mathematical expression to an LVGL canvas
 *
 * Parses the expression string, builds a layout box hierarchy, and renders
 * it to the canvas using LVGL's drawing API. Supports fractions, atoms,
 * and basic mathematical notation.
 *
 * @param canvas        Target LVGL canvas object
 * @param width         Canvas width in pixels
 * @param height        Canvas height in pixels
 * @param layout_engine Layout engine for box construction and positioning
 * @param expr_str      Mathematical expression string to render
 */
static void draw_math_to_canvas( lv_obj_t* canvas, int width, int height,
                                 layout::Layout_Engine& layout_engine,
                                 const std::string&          expr_str);

/*****************************/
/*        LCD Section        */
/*****************************/
void LCD_Section::build(lv_obj_t* parent) {
    bezel = lv_obj_create(parent);
    lv_obj_set_size(bezel, hal::LCD_WIDTH - 8, hal::LCD_HEIGHT - 8);
    lv_obj_align(bezel, LV_ALIGN_TOP_LEFT, 4, 4);

    // Bezel styling
    lv_obj_set_style_bg_color(bezel, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_color(bezel, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_border_width(bezel, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(bezel, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(bezel, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(bezel, lvgl_color(LVGL_COLOR_SHADOW), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(bezel, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(bezel, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_shadow_spread(bezel, 1, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(bezel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(bezel, LV_OBJ_FLAG_SCROLLABLE);

    // History table
    table = lv_table_create(bezel);
    lv_obj_set_size(table, hal::LCD_WIDTH - 16, hal::LCD_HEIGHT - 60);
    lv_obj_align(table, LV_ALIGN_TOP_MID, 0, 24);
    lv_obj_set_style_bg_color(table, lvgl_color(LVGL_COLOR_BG_TABLE), LV_PART_MAIN);
    lv_obj_set_style_border_width(table, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(table, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_top(table, 2, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_bottom(table, 2, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_left(table, 4, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_right(table, 4, static_cast<uint32_t>(LV_PART_ITEMS));

    // Column setup
    lv_table_set_column_width(table, 0, (hal::LCD_WIDTH - 24) * 2 / 3);
    lv_table_set_column_width(table, 1, (hal::LCD_WIDTH - 24) / 3);
    lv_obj_set_scrollbar_mode(table, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(table, LV_DIR_VER);

    // Header row
    lv_table_set_cell_value(table, 0, 0, "Expression");
    lv_table_set_cell_value(table, 0, 1, "Result");

    // Table cell styling
    const auto table_parts = static_cast<uint32_t>(LV_PART_ITEMS) | static_cast<uint32_t>(LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(table, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), table_parts);
    lv_obj_set_style_bg_color(table, lvgl_color(LVGL_COLOR_BG_TABLE), table_parts);

    // Header styling
    const auto header_parts = static_cast<uint32_t>(LV_PART_ITEMS) | static_cast<uint32_t>(LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(table, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), header_parts);
    lv_obj_set_style_bg_color(table, lvgl_color(LVGL_COLOR_BG_BEZEL), header_parts);
    lv_obj_set_style_text_font(table, &lv_font_montserrat_12, header_parts);

    // Preview canvas - allocate and clear buffer to prevent garbage display
    preview_canvas = lv_canvas_create(bezel);
    lv_obj_set_size(preview_canvas, hal::PREVIEW_MAX_WIDTH, hal::PREVIEW_MAX_HEIGHT);
    lv_obj_align(preview_canvas, LV_ALIGN_BOTTOM_MID, 0, -hal::PREVIEW_OFFSET_Y);
    canvas_buf.resize(static_cast<size_t>(hal::PREVIEW_MAX_WIDTH * hal::PREVIEW_MAX_HEIGHT));
    std::fill(canvas_buf.begin(), canvas_buf.end(), 0xFFF8F8F8);  // Fill with canvas bg color
    lv_canvas_set_buffer(preview_canvas, canvas_buf.data(),
                         hal::PREVIEW_MAX_WIDTH, hal::PREVIEW_MAX_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_set_style_border_width(preview_canvas, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(preview_canvas, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_radius(preview_canvas, 4, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(preview_canvas, LV_OPA_TRANSP, LV_PART_MAIN);
}

/***************************/
/*        LCD Refresh        */
/***************************/
void LCD_Section::refresh() {
    // Clear preview canvas
    lv_draw_buf_t* draw_buf = lv_canvas_get_draw_buf(preview_canvas);
    if (draw_buf) {
        lv_draw_buf_clear(draw_buf, nullptr);
    }

    // Get expression string
    const std::string& expr_str = engine.state().expression.render_string();

    // Draw typeset math
    int pw = hal::PREVIEW_MAX_WIDTH;
    int ph = hal::PREVIEW_MAX_HEIGHT;
    draw_math_to_canvas(preview_canvas, pw, ph, layout_engine, expr_str);

    lv_obj_invalidate(preview_canvas);
}

/***************************/
/*        Math Drawing        */
/***************************/
static void draw_math_to_canvas( lv_obj_t* canvas, int width, int height,
                                 layout::Layout_Engine& layout_engine,
                                 const std::string&          expr_str)
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
        std::function<void(const layout::Layout_Box&, int, int)> draw_box;
        draw_box = [&](const layout::Layout_Box& b, int offset_x, int offset_y) {
            int x = b.pos.x + offset_x;
            int y = b.pos.y + offset_y;

            if (b.kind == layout::Box_Kind::ATOM && !b.text.empty()) {
                // Draw text using label descriptor
                lv_draw_label_dsc_t label_dsc;
                lv_draw_label_dsc_init(&label_dsc);
                label_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
                label_dsc.font = LVGL_FONT_DEFAULT;
                label_dsc.opa = LV_OPA_COVER;

                lv_area_t coords = {
                    static_cast<int32_t>(x),
                    static_cast<int32_t>(y),
                    static_cast<int32_t>(x + b.size.x),
                    static_cast<int32_t>(y + b.size.y)
                };
                label_dsc.text = b.text.c_str();
                lv_draw_label(&layer, &label_dsc, &coords);
            }

            // Draw fraction bar for FRACTION boxes
            if (b.kind == layout::Box_Kind::FRACTION && b.children.size() == 2) {
                const auto& num = b.children[0];
                const auto& den = b.children[1];
                int bar_y = y + num.size.y + 2;  // Small gap
                int bar_width = std::max(num.size.x, den.size.x);

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
                rect_dsc.bg_opa = LV_OPA_COVER;

                lv_area_t bar_coords = {
                    static_cast<int32_t>(x + (b.size.x - bar_width) / 2),
                    static_cast<int32_t>(bar_y),
                    static_cast<int32_t>(x + (b.size.x + bar_width) / 2),
                    static_cast<int32_t>(bar_y + 2)
                };
                lv_draw_rect(&layer, &rect_dsc, &bar_coords);
            }

            // Recursively draw children
            for (const auto& child : b.children) {
                draw_box(child, offset_x, offset_y);
            }
        };

        draw_box(box, 0, 0);

        // Finalize drawing
        lv_canvas_finish_layer(canvas, &layer);

    } catch (const std::exception& e) {
        // Parse error - log it and show raw expression as fallback
        s_math_logger.error("Math render error: " + std::string(e.what()) + " | Expression: " + expr_str);

        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
        label_dsc.font = LVGL_FONT_DEFAULT;
        label_dsc.opa = LV_OPA_COVER;

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
