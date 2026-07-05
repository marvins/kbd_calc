/**
 * @file    settings_page.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Settings page panel implementation with editable key-value pairs
 */
#include <overboard/apps/settings/settings_page.hpp>

// C++ Standard Libraries
#include <sstream>
#include <vector>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {


/*******************************/
/*       Load Settings         */
/*******************************/
void Settings_Page::load_settings() {
    m_entries.clear();

    // Flatten the TOML tree into key-value pairs
    const auto& tree = m_settings->tree();
    const auto& data = tree.data();

    std::function<void(const toml::table&, const std::string&)> visit_table;
    visit_table = [&](const toml::table& tbl, const std::string& prefix) {
        for (const auto& [key, value] : tbl) {
            std::string full_key = prefix.empty() ? std::string(key) : prefix + "." + std::string(key);

            if (value.is_table()) {
                visit_table(*value.as_table(), full_key);
            } else {
                Setting_Entry entry;
                entry.key = full_key;

                // Convert value to string
                std::ostringstream oss;
                if (value.is_string()) {
                    oss << value.as_string()->get();
                } else if (value.is_integer()) {
                    oss << *value.as_integer();
                } else if (value.is_floating_point()) {
                    oss << *value.as_floating_point();
                } else if (value.is_boolean()) {
                    oss << (*value.as_boolean() ? "true" : "false");
                } else {
                    oss << "<unsupported>";
                }
                entry.value = oss.str();
                entry.modified = false;

                m_entries.push_back(entry);
            }
        }
    };

    visit_table(data, "");
    LOG_DEBUG("Settings_Page: loaded ", m_entries.size(), " settings");
}

/*******************************/
/*     Create Setting Row      */
/*******************************/
void Settings_Page::create_setting_row(Setting_Entry& entry, lv_obj_t* parent) {
    // Row container
    entry.row = lv_obj_create(parent);
    lv_obj_set_size(entry.row, lv_pct(100), 40);
    lv_obj_set_style_bg_color(entry.row, lvgl_color(LVGL_COLOR_BG_TABLE), LV_PART_MAIN);
    lv_obj_set_style_border_width(entry.row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(entry.row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_row(entry.row, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(entry.row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(entry.row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Key label (left side, 40% width)
    entry.key_label = lv_label_create(entry.row);
    lv_label_set_text(entry.key_label, entry.key.c_str());
    lv_obj_set_style_text_color(entry.key_label, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_set_width(entry.key_label, lv_pct(40));

    // Value input (right side, 55% width)
    entry.val_input = lv_textarea_create(entry.row);
    lv_textarea_set_one_line(entry.val_input, true);
    lv_textarea_set_text(entry.val_input, entry.value.c_str());
    lv_obj_set_width(entry.val_input, lv_pct(55));
    lv_obj_set_style_bg_color(entry.val_input, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_text_color(entry.val_input, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);

    // Track modifications
    lv_obj_set_user_data(entry.val_input, &entry);
    lv_obj_add_event_cb(entry.val_input, [](lv_event_t* e) {
        auto* entry_ptr = static_cast<Setting_Entry*>(lv_event_get_user_data(e));
        if (!entry_ptr) return;

        const char* new_val = lv_textarea_get_text(entry_ptr->val_input);
        if (entry_ptr->value != new_val) {
            entry_ptr->modified = true;
            entry_ptr->value = new_val;

            // Update save button state
            auto* page = static_cast<Settings_Page*>(lv_obj_get_user_data(lv_obj_get_parent(entry_ptr->row)));
            if (page) {
                page->update_save_button_state();
            }
        }
    }, LV_EVENT_VALUE_CHANGED, &entry);
}

/*******************************/
/*          Save All           */
/*******************************/
void Settings_Page::save_all() {
    LOG_DEBUG("Settings_Page: saving settings");

    for (auto& entry : m_entries) {
        if (entry.modified) {
            // Try to parse as number first, then bool, then string
            try {
                if (entry.value == "true" || entry.value == "false") {
                    m_settings->set(entry.key, entry.value == "true");
                } else if (entry.value.find('.') != std::string::npos) {
                    m_settings->set(entry.key, std::stod(entry.value));
                } else {
                    try {
                        m_settings->set(entry.key, std::stoi(entry.value));
                    } catch (...) {
                        m_settings->set(entry.key, entry.value);
                    }
                }
                entry.modified = false;
            } catch (const std::exception& e) {
                LOG_WARN("Settings_Page: failed to parse value for ", entry.key, ": ", e.what());
                m_settings->set(entry.key, entry.value);
                entry.modified = false;
            }
        }
    }

    if (m_settings->save()) {
        LOG_INFO("Settings_Page: settings saved successfully");
    } else {
        LOG_ERROR("Settings_Page: failed to save settings");
    }

    update_save_button_state();
}

/*******************************/
/*         Reload All          */
/*******************************/
void Settings_Page::reload_all() {
    LOG_DEBUG("Settings_Page: reloading settings");

    if (m_settings->reload()) {
        // Reload entries and update UI
        load_settings();

        // Recreate all rows
        if (m_scroll_area) {
            lv_obj_clean(m_scroll_area);
            for (auto& entry : m_entries) {
                create_setting_row(entry, m_scroll_area);
            }
        }

        LOG_INFO("Settings_Page: settings reloaded successfully");
    } else {
        LOG_ERROR("Settings_Page: failed to reload settings");
    }

    update_save_button_state();
}

/*******************************/
/*   Update Save Button State  */
/*******************************/
void Settings_Page::update_save_button_state() {
    bool has_changes = false;
    for (const auto& entry : m_entries) {
        if (entry.modified) {
            has_changes = true;
            break;
        }
    }

    if (m_save_btn) {
        if (has_changes || m_settings->is_dirty()) {
            lv_obj_clear_state(m_save_btn, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_save_btn, LV_STATE_DISABLED);
        }
    }
}


/*******************************/
/*          Constructor        */
/*******************************/
Settings_Page::Settings_Page(hal::I_System_Info& system_info,
                     std::shared_ptr<core::Settings_Manager> settings,
                     Back_Cb on_back)
    : m_settings(settings), m_system_info(system_info), m_on_back(std::move(on_back)) {}

/*******************************/
/*          Destructor         */
/*******************************/
Settings_Page::~Settings_Page() = default;

/*******************************/
/*           Activate          */
/*******************************/
void Settings_Page::activate(lv_obj_t* parent) {
    LOG_DEBUG("Settings_Page: activating");

    m_container = lv_obj_create(parent);
    lv_obj_set_size(m_container, lv_pct(100), lv_pct(100));
    lv_obj_align(m_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_container, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(m_container, false);

    // Use explicit dimensions - parent may not be laid out yet
    const int width = hal::LCD_WIDTH;

    // Header bar
    m_header = std::make_unique<Header_Bar>( m_container,
                                             width,
                                             m_system_info );
    m_header->set_app_name("Settings");

    // Button bar (Save / Reload)
    const int button_bar_y = Header_Bar::HEIGHT + 4;
    const int button_bar_height = 36;

    lv_obj_t* button_bar = lv_obj_create(m_container);
    lv_obj_set_size(button_bar, width - 16, button_bar_height);
    lv_obj_align(button_bar, LV_ALIGN_TOP_LEFT, 8, button_bar_y);
    lv_obj_set_style_bg_color(button_bar, lvgl_color(LVGL_COLOR_BG_TABLE), LV_PART_MAIN);
    lv_obj_set_style_border_width(button_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button_bar, 4, LV_PART_MAIN);
    lv_obj_set_flex_flow(button_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(button_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Save button
    m_save_btn = lv_btn_create(button_bar);
    lv_obj_set_size(m_save_btn, 80, 28);
    lv_obj_t* save_lbl = lv_label_create(m_save_btn);
    lv_label_set_text(save_lbl, LV_SYMBOL_SAVE " Save");
    lv_obj_center(save_lbl);
    lv_obj_set_user_data(m_save_btn, this);
    lv_obj_add_event_cb(m_save_btn, [](lv_event_t* e) {
        auto* page = static_cast<Settings_Page*>(lv_event_get_user_data(e));
        if (page) page->save_all();
    }, LV_EVENT_CLICKED, nullptr);

    // Reload button
    m_reload_btn = lv_btn_create(button_bar);
    lv_obj_set_size(m_reload_btn, 100, 28);
    lv_obj_t* reload_lbl = lv_label_create(m_reload_btn);
    lv_label_set_text(reload_lbl, LV_SYMBOL_REFRESH " Reload");
    lv_obj_center(reload_lbl);
    lv_obj_set_user_data(m_reload_btn, this);
    lv_obj_add_event_cb(m_reload_btn, [](lv_event_t* e) {
        auto* page = static_cast<Settings_Page*>(lv_event_get_user_data(e));
        if (page) page->reload_all();
    }, LV_EVENT_CLICKED, nullptr);

    // Scrollable content area
    const int content_y = button_bar_y + button_bar_height + 8;
    const int content_height = hal::LCD_HEIGHT - content_y - Footer_Bar::HEIGHT - 8;

    m_scroll_area = lv_obj_create(m_container);
    lv_obj_set_size(m_scroll_area, width - 16, content_height);
    lv_obj_align(m_scroll_area, LV_ALIGN_TOP_LEFT, 8, content_y);
    lv_obj_set_style_bg_color(m_scroll_area, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_scroll_area, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_scroll_area, lvgl_color(LVGL_COLOR_BORDER_LIGHT), LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_scroll_area, 4, LV_PART_MAIN);
    lv_obj_set_flex_flow(m_scroll_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(m_scroll_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_user_data(m_scroll_area, this);

    // Load and display settings
    load_settings();
    for (auto& entry : m_entries) {
        create_setting_row(entry, m_scroll_area);
    }

    update_save_button_state();

    // Footer bar
    m_footer = std::make_unique<Footer_Bar>(m_container, width);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Settings_Page::deactivate() {
    LOG_DEBUG("Settings_Page: deactivating");
    m_entries.clear();
    m_footer.reset();
    m_header.reset();
    if (m_container) {
        lv_obj_del(m_container);
        m_container = nullptr;
    }
}

/*******************************/
/*        Handle Input         */
/*******************************/
bool Settings_Page::handle_input(core::Action_Code action) {
    if (action == core::Action_Code::ESCAPE && m_on_back) {
        m_on_back();
    }
    return true;
}

/*******************************/
/*           Refresh           */
/*******************************/
void Settings_Page::refresh() {
    // Settings page doesn't need periodic refresh
}

} // namespace ovb::gui
