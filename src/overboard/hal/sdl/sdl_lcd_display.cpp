/**
 * @file    sdl_lcd_display.cpp
 * @author  Marvin Smith
 * @date    2026-05-21
 *
 * @brief   SDL LCD display implementation
 */
#include <overboard/hal/sdl/sdl_lcd_display.hpp>

// C++ Standard Libraries
#include <functional>
#include <string>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>
#include <lvgl/src/drivers/sdl/lv_sdl_window.h>

// Project Libraries
#include <overboard/hal/sdl/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/math/calc_engine.hpp>
#include <overboard/math/expression.hpp>
#include <overboard/math/layout/engine.hpp>
#include <overboard/font/font_metrics.hpp>
#include <overboard/math/parser.hpp>

namespace ovb::hal::sdl {

// Static logger instance for this module
static log::Stdout_Logger s_logger(log::Log_Level::Debug);

// Forward declarations for draw helpers
static void draw_math_to_canvas( lv_obj_t* canvas, int width, int height,
                                 layout::Layout_Engine& layout_engine,
                                 const std::string&          expr_str);

struct SDL_LCD_Display::Impl {
    SDL_Window*   sdl_window   = nullptr;
    lv_display_t* lv_display   = nullptr;
    int           width        = 0;
    int           height       = 0;

    // LVGL widgets
    lv_obj_t*     screen       = nullptr;
    lv_obj_t*     bezel        = nullptr;
    lv_obj_t*     table        = nullptr;
    lv_obj_t*     preview_area = nullptr;
    lv_obj_t*     preview_canvas = nullptr;  // Canvas for typeset math

    // References
    const math::Calc_Engine&   engine;
    const core::Layer_Manager& layers;
    layout::Layout_Engine      layout_engine{font::Font_Metrics::make_default(), 2};  // Scale factor 2

    // History canvases (one per visible row, expression and result)
    std::vector<std::pair<lv_obj_t*, lv_obj_t*>> history_cells;

    Impl( int w, int h,
          const math::Calc_Engine& e,
          const core::Layer_Manager& l)
        : width(w), height(h), engine(e), layers(l) {}
};

/************************************/
/*          Constructor             */
/************************************/
SDL_LCD_Display::SDL_LCD_Display( const std::string&              title,
                                     int                             x,
                                     int                             y,
                                     int                             width,
                                     int                             height,
                                     const math::Calc_Engine&   engine,
                                     const core::Layer_Manager& layers )
    : m_impl(std::make_unique<Impl>(width, height, engine, layers))
{
    // Create LVGL SDL display (LVGL v9 creates its own SDL window)
    m_impl->lv_display = lv_sdl_window_create(width, height);
    lv_display_set_default(m_impl->lv_display);

    // Get the SDL window handle for event filtering
    m_impl->sdl_window = lv_sdl_window_get_window(m_impl->lv_display);

    // Position and title the window
    SDL_SetWindowPosition(m_impl->sdl_window, x, y);
    SDL_SetWindowTitle(m_impl->sdl_window, title.c_str());

    m_impl->screen = lv_screen_active();

    // Clear default styling
    lv_obj_set_style_pad_all(m_impl->screen, 0, 0);
    lv_obj_set_style_border_width(m_impl->screen, 0, 0);
    lv_obj_set_style_bg_color(m_impl->screen, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);

    // Create bezel container
    m_impl->bezel = lv_obj_create(m_impl->screen);
    lv_obj_set_size(m_impl->bezel, width - 8, height - 8);
    lv_obj_align(m_impl->bezel, LV_ALIGN_CENTER, 0, 0);

    // Bezel styling: light background, subtle border, soft shadow
    lv_obj_set_style_bg_color(m_impl->bezel, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->bezel, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->bezel, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->bezel, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->bezel, 4, LV_PART_MAIN);

    // Soft shadow effect
    lv_obj_set_style_shadow_color(m_impl->bezel, lvgl_color(LVGL_COLOR_SHADOW), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(m_impl->bezel, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(m_impl->bezel, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_shadow_spread(m_impl->bezel, 1, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(m_impl->bezel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(m_impl->bezel, LV_OBJ_FLAG_SCROLLABLE);

    // Create history table
    m_impl->table = lv_table_create(m_impl->bezel);
    lv_obj_set_size(m_impl->table, width - 16, height - 60);
    lv_obj_align(m_impl->table, LV_ALIGN_TOP_MID, 0, 24);

    // Table styling - light theme
    lv_obj_set_style_bg_color(m_impl->table, lvgl_color(LVGL_COLOR_BG_TABLE), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->table, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->table, 0, LV_PART_MAIN);

    // Compact header styling
    lv_obj_set_style_pad_top(m_impl->table, 2, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_bottom(m_impl->table, 2, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_left(m_impl->table, 4, static_cast<uint32_t>(LV_PART_ITEMS));
    lv_obj_set_style_pad_right(m_impl->table, 4, static_cast<uint32_t>(LV_PART_ITEMS));

    // Column setup: expression (wider), result
    lv_table_set_column_width(m_impl->table, 0, (width - 24) * 2 / 3);
    lv_table_set_column_width(m_impl->table, 1, (width - 24) / 3);

    // Enable scrolling but hide scrollbar when not needed
    lv_obj_set_scrollbar_mode(m_impl->table, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(m_impl->table, LV_DIR_VER);

    // Header row
    lv_table_set_cell_value(m_impl->table, 0, 0, "Expression");
    lv_table_set_cell_value(m_impl->table, 0, 1, "Result");

    // Table cell styling - dark text on light background
    const auto table_parts = static_cast<uint32_t>(LV_PART_ITEMS) | static_cast<uint32_t>(LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(m_impl->table, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), table_parts);
    lv_obj_set_style_bg_color(m_impl->table, lvgl_color(LVGL_COLOR_BG_TABLE), table_parts);

    // Preview area for current expression (below table)
    m_impl->preview_area = lv_obj_create(m_impl->bezel);
    lv_obj_set_size(m_impl->preview_area, width - 24, 32);
    lv_obj_align(m_impl->preview_area, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_set_style_bg_color(m_impl->preview_area, lvgl_color(LVGL_COLOR_BG_PREVIEW), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->preview_area, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->preview_area, lvgl_color(LVGL_COLOR_BORDER_LIGHT), LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->preview_area, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->preview_area, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(m_impl->preview_area, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(m_impl->preview_area, LV_OBJ_FLAG_SCROLLABLE);

    // Preview canvas for typeset math
    m_impl->preview_canvas = lv_canvas_create(m_impl->preview_area);
    lv_obj_align(m_impl->preview_canvas, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(m_impl->preview_canvas, width - 28, 28);

    // Mode indicator label (top left of bezel)
    lv_obj_t* mode_label = lv_label_create(m_impl->bezel);
    lv_label_set_text(mode_label, "MATH");
    lv_obj_align(mode_label, LV_ALIGN_TOP_LEFT, 4, 4);
    lv_obj_set_style_text_color(mode_label, lvgl_color(LVGL_COLOR_ACCENT_GREEN), LV_PART_MAIN);
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_12, LV_PART_MAIN);

    // Layer indicator (top right)
    lv_obj_t* layer_label = lv_label_create(m_impl->bezel);
    lv_label_set_text(layer_label, std::string(layers.current_layer().name).c_str());
    lv_obj_align(layer_label, LV_ALIGN_TOP_RIGHT, -4, 4);
    lv_obj_set_style_text_color(layer_label, lvgl_color(LVGL_COLOR_ACCENT_BLUE), LV_PART_MAIN);
    lv_obj_set_style_text_font(layer_label, &lv_font_montserrat_12, LV_PART_MAIN);

    // Initial population
    refresh();
}

/************************************/
/*         Destructor               */
/************************************/
SDL_LCD_Display::~SDL_LCD_Display() = default;

/************************************/
/*         Getters                  */
/************************************/
int  SDL_LCD_Display::width()  const { return m_impl->width; }
int  SDL_LCD_Display::height() const { return m_impl->height; }

void SDL_LCD_Display::clear( [[maybe_unused]] Color c ) {
    // LVGL handles clearing via redraw
}

/********************************/
/*          Draw a Pixel        */
/********************************/
void SDL_LCD_Display::draw_pixel( [[maybe_unused]] core::Point<int> pos,
                                  [[maybe_unused]] Color c ) {
    // LVGL handles rendering via widgets
}

/****************************************/
/*          Draw a rectangle            */
/****************************************/
void SDL_LCD_Display::draw_rect( [[maybe_unused]] core::Point<int> pos,
                                 [[maybe_unused]] core::Point<int> size,
                                 [[maybe_unused]] Color c,
                                 [[maybe_unused]] bool filled ) {
    // LVGL handles rendering via widgets
}

/*****************************/
/*          Draw Text        */
/*****************************/
void SDL_LCD_Display::draw_text([[maybe_unused]] core::Point<int> pos,
                                 [[maybe_unused]] const std::string& text,
                                 [[maybe_unused]] Color fg, [[maybe_unused]] Color bg, [[maybe_unused]] int scale) {
    // LVGL handles rendering via widgets
}

void SDL_LCD_Display::flush() {
    // LVGL flush is handled by lv_timer_handler()
}

uint32_t SDL_LCD_Display::window_id() const {
    return m_impl->sdl_window ? SDL_GetWindowID(m_impl->sdl_window) : 0;
}

/*****************************************/
/*         Refresh the Display           */
/*****************************************/
void SDL_LCD_Display::refresh() {
    const auto& st = m_impl->engine.state();
    const auto& history = st.history;

    // Clear existing rows (keep header) - LVGL v9 uses row count manipulation
    uint32_t row_count = lv_table_get_row_count(m_impl->table);
    // Reset to just header row by setting row count to 1
    if (row_count > 1) {
        // LVGL v9: recreate table or clear cells since remove_row doesn't exist
        for (uint32_t i = 1; i < row_count; ++i) {
            lv_table_set_cell_value(m_impl->table, i, 0, "");
            lv_table_set_cell_value(m_impl->table, i, 1, "");
        }
    }

    // Add history rows (newest first) - limit to visible rows
    const uint32_t max_rows = 8;
    uint32_t start_idx = history.size() > max_rows
                         ? static_cast<uint32_t>(history.size() - max_rows) : 0;

    for (uint32_t idx = static_cast<uint32_t>(history.size()); idx > start_idx; --idx) {
        const auto& entry = history[idx - 1];
        uint32_t row = static_cast<uint32_t>(history.size()) - idx + 1;  // Row 0 is header

        // LVGL v9: set cell value directly, rows auto-expand
        lv_table_set_cell_value(m_impl->table, row, 0, entry.input.c_str());
        lv_table_set_cell_value(m_impl->table, row, 1, entry.result.c_str());
    }

    // Update preview area with current expression typeset
    if (!st.expression.empty()) {
        draw_math_to_canvas( m_impl->preview_canvas, m_impl->width - 28, 28,
                             m_impl->layout_engine,
                             st.expression.eval_string() );
    }
}

/*****************************************/
/*         Render the Display            */
/*****************************************/
void SDL_LCD_Display::render() {
    lv_display_set_default(m_impl->lv_display);
    lv_timer_handler();
}

/*****************************************/
/*         Draw Math to Canvas           */
/*****************************************/
// Helper: draw typeset math to LVGL canvas using layer-based rendering (LVGL v9)
static void draw_math_to_canvas(lv_obj_t* canvas, int width, int height,
                                 layout::Layout_Engine& layout_engine,
                                 const std::string&          expr_str) {
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
        s_logger.error("Math render error: " + std::string(e.what()) + " | Expression: " + expr_str);

        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
        label_dsc.font = LVGL_FONT_DEFAULT;
        label_dsc.opa = LV_OPA_COVER;

        lv_area_t coords = {4, 4, width - 4, height - 4};
        // Show raw expression text instead of "Error" so user sees partial input
        label_dsc.text = expr_str.c_str();
        lv_draw_label(&layer, &label_dsc, &coords);

        lv_canvas_finish_layer(canvas, &layer);
    } catch (...) {
        // Parse error - log it and show raw expression as fallback (unknown exception type)
        s_logger.error("Math render error: unknown exception | Expression: " + expr_str);

        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lvgl_color(LVGL_COLOR_TEXT_PRIMARY);
        label_dsc.font = LVGL_FONT_DEFAULT;
        label_dsc.opa = LV_OPA_COVER;

        lv_area_t coords = {4, 4, width - 4, height - 4};
        // Show raw expression text instead of "Error" so user sees partial input
        label_dsc.text = expr_str.c_str();
        lv_draw_label(&layer, &label_dsc, &coords);

        lv_canvas_finish_layer(canvas, &layer);
    }
}

} // namespace ovb::hal::sdl
