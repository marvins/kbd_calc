/**
 * @file    dimension_picker_popup.cpp
 * @author  Marvin Smith
 * @date    2026-07-05
 *
 * @brief   Two-axis dimension spinner popup implementation
 */
#include <overboard/gui/dimension_picker_popup.hpp>

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/***************************/
/*       Constructor       */
/***************************/
Dimension_Picker_Popup::Dimension_Picker_Popup( lv_obj_t*   parent,
                                                 std::string title,
                                                 Confirm_Cb  on_confirm,
                                                 bool        show_cols )
    : m_parent(parent)
    , m_title(std::move(title))
    , m_on_confirm(std::move(on_confirm))
    , m_show_cols(show_cols)
{
}

/***************************/
/*        Destructor       */
/***************************/
Dimension_Picker_Popup::~Dimension_Picker_Popup() {
    hide();
}

/****************************/
/*   Update spinner labels  */
/****************************/
static void update_spinner(lv_obj_t* label, const char* prefix, int value, bool focused) {
    std::string text = std::string(prefix) + ": [ " + std::to_string(value) + " ]";
    lv_label_set_text(label, text.c_str());
    if (focused) {
        lv_obj_set_style_bg_color(label, lv_color_hex(LVGL_COLOR_ACCENT_BLUE), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(label, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_opa(label, LV_OPA_0, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    }
}

/****************************/
/*           Show           */
/****************************/
void Dimension_Picker_Popup::show() {
    if (m_visible) return;

    constexpr int FOOTER_BAR_HEIGHT { 28 };
    constexpr int CONTAINER_W_PCT   { 60 };
    constexpr int CONTAINER_H       { 130 };
    constexpr int ROW_H             { 32 };

    m_container = lv_obj_create(m_parent);
    lv_obj_set_size(m_container, LV_PCT(CONTAINER_W_PCT), CONTAINER_H);
    lv_obj_align(m_container, LV_ALIGN_BOTTOM_MID, 0, -FOOTER_BAR_HEIGHT);
    lv_obj_set_style_bg_color(m_container, lv_color_hex(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_color(m_container, lv_color_hex(LVGL_COLOR_ACCENT_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_container, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(m_container, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_container, 8, LV_PART_MAIN);
    lv_obj_set_flex_flow(m_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(m_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Title
    lv_obj_t* title_lbl = lv_label_create(m_container);
    lv_label_set_text(title_lbl, m_title.c_str());
    lv_obj_set_style_text_font(title_lbl, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_lbl, lv_color_hex(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_set_width(title_lbl, LV_PCT(100));

    // Rows spinner
    m_rows_label = lv_label_create(m_container);
    lv_obj_set_style_text_font(m_rows_label, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_width(m_rows_label, LV_PCT(100));
    lv_obj_set_height(m_rows_label, ROW_H);
    lv_obj_set_style_pad_all(m_rows_label, 4, LV_PART_MAIN);
    lv_obj_set_style_radius(m_rows_label, 4, LV_PART_MAIN);

    // Cols spinner (only if 2D)
    if (m_show_cols) {
        m_cols_label = lv_label_create(m_container);
        lv_obj_set_style_text_font(m_cols_label, LVGL_FONT_DEFAULT, LV_PART_MAIN);
        lv_obj_set_width(m_cols_label, LV_PCT(100));
        lv_obj_set_height(m_cols_label, ROW_H);
        lv_obj_set_style_pad_all(m_cols_label, 4, LV_PART_MAIN);
        lv_obj_set_style_radius(m_cols_label, 4, LV_PART_MAIN);
    }

    // Hint
    m_hint_label = lv_label_create(m_container);
    lv_label_set_text(m_hint_label, "Up/Dn: switch  L/R: change  Enter=OK  Esc=Cancel");
    lv_obj_set_style_text_font(m_hint_label, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_hint_label, lv_color_hex(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
    lv_obj_set_width(m_hint_label, LV_PCT(100));

    render();

    m_visible = true;
    LOG_DEBUG("Dimension_Picker_Popup: shown - ", m_title);
}

/****************************/
/*           Hide           */
/****************************/
void Dimension_Picker_Popup::hide() {
    if (!m_visible) return;
    if (m_container && lv_display_get_next(nullptr) != nullptr) {
        lv_obj_del(m_container);
    }
    m_container  = nullptr;
    m_rows_label = nullptr;
    m_cols_label = nullptr;
    m_hint_label = nullptr;
    m_visible = false;
    LOG_DEBUG("Dimension_Picker_Popup: hidden");
}

/****************************/
/*          Render          */
/****************************/
void Dimension_Picker_Popup::render() {
    if (!m_rows_label) return;
    update_spinner(m_rows_label, "Rows", m_rows, m_focused == 0);
    if (m_cols_label) {
        update_spinner(m_cols_label, "Cols", m_cols, m_focused == 1);
    }
}

/****************************/
/*       Is Visible         */
/****************************/
bool Dimension_Picker_Popup::is_visible() const {
    return m_visible;
}

/****************************/
/*    Handle Input (Key)    */
/****************************/
bool Dimension_Picker_Popup::handle_input([[maybe_unused]] core::Input_Key key) {
    return false; // raw key not used — action codes drive everything
}

/*****************************/
/*  Handle Input (Action)    */
/*****************************/
bool Dimension_Picker_Popup::handle_input(core::Action_Code action) {
    using AC = core::Action_Code;

    constexpr int MAX_DIM { 9 };
    constexpr int MIN_DIM { 1 };

    int& val = (m_focused == 0) ? m_rows : m_cols;

    switch (action) {
        case AC::CURSOR_UP:
        case AC::CURSOR_DOWN:
            if (m_show_cols) {
                m_focused = (m_focused == 0) ? 1 : 0;
                render();
            }
            return true;

        case AC::CURSOR_LEFT:
            if (val > MIN_DIM) { --val; render(); }
            return true;

        case AC::CURSOR_RIGHT:
            if (val < MAX_DIM) { ++val; render(); }
            return true;

        // Digit shortcuts 1–9 set the focused field directly
        case AC::DIGIT_1: val = 1; render(); return true;
        case AC::DIGIT_2: val = 2; render(); return true;
        case AC::DIGIT_3: val = 3; render(); return true;
        case AC::DIGIT_4: val = 4; render(); return true;
        case AC::DIGIT_5: val = 5; render(); return true;
        case AC::DIGIT_6: val = 6; render(); return true;
        case AC::DIGIT_7: val = 7; render(); return true;
        case AC::DIGIT_8: val = 8; render(); return true;
        case AC::DIGIT_9: val = 9; render(); return true;

        case AC::RETURN:
        case AC::EVAL:
            hide();
            if (m_on_confirm) {
                m_on_confirm(m_rows, m_show_cols ? m_cols : 1);
            }
            return true;

        case AC::ESCAPE:
            hide();
            return true;

        default:
            return false;
    }
}

} // namespace ovb::gui
