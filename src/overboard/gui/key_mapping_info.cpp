/**
 * @file    key_mapping_info.cpp
 * @author  Marvin Smith
 * @date    2026-06-03
 *
 * @brief   Key Mapping Info Panel implementation
 */
#include <overboard/gui/key_mapping_info.hpp>

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/font/font_selector.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/***************************/
/*        Constructor       */
/***************************/
Key_Mapping_Info::Key_Mapping_Info( lv_obj_t*                      parent,
                                   const ovb::core::Grid_Layout& layout,
                                   const core::Layer_Manager&    layers,
                                   int                           width,
                                   int                           height,
                                   std::function<std::string(int)> get_label_cb )
    : m_layout(layout), m_layers(layers), m_width(width), m_height(height), m_get_label_cb(std::move(get_label_cb))
{
    LOG_TRACE("Key_Mapping_Info: Creating root container");
    // Root container: full display, light background, no padding
    m_container = lv_obj_create(parent);
    lv_obj_set_size(m_container, width, height);
    lv_obj_align(m_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_container, lvgl_color(LVGL_COLOR_KBD_BG), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_container, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("Key_Mapping_Info: Creating header bar");
    // Header bar: layer name
    lv_obj_t* header = lv_obj_create(m_container);
    lv_obj_set_size(header, width, HEADER_H);
    lv_obj_align(header, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(header, lvgl_color(LVGL_COLOR_KBD_HEADER), LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(header, 0, LV_PART_MAIN);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("Key_Mapping_Info: Creating header label");
    m_header_label = lv_label_create(header);
    const std::string layer_text = "Layer: " + std::string(m_layers.current_layer().name);
    lv_label_set_text(m_header_label, layer_text.c_str());
    lv_obj_set_style_text_color(m_header_label, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(m_header_label, LV_ALIGN_LEFT_MID, MARGIN_LEFT, 0);

    LOG_TRACE("Key_Mapping_Info: Building key display elements");
    build_keys(m_container);
    LOG_TRACE("Key_Mapping_Info: Constructor complete");
}

/***************************/
/*        Build Keys        */
/***************************/
void Key_Mapping_Info::build_keys(lv_obj_t* parent) {
    LOG_TRACE("Key_Mapping_Info::build_keys: Getting key count from layout");
    const int key_count = m_layout.key_count();
    m_key_labels.assign(static_cast<std::size_t>(key_count), nullptr);
    m_key_rects.assign(static_cast<std::size_t>(key_count), nullptr);

    const int grid_x  = MARGIN_LEFT;
    const int grid_y  = HEADER_H + MARGIN_TOP;
    const int grid_w  = m_width  - MARGIN_LEFT - KEY_PAD;
    const int grid_h  = m_height - grid_y       - KEY_PAD;
    const int cell_w  = grid_w / m_layout.cols();
    const int cell_h  = grid_h / m_layout.rows();

    const auto& layer = m_layers.current_layer();
    LOG_TRACE("Key_Mapping_Info::build_keys: layer.keys.size=" + std::to_string(layer.keys.size()));

    int keys_created = 0;

    for (int i = 0; i < key_count; ++i) {
        LOG_TRACE("Key_Mapping_Info::build_keys: Processing key " + std::to_string(i));
        auto pos_opt = m_layout.get_key_position(i);
        if (!pos_opt) {
            LOG_TRACE("Key_Mapping_Info::build_keys: Key " + std::to_string(i) + " has no position, skipping");
            continue;
        }

        if (m_layout.get_cell_type(pos_opt->col, pos_opt->row) != ovb::core::Cell_Type::KEY_START) {
            LOG_TRACE("Key_Mapping_Info::build_keys: Key " + std::to_string(i) + " is not KEY_START, skipping");
            continue;
        }

        auto rect_opt = m_layout.get_key_rect(
            i,
            ovb::core::Point<int>{grid_x, grid_y},
            ovb::core::Point<int>{cell_w,  cell_h},
            KEY_PAD);
        if (!rect_opt) {
            LOG_TRACE("Key_Mapping_Info::build_keys: Key " + std::to_string(i) + " has no rect, skipping");
            continue;
        }

        const auto& r = *rect_opt;

        // Rectangle widget (non-interactive, no click flag)
        lv_obj_t* rect = lv_obj_create(parent);
        if (!rect) {
            LOG_TRACE("Key_Mapping_Info::build_keys: Failed to create rect for key " + std::to_string(i) + ", skipping");
            continue;
        }
        keys_created++;

        lv_obj_set_pos(rect, r.x, r.y);
        lv_obj_set_size(rect, r.w, r.h);
        lv_obj_set_style_bg_color(rect, lvgl_color(LVGL_COLOR_KBD_BUTTON), LV_PART_MAIN);
        lv_obj_set_style_border_color(rect, lvgl_color(LVGL_COLOR_BORDER_DARK), LV_PART_MAIN);
        lv_obj_set_style_border_width(rect, 1, LV_PART_MAIN);
        lv_obj_set_style_radius(rect, 4, LV_PART_MAIN);
        lv_obj_set_style_pad_all(rect, 2, LV_PART_MAIN);
        // Note: NOT clickable — purely informational

        LOG_TRACE("Key_Mapping_Info::build_keys: Creating label for key " + std::to_string(i));
        // Label
        lv_obj_t* lbl = lv_label_create(rect);
        if (!lbl) {
            LOG_TRACE("Key_Mapping_Info::build_keys: Failed to create label for key " + std::to_string(i) + ", skipping");
            continue;
        }

        LOG_TRACE("Key_Mapping_Info::build_keys: Getting display text for key " + std::to_string(i));
        auto action_code = layer.keys[static_cast<std::size_t>(i)];
        LOG_TRACE("Key_Mapping_Info::build_keys: Key " + std::to_string(i) + " has action_code=" + std::to_string(static_cast<int>(action_code)));
        // Get display text: custom panel label > JSON label > action code display
        std::string text = get_key_label(i, action_code);
        LOG_TRACE("Key_Mapping_Info::build_keys: Display text for key " + std::to_string(i) + " is: '" + text + "' (length=" + std::to_string(text.length()) + ")");

        // Set label text - empty string for unassigned keys
        lv_label_set_text(lbl, text.c_str());

        // Hide label if no text to display
        if (text.empty()) {
            lv_obj_add_flag(lbl, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(lbl, LV_OBJ_FLAG_HIDDEN);
        }

        LOG_TRACE("Key_Mapping_Info::build_keys: Label text set for key " + std::to_string(i));
        lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);

        // Use custom font for math symbols
        if (font::requires_custom_font(action_code)) {
            lv_obj_set_style_text_font(lbl, &lv_font_superscript, LV_PART_MAIN);
        }

        lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);

        m_key_rects[static_cast<std::size_t>(i)]  = rect;
        m_key_labels[static_cast<std::size_t>(i)] = lbl;
        LOG_TRACE("Key_Mapping_Info::build_keys: Key " + std::to_string(i) + " complete");
    }
    LOG_TRACE("Key_Mapping_Info::build_keys: Complete - created=" + std::to_string(keys_created));
}

/***************************/
/*       Layer Update      */
/***************************/
void Key_Mapping_Info::update_layer() {
    LOG_DEBUG("Key_Mapping_Info::update_layer: starting");
    const auto& layer = m_layers.current_layer();
    const std::size_t key_count = m_key_labels.size();
    LOG_DEBUG("Key_Mapping_Info::update_layer: key_count=", std::to_string(key_count));

    // Update header label
    LOG_DEBUG("Key_Mapping_Info::update_layer: updating header");
    const std::string layer_text = "Layer: " + std::string(layer.name);
    lv_label_set_text(m_header_label, layer_text.c_str());
    LOG_DEBUG("Key_Mapping_Info::update_layer: header updated");

    LOG_DEBUG("Key_Mapping_Info::update_layer: entering loop");
    for (std::size_t i = 0; i < key_count; ++i) {
        if (!m_key_labels[i]) continue;
        // Get display text: custom panel label > JSON label > action code display
        std::string text = get_key_label(static_cast<int>(i), layer.keys[i]);
        lv_label_set_text(m_key_labels[i], text.c_str());

        // Use custom font for math symbols
        if (font::requires_custom_font(layer.keys[i])) {
            lv_obj_set_style_text_font(m_key_labels[i], &lv_font_superscript, LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_font(m_key_labels[i], LVGL_FONT_DEFAULT, LV_PART_MAIN);
        }

        // Show/hide label based on whether there's text
        if (text.empty()) {
            lv_obj_add_flag(m_key_labels[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(m_key_labels[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    LOG_DEBUG("Key_Mapping_Info::update_layer: complete");
}

/***************************/
/*      Get Key Label      */
/***************************/
std::string Key_Mapping_Info::get_key_label(int key_index, core::Action_Code action_code) const {
    // First: check if panel provides a custom label
    if (m_get_label_cb) {
        std::string custom = m_get_label_cb(key_index);
        if (!custom.empty()) {
            return custom;
        }
    }
    // Second: use JSON label from keyboard.json
    const auto& layer = m_layers.current_layer();
    std::string text = layer.labels[static_cast<std::size_t>(key_index)];
    if (!text.empty()) {
        return text;
    }
    // Third: fall back to action code display string
    return core::action_code_to_display(action_code);
}

} // namespace ovb::gui
