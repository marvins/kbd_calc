/**
 * @file    display_controller.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Implementation of the Display_Controller class.
 *
 * @details This file contains the implementation of the Display_Controller class,
 *          which is responsible for rendering the keyboard and LCD displays.
 */
#include <overboard/core/display_controller.hpp>

// C++ Standard Libraries
#include <functional>
#include <string>

// Project Libraries
#include <overboard/core/point.hpp>
#include <overboard/hal/lcd_config.hpp>
#include <overboard/math/parser.hpp>

namespace ovb::core {

/****************************/
/*     Constructor          */
/****************************/
Display_Controller::Display_Controller( I_Display&           kbd_display,
                                        I_Display&           lcd_display,
                                        const Layer_Manager& layers,
                                        const math::Calc_Engine& engine )
    : m_kbd_display(kbd_display), m_lcd_display(lcd_display),
      m_layers(layers), m_engine(engine), m_layout(Grid_Layout::standard_5x6()) {}

Display_Controller::Display_Controller( I_Display&           kbd_display,
                                        I_Display&           lcd_display,
                                        const Layer_Manager& layers,
                                        const math::Calc_Engine& engine,
                                        Grid_Layout          layout )
    : m_kbd_display(kbd_display), m_lcd_display(lcd_display),
      m_layers(layers), m_engine(engine), m_layout(std::move(layout)) {}

/*****************************************/
/*          Set the Pressed Key          */
/*****************************************/
void Display_Controller::set_pressed_key(int key_index) {
    m_pressed_key = key_index;
}

/*****************************************/
/*          Clear the Pressed Key        */
/*****************************************/
void Display_Controller::clear_pressed_key() {
    m_pressed_key = -1;
}

/****************************/
/*       Main Render        */
/****************************/
void Display_Controller::render() {
    render_keyboard();
    render_lcd();
}

void Display_Controller::render_keyboard() {
    const auto& layer     = m_layers.current_layer();
    const int   key_count = static_cast<int>(layer.keys.size());

    const int header_h = 22;
    const int grid_x   = MARGIN_LEFT;
    const int grid_y   = header_h + MARGIN_TOP;
    const int grid_w   = m_kbd_display.width()  - MARGIN_LEFT - KEY_PAD;
    const int grid_h   = m_kbd_display.height() - grid_y      - KEY_PAD;

    // Calculate cell size based on layout grid dimensions
    const int cell_w   = grid_w / m_layout.cols();
    const int cell_h   = grid_h / m_layout.rows();

    m_kbd_display.clear(Color{30, 30, 30});

    // ── Header bar ──────────────────────────────────────────────────────────
    std::string header = "Layer: " + std::string(layer.name);
    m_kbd_display.draw_rect(Point<int>(0, 0), Point<int>(m_kbd_display.width(), header_h), Color{50, 50, 80}, true);
    m_kbd_display.draw_text(Point<int>(MARGIN_LEFT + KEY_PAD, 4), header, Color::white(), Color{50, 50, 80}, 1);

    // ── Col ID labels (0–layout.cols-1) ────────────────────────────────────
    for (int c = 0; c < m_layout.cols(); ++c) {
        std::string lbl = std::to_string(c);
        int tx = grid_x + c * cell_w + cell_w / 2 - 3;
        m_kbd_display.draw_text(Point<int>(tx, header_h + 2), lbl, Color{160, 160, 160}, Color{30, 30, 30}, 1);
    }

    // ── Row ID labels (0–layout.rows-1) ──────────────────────────────────────
    for (int r = 0; r < m_layout.rows(); ++r) {
        std::string lbl = std::to_string(r);
        int ty = grid_y + r * cell_h + cell_h / 2 - 4;
        m_kbd_display.draw_text(Point<int>(4, ty), lbl, Color{160, 160, 160}, Color{30, 30, 30}, 1);
    }

    // ── Draw grid cells (optional visualization) ────────────────────────────
    // Draw faint cell borders to show the grid structure
    for (int r = 0; r < m_layout.rows(); ++r) {
        for (int c = 0; c < m_layout.cols(); ++c) {
            int x = grid_x + c * cell_w;
            int y = grid_y + r * cell_h;
            // Draw cell outline (very faint)
            m_kbd_display.draw_rect(Point<int>(x, y), Point<int>(cell_w, 1), Color{50, 50, 50}, true);
            m_kbd_display.draw_rect(Point<int>(x, y), Point<int>(1, cell_h), Color{50, 50, 50}, true);
        }
    }

    // ── Keys ────────────────────────────────────────────────────────────────
    // Only draw keys that are KEY_START cells (not continuations)
    for (int i = 0; i < key_count; ++i) {
        auto pos_opt = m_layout.get_key_position(i);
        if (!pos_opt) continue;

        const auto& pos = *pos_opt;

        // Skip if this key doesn't start at this cell (it's a continuation)
        Cell_Type cell_type = m_layout.get_cell_type(pos.col, pos.row);
        if (cell_type != Cell_Type::KEY_START) continue;

        // Calculate key rectangle
        auto rect_opt = m_layout.get_key_rect(i, Point<int>(grid_x, grid_y),
                                                 Point<int>(cell_w, cell_h), KEY_PAD);
        if (!rect_opt) continue;

        draw_key(layer.keys[static_cast<std::size_t>(i)], *rect_opt, i == m_pressed_key);
    }

    m_kbd_display.flush();
}

/****************************/
/*       Key Drawing        */
/****************************/
void Display_Controller::draw_key( Key_Code        code,
                                   core::Rect<int> rect,
                                   bool            pressed ) {
    int x = rect.x;
    int y = rect.y;
    int w = rect.w;
    int h = rect.h;

    Color bg     = pressed ? Color{180, 180, 60} : Color{60, 60, 80};
    Color fg     = pressed ? Color::black()      : Color::white();
    Color border = Color{100, 100, 130};

    // Draw border and fill
    m_kbd_display.draw_rect(Point<int>(x, y), Point<int>(w, h), border, true);
    m_kbd_display.draw_rect(Point<int>(x + 1, y + 1), Point<int>(w - 2, h - 2), bg, true);

    const std::string lbl = key_code_to_display(code);
    if (!lbl.empty()) {
        constexpr int KEY_FONT_SCALE = 2;
        constexpr int CHAR_W = 6 * KEY_FONT_SCALE;
        constexpr int CHAR_H = 7 * KEY_FONT_SCALE;
        // Count codepoints (not bytes) for correct centering of Unicode labels
        int glyphs = 0;
        for (size_t i = 0; i < lbl.size(); ) {
            uint8_t b = static_cast<uint8_t>(lbl[i]);
            if      (b < 0x80)                    { i += 1; }
            else if ((b & 0xE0) == 0xC0)          { i += 2; }
            else if ((b & 0xF0) == 0xE0)          { i += 3; }
            else                                  { i += 4; }
            ++glyphs;
        }
        int tx = x + w / 2 - glyphs * CHAR_W / 2;
        int ty = y + h / 2 - CHAR_H / 2;
        m_kbd_display.draw_text(Point<int>(tx, ty), lbl, fg, bg, KEY_FONT_SCALE);
    }
}

/******************************/
/*       LCD Rendering        */
/******************************/
static std::string pretty_expr(const std::string& expr) {
    std::string out;
    out.reserve(expr.size());
    for (size_t i = 0; i < expr.size(); ) {
        if (expr.compare(i, 3, "phi") == 0) { out += "\xCF\x86"; i += 3; } // φ
        else if (expr.compare(i, 3, "tau") == 0) { out += "\xCF\x84"; i += 3; } // τ
        else if (expr.compare(i, 2, "pi")  == 0) { out += "\xCF\x80"; i += 2; } // π
        else { out += expr[i++]; }
    }
    return out;
}

/****************************/
/*       Mode Panel         */
/****************************/
void Display_Controller::draw_mode_panel(Point<int> pos) {
    const auto& st = m_engine.state();
    const char* mode_text = st.use_math_layout ? "MATH" : "LINE";
    Color mode_color = st.use_math_layout ? Color{50, 200, 100} : Color{200, 200, 100};
    m_lcd_display.draw_text(pos, mode_text, mode_color, Color::black(), 1);
}

void Display_Controller::render_math_expression(Point<int> pos, Point<int> max_size) {
    const auto& st = m_engine.state();
    if (!st.last_ast || !st.use_math_layout) return;

    auto box = m_layout_engine.build(st.last_ast.get());
    m_layout_engine.prepare(box, max_size);

    // Simple rendering: just draw boxes as outlines for now
    std::function<void(const ovb::layout::Layout_Box&, Point<int>)> draw_box;
    draw_box = [&](const ovb::layout::Layout_Box& b, Point<int> offset) {
        auto box_pos = b.pos + offset;
        m_lcd_display.draw_rect(box_pos, b.size, Color{100, 100, 100}, false);
        for (const auto& child : b.children) {
            draw_box(child, offset);
        }
    };
    draw_box(box, pos);
}

/****************************/
/*     LCD Main Render      */
/****************************/
void Display_Controller::render_lcd() {
    const auto& st = m_engine.state();

    m_lcd_display.clear(Color::black());

    // ─── Mode panel (top right) ──────────────────────────────────────────────
    draw_mode_panel(Point<int>(m_lcd_display.width() - 40, 4));

    // ─── History entries (top portion, NSpire style) ─────────────────────────
    constexpr int HISTORY_SCALE = 2;
    constexpr int LINE_H = 14;     // 12px font (scale 2) + 2px spacing
    constexpr int ENTRY_H = 36;    // height per history entry (input + result + gap)
    constexpr int HISTORY_Y_START = 4;
    constexpr int MAX_VISIBLE_HISTORY = 2;

    for (size_t i = 0; i < st.history.size() && i < MAX_VISIBLE_HISTORY; ++i) {
        const auto& entry = st.history[i];
        int y = HISTORY_Y_START + static_cast<int>(i) * ENTRY_H;

        // Input (upper left)
        std::string pretty_input = pretty_expr(entry.input);
        m_lcd_display.draw_text(Point<int>(8, y), pretty_input, Color{140, 140, 140}, Color::black(), HISTORY_SCALE);

        // Result (lower right, aligned to right edge)
        int result_w = static_cast<int>(entry.result.size()) * 6 * HISTORY_SCALE;
        int result_x = m_lcd_display.width() - result_w - 8;
        m_lcd_display.draw_text(Point<int>(result_x, y + LINE_H), entry.result, Color::white(), Color::black(), HISTORY_SCALE);

        // Separator line
        int sep_y = y + ENTRY_H - 2;
        m_lcd_display.draw_rect(Point<int>(8, sep_y), Point<int>(m_lcd_display.width() - 16, 1), Color{60, 60, 60}, true);
    }

    int visible_history = static_cast<int>(std::min(st.history.size(), static_cast<size_t>(MAX_VISIBLE_HISTORY)));
    int current_y = HISTORY_Y_START + visible_history * ENTRY_H + 8;

    // ─── Current working entry (NSpire style: left input, lower-right result) ─
    constexpr int WORK_SCALE = 2;  // working equation font size
    constexpr int RESULT_SCALE = 3;  // larger result after = pressed

    // Expression line with cursor (upper left)
    Color expr_fg = Color{200, 200, 200};
    std::string full_expr  = pretty_expr(st.expression.render_string());
    int         cursor_pos = st.expression.cursor_glyph_pos();
    // Split the pretty string at the cursor glyph boundary.
    std::size_t byte_off = 0;
    int g = 0;
    while (g < cursor_pos && byte_off < full_expr.size()) {
        uint8_t b = static_cast<uint8_t>(full_expr[byte_off]);
        if      (b < 0x80)           byte_off += 1;
        else if ((b & 0xE0) == 0xC0) byte_off += 2;
        else if ((b & 0xF0) == 0xE0) byte_off += 3;
        else                         byte_off += 4;
        ++g;
    }
    std::string before_cursor = full_expr.substr(0, byte_off);
    std::string after_cursor  = full_expr.substr(byte_off);
    int cursor_x = 8 + cursor_pos * 6 * WORK_SCALE;
    m_lcd_display.draw_text(Point<int>(8, current_y), before_cursor, expr_fg, Color::black(), WORK_SCALE);
    m_lcd_display.draw_rect(Point<int>(cursor_x, current_y - 2), Point<int>(2 * WORK_SCALE, 10), Color::white(), true);
    m_lcd_display.draw_text(Point<int>(cursor_x + 3, current_y), after_cursor, expr_fg, Color::black(), WORK_SCALE);

    // Result (lower right, after = pressed) — shown when result is available
    const std::string& val = st.error.empty() ? st.display_value : st.error;
    if (!val.empty() && val != "0") {  // show result if we have one
        Color val_fg = st.error.empty() ? Color::white() : Color::red();
        int result_w = static_cast<int>(val.size()) * 6 * RESULT_SCALE;
        int result_x = m_lcd_display.width() - result_w - 8;
        int result_y = current_y + 20;  // lower-right positioning
        m_lcd_display.draw_text(Point<int>(result_x, result_y), val, val_fg, Color::black(), RESULT_SCALE);
    }

    // ─── Memory indicator ───────────────────────────────────────────────────
    if (m_engine.state().memory != 0.0) {
        m_lcd_display.draw_text(Point<int>( 8,
                                m_lcd_display.height() - 20),
                                            "M", Color::yellow(),
                                            Color::black(), 1);
    }

    // ─── Layer indicator bottom-right ───────────────────────────────────────
    std::string layer_label = std::string(m_layers.current_layer().name);
    int lx = m_lcd_display.width() - static_cast<int>(layer_label.size()) * 6 - 8;
    m_lcd_display.draw_text( Point<int>( lx, m_lcd_display.height() - 20),
                             layer_label,
                             Color::light_blue(),
                             Color::black(),
                             1);

    // ─── Math layout preview (if enabled and result shown) ──────────────────
    if (st.use_math_layout && !st.last_ast && !st.expression.empty()) {

        // Build AST from current expression for preview
        try {
            math::Parser p(st.expression.eval_string());
            auto preview_ast = p.parse();
            auto box = m_layout_engine.build(preview_ast.get());
            m_layout_engine.prepare(box, Point<int>(hal::PREVIEW_MAX_WIDTH, hal::PREVIEW_MAX_HEIGHT));

            // Draw preview at bottom center
            int preview_x = (m_lcd_display.width() - box.size.x) / 2;
            int preview_y = m_lcd_display.height() - hal::PREVIEW_OFFSET_Y;
            std::function<void(const ovb::layout::Layout_Box&, Point<int>)> draw_box;

            draw_box = [&](const ovb::layout::Layout_Box& b, Point<int> offset) {
                auto pos = b.pos + offset;
                // Draw text for ATOM boxes
                if (b.kind == ovb::layout::Box_Kind::ATOM && !b.text.empty()) {
                    m_lcd_display.draw_text(pos, b.text, Color::white(), Color::black(), static_cast<int>(b.scale));
                }
                for (const auto& child : b.children) {
                    draw_box(child, offset);
                }
            };
            draw_box(box, Point<int>(preview_x, preview_y));
        } catch (...) {
            // Ignore parse errors for preview
        }
    }

    m_lcd_display.flush();
}

} // namespace ovb::core
