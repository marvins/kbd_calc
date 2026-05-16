#include <overboard/core/display_controller.hpp>
#include <string>

Display_Controller::Display_Controller(I_Display& kbd_display, I_Display& lcd_display,
                                       const Layer_Manager& layers, const Calc_Engine& engine)
    : m_kbd_display(kbd_display), m_lcd_display(lcd_display),
      m_layers(layers), m_engine(engine) {}

void Display_Controller::set_pressed_key(int key_index) {
    m_pressed_key = key_index;
}

void Display_Controller::clear_pressed_key() {
    m_pressed_key = -1;
}

void Display_Controller::render() {
    render_keyboard();
    render_lcd();
}

void Display_Controller::render_keyboard() {
    const auto& layer     = m_layers.current_layer();
    const int   key_count = static_cast<int>(layer.keys.size());
    const int   rows      = (key_count + COLS - 1) / COLS;

    const int header_h = 22;
    const int grid_x   = MARGIN_LEFT;
    const int grid_y   = header_h + MARGIN_TOP;
    const int grid_w   = m_kbd_display.width()  - MARGIN_LEFT - KEY_PAD;
    const int grid_h   = m_kbd_display.height() - grid_y      - KEY_PAD;
    const int cell_w   = grid_w / COLS;
    const int cell_h   = grid_h / rows;

    m_kbd_display.clear(Color{30, 30, 30});

    // ── Header bar ──────────────────────────────────────────────────────────
    std::string header = "Layer: " + std::string(layer.name);
    m_kbd_display.draw_rect(0, 0, m_kbd_display.width(), header_h, Color{50, 50, 80}, true);
    m_kbd_display.draw_text(MARGIN_LEFT + KEY_PAD, 4, header, Color::white(), Color{50, 50, 80}, 1);

    // ── Col ID labels (0–COLS-1) ─────────────────────────────────────────────
    for (int c = 0; c < COLS; ++c) {
        std::string lbl = std::to_string(c);
        int tx = grid_x + c * cell_w + cell_w / 2 - 3;
        m_kbd_display.draw_text(tx, header_h + 2, lbl, Color{160, 160, 160}, Color{30, 30, 30}, 1);
    }

    // ── Row ID labels (0–rows-1) ─────────────────────────────────────────────
    for (int r = 0; r < rows; ++r) {
        std::string lbl = std::to_string(r);
        int ty = grid_y + r * cell_h + cell_h / 2 - 4;
        m_kbd_display.draw_text(4, ty, lbl, Color{160, 160, 160}, Color{30, 30, 30}, 1);
    }

    // ── Keys ────────────────────────────────────────────────────────────────
    for (int i = 0; i < key_count; ++i) {
        draw_key(i, layer.keys[static_cast<std::size_t>(i)], i == m_pressed_key);
    }

    m_kbd_display.flush();
}

void Display_Controller::draw_key(int index, const Key_Def& key, bool pressed) {
    const auto& layer     = m_layers.current_layer();
    const int   key_count = static_cast<int>(layer.keys.size());
    const int   rows      = (key_count + COLS - 1) / COLS;

    const int header_h = 22;
    const int grid_x   = MARGIN_LEFT;
    const int grid_y   = header_h + MARGIN_TOP;
    const int grid_w   = m_kbd_display.width()  - MARGIN_LEFT - KEY_PAD;
    const int grid_h   = m_kbd_display.height() - grid_y      - KEY_PAD;
    const int cell_w   = grid_w / COLS;
    const int cell_h   = grid_h / rows;

    int col = index % COLS;
    int row = index / COLS;
    int x   = grid_x + col * cell_w + KEY_PAD / 2;
    int y   = grid_y + row * cell_h + KEY_PAD / 2;
    int w   = cell_w - KEY_PAD;
    int h   = cell_h - KEY_PAD;

    Color bg     = pressed ? Color{180, 180, 60} : Color{60, 60, 80};
    Color fg     = pressed ? Color::black()      : Color::white();
    Color border = Color{100, 100, 130};

    m_kbd_display.draw_rect(x, y, w, h, border, true);
    m_kbd_display.draw_rect(x + 1, y + 1, w - 2, h - 2, bg, true);

    const std::string lbl = label_string(key.label);
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
        m_kbd_display.draw_text(tx, ty, lbl, fg, bg, KEY_FONT_SCALE);
    }
}

/********************************/
/*          Render LCD          */
/********************************/

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


void Display_Controller::render_lcd() {
    const auto& st = m_engine.state();

    m_lcd_display.clear(Color::black());

    // Expression line with cursor
    Color expr_fg = Color{160, 160, 160};
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
    int cursor_x = 8 + cursor_pos * 6;
    m_lcd_display.draw_text(8, 8, before_cursor, expr_fg, Color::black(), 1);
    m_lcd_display.draw_rect(cursor_x, 6, 2, 10, Color::white(), true);
    m_lcd_display.draw_text(cursor_x + 3, 8, after_cursor, expr_fg, Color::black(), 1);

    // Main display value (large)
    const std::string& val = st.error.empty() ? st.display_value : st.error;
    Color val_fg = st.error.empty() ? Color::white() : Color::red();
    int scale = 3;
    int text_w = static_cast<int>(val.size()) * 6 * scale;
    int tx = m_lcd_display.width() - text_w - 8;
    if (tx < 8) tx = 8;
    m_lcd_display.draw_text(tx, 40, val, val_fg, Color::black(), scale);

    // Memory indicator
    if (m_engine.state().memory != 0.0) {
        m_lcd_display.draw_text(8, m_lcd_display.height() - 20, "M", Color::yellow(), Color::black(), 1);
    }

    // Layer indicator bottom-right
    std::string layer_label = std::string(m_layers.current_layer().name);
    int lx = m_lcd_display.width() - static_cast<int>(layer_label.size()) * 6 - 8;
    m_lcd_display.draw_text(lx, m_lcd_display.height() - 20, layer_label, Color{100, 100, 200}, Color::black(), 1);

    m_lcd_display.flush();
}
