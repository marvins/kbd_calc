/**
 * @file    keyboard_view.cpp
 * @author  Marvin Smith
 * @date    2026-05-20
 *
 * @brief   LVGL widget-based keyboard view implementation
 */
#include <overboard/gui/keyboard_view.hpp>

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/****************************/
/*      Constructor         */
/****************************/
Keyboard_View::Keyboard_View( lv_obj_t*                      parent,
                              const ovb::core::Grid_Layout& layout,
                              const core::Layer_Manager&    layers,
                              int                           width,
                              int                           height )
    : m_layout(layout), m_layers(layers), m_width(width), m_height(height)
{
    LOG_TRACE("Keyboard_View: Creating root container");
    // Root container: full display, light background, no padding
    m_container = lv_obj_create(parent);
    lv_obj_set_size(m_container, width, height);
    lv_obj_align(m_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_container, lvgl_color(LVGL_COLOR_KBD_BG), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_container, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("Keyboard_View: Creating header bar");
    // Header bar: layer name
    lv_obj_t* header = lv_obj_create(m_container);
    lv_obj_set_size(header, width, HEADER_H);
    lv_obj_align(header, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(header, lvgl_color(LVGL_COLOR_KBD_HEADER), LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(header, 0, LV_PART_MAIN);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("Keyboard_View: Creating header label");
    lv_obj_t* header_label = lv_label_create(header);
    const std::string layer_text = "Layer: " + std::string(m_layers.current_layer().name);
    lv_label_set_text(header_label, layer_text.c_str());
    lv_obj_set_style_text_color(header_label, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(header_label, LV_ALIGN_LEFT_MID, MARGIN_LEFT, 0);

    LOG_TRACE("Keyboard_View: Building buttons");
    build_buttons(m_container);
    LOG_TRACE("Keyboard_View: Constructor complete");
}

/****************************/
/*     Button Building      */
/****************************/
void Keyboard_View::build_buttons(lv_obj_t* parent) {
    LOG_TRACE("build_buttons: Getting key count from layout");
    const int key_count = m_layout.key_count();
    m_buttons.assign(static_cast<std::size_t>(key_count), nullptr);
    m_labels.assign(static_cast<std::size_t>(key_count), nullptr);

    const int grid_x  = MARGIN_LEFT;
    const int grid_y  = HEADER_H + MARGIN_TOP;
    const int grid_w  = m_width  - MARGIN_LEFT - KEY_PAD;
    const int grid_h  = m_height - grid_y       - KEY_PAD;
    const int cell_w  = grid_w / m_layout.cols();
    const int cell_h  = grid_h / m_layout.rows();

    const auto& layer = m_layers.current_layer();
    LOG_TRACE("build_buttons: layer.keys.size=" + std::to_string(layer.keys.size()));

    int buttons_created = 0;
    int buttons_failed = 0;

    for (int i = 0; i < key_count; ++i) {
        LOG_TRACE("build_buttons: Processing key " + std::to_string(i));
        auto pos_opt = m_layout.get_key_position(i);
        if (!pos_opt) {
            LOG_TRACE("build_buttons: Key " + std::to_string(i) + " has no position, skipping");
            continue;
        }

        if (m_layout.get_cell_type(pos_opt->col, pos_opt->row) != ovb::core::Cell_Type::KEY_START) {
            LOG_TRACE("build_buttons: Key " + std::to_string(i) + " is not KEY_START, skipping");
            continue;
        }

        auto rect_opt = m_layout.get_key_rect(
            i,
            ovb::core::Point<int>{grid_x, grid_y},
            ovb::core::Point<int>{cell_w,  cell_h},
            KEY_PAD);
        if (!rect_opt) {
            LOG_TRACE("build_buttons: Key " + std::to_string(i) + " has no rect, skipping");
            continue;
        }

        const auto& r = *rect_opt;

        // Button widget
        lv_obj_t* btn = lv_button_create(parent);
        if (!btn) {
            LOG_TRACE("build_buttons: Failed to create button for key " + std::to_string(i) + ", skipping");
            buttons_failed++;
            continue;
        }
        buttons_created++;

        lv_obj_set_pos(btn, r.x, r.y);
        lv_obj_set_size(btn, r.w, r.h);
        lv_obj_set_style_bg_color(btn, lvgl_color(LVGL_COLOR_KBD_BUTTON),      static_cast<uint32_t>(LV_PART_MAIN) | static_cast<uint32_t>(LV_STATE_DEFAULT));
        lv_obj_set_style_bg_color(btn, lvgl_color(LVGL_COLOR_KBD_BUTTON_HOVER), static_cast<uint32_t>(LV_PART_MAIN) | static_cast<uint32_t>(LV_STATE_HOVERED));
        lv_obj_set_style_border_color(btn, lvgl_color(LVGL_COLOR_BORDER_DARK), LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
        lv_obj_set_style_radius(btn, 4, LV_PART_MAIN);
        lv_obj_set_style_pad_all(btn, 2, LV_PART_MAIN);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_ALL, this);

        LOG_TRACE("build_buttons: Creating label for key " + std::to_string(i));
        // Label
        lv_obj_t* lbl = lv_label_create(btn);
        if (!lbl) {
            LOG_TRACE("build_buttons: Failed to create label for key " + std::to_string(i) + ", skipping");
            continue;
        }

        LOG_TRACE("build_buttons: Getting display text for key " + std::to_string(i));
        const std::string text = core::key_code_to_display(layer.keys[static_cast<std::size_t>(i)]);
        LOG_TRACE("build_buttons: Display text for key " + std::to_string(i) + " is: '" + text + "' (length=" + std::to_string(text.length()) + ")");

        if (!text.empty()) {
            LOG_TRACE("build_buttons: Setting label text for key " + std::to_string(i));
            lv_label_set_text(lbl, text.c_str());
            LOG_TRACE("build_buttons: Label text set successfully for key " + std::to_string(i));
        } else {
            LOG_TRACE("build_buttons: Skipping empty label text for key " + std::to_string(i));
        }
        lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);

        m_key_indices.push_back(i);
        m_buttons[static_cast<std::size_t>(i)] = btn;
        m_labels[static_cast<std::size_t>(i)]  = lbl;
        LOG_TRACE("build_buttons: Key " + std::to_string(i) + " complete");
    }
    LOG_TRACE("build_buttons: Complete - created=" + std::to_string(buttons_created) + ", failed=" + std::to_string(buttons_failed));
}

/****************************/
/*     Layer Update         */
/****************************/
void Keyboard_View::update_layer() {
    const auto& layer = m_layers.current_layer();
    const std::size_t key_count = m_buttons.size();

    for (std::size_t i = 0; i < key_count; ++i) {
        if (!m_labels[i]) continue;
        const std::string text = core::key_code_to_display(layer.keys[i]);
        lv_label_set_text(m_labels[i], text.c_str());
    }
}

/******************************/
/*     Click Callback Setup   */
/*****************************/
void Keyboard_View::set_click_callback( void (*cb)(int key_index, void* user_data),
                                        void* user_data ) {
    m_click_cb        = cb;
    m_click_user_data = user_data;
}

/****************************/
/*      LVGL Event Callback  */
/****************************/
void Keyboard_View::button_event_cb(lv_event_t* e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_PRESSED &&
        code != LV_EVENT_RELEASED &&
        code != LV_EVENT_PRESS_LOST &&
        code != LV_EVENT_CLICKED) return;

    lv_obj_t* btn  = static_cast<lv_obj_t*>(lv_event_get_target(e));
    auto*     view = static_cast<Keyboard_View*>(lv_event_get_user_data(e));
    if (!view) return;

    int key_index = -1;
    for (const int idx : view->m_key_indices) {
        if (view->m_buttons[static_cast<std::size_t>(idx)] == btn) {
            key_index = idx;
            break;
        }
    }
    if (key_index < 0) return;

    lv_obj_t* lbl = view->m_labels[static_cast<std::size_t>(key_index)];
    const auto sel = static_cast<uint32_t>(LV_PART_MAIN) | static_cast<uint32_t>(LV_STATE_DEFAULT);

    if (code == LV_EVENT_PRESSED) {
        lv_obj_set_style_bg_color(btn, lvgl_color(LVGL_COLOR_KBD_BUTTON_PRESSED), sel);
        lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        lv_obj_set_style_bg_color(btn, lvgl_color(LVGL_COLOR_KBD_BUTTON), sel);
        lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    } else if (code == LV_EVENT_CLICKED) {
        if (view->m_click_cb) {
            view->m_click_cb(key_index, view->m_click_user_data);
        }
    }
}

} // namespace ovb::gui
