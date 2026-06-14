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
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*****************************/
/*       Constructor         */
/*****************************/
LCD_Section::LCD_Section(const math::Calc_Engine& engine, const core::Layer_Manager& layers)
    : m_engine(engine), m_layers(layers) {}

/*****************************/
/*        Destructor         */
/*****************************/
LCD_Section::~LCD_Section() {
    LOG_DEBUG("LCD_Section: destructor");
    // Delete LVGL objects before m_canvas_buf is freed to prevent use-after-free
    if (m_preview_canvas) {
        lv_obj_del(m_preview_canvas);
        m_preview_canvas = nullptr;
    }
    if (m_table) {
        lv_obj_del(m_table);
        m_table = nullptr;
    }
    if (m_bezel) {
        lv_obj_del(m_bezel);
        m_bezel = nullptr;
    }
    LOG_DEBUG("LCD_Section: destructor complete");
}

/*****************************/
/*        LCD Section        */
/*****************************/
void LCD_Section::build(lv_obj_t* parent, int avail_w, int avail_h) {
    LOG_DEBUG("LCD_Section::build: starting, size=", std::to_string(avail_w), "x", std::to_string(avail_h));

    m_bezel = lv_obj_create(parent);
    lv_obj_set_size(m_bezel, avail_w - 8, avail_h - 8);
    lv_obj_align(m_bezel, LV_ALIGN_TOP_LEFT, 4, 4);

    // Bezel styling
    lv_obj_set_style_bg_color(m_bezel, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_color(m_bezel, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_bezel, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(m_bezel, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_bezel, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(m_bezel, lvgl_color(LVGL_COLOR_SHADOW), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(m_bezel, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(m_bezel, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_shadow_spread(m_bezel, 1, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(m_bezel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(m_bezel, LV_OBJ_FLAG_SCROLLABLE);

    // History table - positioned above current entry
    m_table = lv_table_create(m_bezel);
    int history_height = avail_h - 16 - hal::PREVIEW_MAX_HEIGHT - 8;  // Remaining space above preview
    lv_obj_set_size(m_table, avail_w - 16, history_height);
    lv_obj_align(m_table, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(m_table, lvgl_color(LVGL_COLOR_BG_TABLE), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_table, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_table, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_top(m_table, 2, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_bottom(m_table, 2, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_left(m_table, 4, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_right(m_table, 4, static_cast<uint32_t>(LV_PART_ITEMS));

    // Column setup - input on left, result on right
    lv_table_set_column_width(m_table, 0, (avail_w - 24) * 2 / 3);
    lv_table_set_column_width(m_table, 1, (avail_w - 24) / 3);
    lv_obj_set_scrollbar_mode(m_table, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(m_table, LV_DIR_VER);

    // Header row
    lv_table_set_cell_value(m_table, 0, 0, "Expression");
    lv_table_set_cell_value(m_table, 0, 1, "Result");

    // Table cell styling
    const auto table_parts = static_cast<uint32_t>(LV_PART_ITEMS) | static_cast<uint32_t>(LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(m_table, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), table_parts);
    lv_obj_set_style_bg_color(m_table, lvgl_color(LVGL_COLOR_BG_TABLE), table_parts);

    // Header styling
    const auto header_parts = static_cast<uint32_t>(LV_PART_ITEMS) | static_cast<uint32_t>(LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(m_table, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), header_parts);
    lv_obj_set_style_bg_color(m_table, lvgl_color(LVGL_COLOR_BG_BEZEL), header_parts);
    lv_obj_set_style_text_font(m_table, &lv_font_montserrat_12, header_parts);

    // Preview canvas - current entry at bottom, full width
    m_preview_canvas = lv_canvas_create(m_bezel);
    lv_obj_set_size(m_preview_canvas, hal::PREVIEW_MAX_WIDTH, hal::PREVIEW_MAX_HEIGHT);
    lv_obj_align(m_preview_canvas, LV_ALIGN_BOTTOM_LEFT, 4, 4);
    m_canvas_buf.resize(static_cast<size_t>(hal::PREVIEW_MAX_WIDTH * hal::PREVIEW_MAX_HEIGHT));
    std::fill(m_canvas_buf.begin(), m_canvas_buf.end(), 0xFFF8F8F8);  // Fill with canvas bg color
    lv_canvas_set_buffer(m_preview_canvas, m_canvas_buf.data(),
                         hal::PREVIEW_MAX_WIDTH, hal::PREVIEW_MAX_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_set_style_radius(m_preview_canvas, 4, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(m_preview_canvas, LV_OPA_TRANSP, LV_PART_MAIN);

#ifdef OVB_DEBUG_LAYOUT
    // Green border = expression canvas region
    lv_obj_set_style_border_width(m_preview_canvas, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_preview_canvas, lv_color_hex(0x00CC00), LV_PART_MAIN);
    lv_obj_set_style_border_opa(m_preview_canvas, LV_OPA_COVER, LV_PART_MAIN);

    // Red box overlay = result region (bottom-right half of canvas)
    lv_obj_t* result_debug = lv_obj_create(m_bezel);
    lv_obj_set_size(result_debug, hal::PREVIEW_MAX_WIDTH / 2, hal::PREVIEW_MAX_HEIGHT / 2);
    lv_obj_align(result_debug, LV_ALIGN_BOTTOM_RIGHT, -4, -4);
    lv_obj_set_style_border_width(result_debug, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(result_debug, lv_color_hex(0xCC0000), LV_PART_MAIN);
    lv_obj_set_style_border_opa(result_debug, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(result_debug, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_radius(result_debug, 0, LV_PART_MAIN);
    lv_obj_clear_flag(result_debug, LV_OBJ_FLAG_SCROLLABLE);
#else
    lv_obj_set_style_border_width(m_preview_canvas, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_preview_canvas, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
#endif

    // Initialize layout engine here, after LVGL fonts are loaded
    LOG_DEBUG("LCD_Section::build: creating layout_engine");
    m_layout_engine.emplace(font::Font_Metrics::make_from_lv_font(LVGL_FONT_DEFAULT), 1);
    LOG_DEBUG("LCD_Section::build: complete");
}

/***************************/
/*     Update History      */
/***************************/
static void update_history_table(lv_obj_t* table, const std::deque<math::History_Entry>& history) {
    // Set row count: header row (1) + history entries
    uint32_t new_row_count = 1 + static_cast<uint32_t>(history.size());
    lv_table_set_row_count(table, new_row_count);

    // Update history entries (newest at top, row 1 is first entry after header)
    for (size_t i = 0; i < history.size(); ++i) {
        uint32_t row = static_cast<uint32_t>(i + 1);  // Row 0 is header
        lv_table_set_cell_value(table, row, 0, history[i].input.c_str());
        lv_table_set_cell_value(table, row, 1, history[i].result.c_str());
    }
}

/***************************/
/*        LCD Refresh        */
/***************************/
void LCD_Section::refresh() {
    LOG_DEBUG("LCD_Section::refresh() called");

    // Update history table
    update_history_table(m_table, m_engine.state().history);

    // Get AST from expression
    const auto& ast = m_engine.state().expression.ast_root();
    LOG_DEBUG("AST retrieved from expression");

    // Only show result in lower right if evaluation was performed
    std::string result_str;
    if (m_engine.result_shown()) {
        result_str = m_engine.state().display_value;
        LOG_DEBUG("Result string: " + result_str);
    }

    // Draw typeset math with result in lower right (if available)
    int pw = hal::PREVIEW_MAX_WIDTH;
    int ph = hal::PREVIEW_MAX_HEIGHT;
    draw_math_to_canvas(m_preview_canvas, pw, ph, *m_layout_engine, ast, result_str);

    lv_obj_invalidate(m_preview_canvas);
    lv_obj_invalidate(m_bezel);
    LOG_DEBUG("Canvas invalidated");
}

} // namespace ovb::gui
