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
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/gui/math_canvas.hpp>
#include <overboard/hal/display_config.hpp>

namespace ovb::gui {

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

} // namespace ovb::gui
