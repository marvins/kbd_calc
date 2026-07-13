/**
 * @file    connections_popup.cpp
 * @author  Marvin Smith
 * @date    2026-07-12
 *
 * @brief   Connections popup implementation
 */
#include <overboard/apps/status/popups/connections_popup.hpp>

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*         Constructor         */
/*******************************/
Connections_Popup::Connections_Popup(hal::I_System_Info& system_info, On_Select_Cb on_select)
    : m_system_info(system_info)
    , m_on_select(std::move(on_select))
{
    build_popup();
    lv_obj_set_hidden(m_popup, true);
}

/*******************************/
/*          Destructor         */
/*******************************/
Connections_Popup::~Connections_Popup() {
    cleanup_wifi_menu_children();
    if (m_wifi_menu) {
        lv_obj_del(m_wifi_menu);
        m_wifi_menu = nullptr;
    }
    if (m_popup) {
        lv_obj_del(m_popup);
        m_popup = nullptr;
    }
}

/*******************************/
/*            show             */
/*******************************/
void Connections_Popup::show() {
    refresh();
    lv_obj_set_hidden(m_popup, false);
}

/*******************************/
/*            hide             */
/*******************************/
void Connections_Popup::hide() {
    hide_wifi_menu();
    lv_obj_set_hidden(m_popup, true);
}

/*******************************/
/*          is_visible         */
/*******************************/
bool Connections_Popup::is_visible() const {
    return m_popup && !lv_obj_is_hidden(m_popup);
}

/*******************************/
/*      wifi_menu_visible      */
/*******************************/
bool Connections_Popup::wifi_menu_visible() const {
    return m_wifi_menu != nullptr;
}

/*******************************/
/*       show_wifi_menu        */
/*******************************/
void Connections_Popup::show_wifi_menu() {
    hide_wifi_menu();
    build_wifi_menu();
}

/*******************************/
/*       hide_wifi_menu        */
/*******************************/
void Connections_Popup::hide_wifi_menu() {
    if (m_wifi_menu) {
        cleanup_wifi_menu_children();
        lv_obj_del(m_wifi_menu);
        m_wifi_menu = nullptr;
    }
}

/*******************************/
/*        build_popup          */
/*******************************/
void Connections_Popup::build_popup() {
    const bool has_wifi = m_system_info.has_wifi();

    constexpr int POPUP_W { 300 };
    constexpr int POPUP_H { 200 };

    m_popup = lv_obj_create(lv_screen_active());
    lv_obj_set_size(m_popup, POPUP_W, POPUP_H);
    lv_obj_center(m_popup);
    lv_obj_set_style_bg_color(m_popup, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_popup, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_popup, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_radius(m_popup, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_popup, 12, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(m_popup);
    lv_label_set_text(title, LV_SYMBOL_WIFI "  Connections");
    lv_obj_set_style_text_font(title, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    // Status label — content set by refresh()
    m_status_lbl = lv_label_create(m_popup);
    lv_label_set_text(m_status_lbl, "");
    lv_obj_set_style_text_font(m_status_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_status_lbl, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    lv_obj_set_width(m_status_lbl, POPUP_W - 28);
    lv_label_set_long_mode(m_status_lbl, LV_LABEL_LONG_WRAP);
    lv_obj_align(m_status_lbl, LV_ALIGN_TOP_LEFT, 0, 28);

    if (has_wifi) {
        // Turn On / Turn Off toggle
        m_toggle_btn = lv_btn_create(m_popup);
        lv_obj_set_size(m_toggle_btn, 120, 32);
        lv_obj_align(m_toggle_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_obj_set_style_radius(m_toggle_btn, 4, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(m_toggle_btn, 0, LV_PART_MAIN);
        lv_obj_add_event_cb(m_toggle_btn, [](lv_event_t* e) {
            auto* self = static_cast<Connections_Popup*>(lv_event_get_user_data(e));
            const bool currently = self->m_system_info.wifi_enabled();
            self->m_system_info.set_wifi_enabled(!currently);
            self->refresh();
        }, LV_EVENT_CLICKED, this);

        m_toggle_lbl = lv_label_create(m_toggle_btn);
        lv_obj_set_style_text_font(m_toggle_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(m_toggle_lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        lv_obj_center(m_toggle_lbl);

        // Select WiFi button — visibility toggled by refresh()
        lv_obj_t* select_btn = lv_btn_create(m_popup);
        lv_obj_set_size(select_btn, 140, 32);
        lv_obj_align(select_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        lv_obj_set_style_bg_color(select_btn, lvgl_color(LVGL_COLOR_KBD_BUTTON), LV_PART_MAIN);
        lv_obj_set_style_radius(select_btn, 4, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(select_btn, 0, LV_PART_MAIN);
        lv_obj_add_event_cb(select_btn, [](lv_event_t* e) {
            auto* self = static_cast<Connections_Popup*>(lv_event_get_user_data(e));
            self->show_wifi_menu();
        }, LV_EVENT_CLICKED, this);
        lv_obj_set_user_data(select_btn, this);

        lv_obj_t* select_lbl = lv_label_create(select_btn);
        lv_label_set_text(select_lbl, "Select WiFi...");
        lv_obj_set_style_text_font(select_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(select_lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        lv_obj_center(select_lbl);
    }

    // Close hint
    lv_obj_t* close_lbl = lv_label_create(m_popup);
    lv_label_set_text(close_lbl, "ESC to close");
    lv_obj_set_style_text_font(close_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(close_lbl, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
    lv_obj_align(close_lbl, LV_ALIGN_TOP_RIGHT, 0, 0);
}

/*******************************/
/*           refresh           */
/*******************************/
void Connections_Popup::refresh() {
    const hal::System_Info info     = m_system_info.get_info();
    const bool             has_wifi = m_system_info.has_wifi();
    const bool             enabled  = has_wifi && m_system_info.wifi_enabled();

    // Update status label
    std::string status_str;
    if (!has_wifi) {
        status_str = "WiFi: Not available";
    } else if (!enabled) {
        status_str = "WiFi: Off";
    } else if (info.wifi.has_value() && info.wifi->connected) {
        status_str = "Connected: " + info.wifi->ssid;
        const int sig = info.wifi->signal_dbm;
        if (sig != 0) {
            status_str += "  (" + std::to_string(sig) + "%)";
        }
    } else {
        status_str = "WiFi: On - not connected";
    }
    lv_label_set_text(m_status_lbl, status_str.c_str());

    if (m_toggle_btn) {
        lv_obj_set_style_bg_color(m_toggle_btn,
            enabled ? lvgl_color(LVGL_COLOR_KBD_BUTTON_PRESSED) : lvgl_color(LVGL_COLOR_KBD_BUTTON),
            LV_PART_MAIN);
        lv_label_set_text(m_toggle_lbl, enabled ? "Turn Off" : "Turn On");

        // Show/hide the Select WiFi button (last child before close hint = index child_count-2)
        const uint32_t count = lv_obj_get_child_count(m_popup);
        if (count >= 2) {
            lv_obj_t* select_btn = lv_obj_get_child(m_popup, static_cast<int32_t>(count) - 2);
            lv_obj_set_hidden(select_btn, !enabled);
        }
    }
}

/*******************************/
/*       build_wifi_menu       */
/*******************************/
void Connections_Popup::build_wifi_menu() {
    const std::vector<std::string> networks = m_system_info.scan_wifi_networks();

    constexpr int MENU_W   { 260 };
    constexpr int ROW_H    { 28 };
    constexpr int HEADER_H { 30 };
    constexpr int FOOTER_H { 24 };
    constexpr int PAD      { 8 };
    const int menu_h = HEADER_H + static_cast<int>(networks.size()) * ROW_H + FOOTER_H + PAD * 2;

    m_wifi_menu = lv_obj_create(lv_screen_active());
    lv_obj_set_size(m_wifi_menu, MENU_W, menu_h);
    lv_obj_center(m_wifi_menu);
    lv_obj_set_style_bg_color(m_wifi_menu, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_wifi_menu, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_wifi_menu, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_radius(m_wifi_menu, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_wifi_menu, PAD, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(m_wifi_menu);
    lv_label_set_text(title, "Available Networks");
    lv_obj_set_style_text_font(title, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    if (networks.empty()) {
        lv_obj_t* none_lbl = lv_label_create(m_wifi_menu);
        lv_label_set_text(none_lbl, "No networks found");
        lv_obj_set_style_text_font(none_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(none_lbl, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
        lv_obj_align(none_lbl, LV_ALIGN_TOP_LEFT, 0, HEADER_H);
    } else {
        int y = HEADER_H;
        for (const auto& ssid : networks) {
            lv_obj_t* row_btn = lv_btn_create(m_wifi_menu);
            lv_obj_set_size(row_btn, MENU_W - PAD * 2, ROW_H - 2);
            lv_obj_set_pos(row_btn, 0, y);
            lv_obj_set_style_bg_color(row_btn, lvgl_color(LVGL_COLOR_KBD_BUTTON), LV_PART_MAIN);
            lv_obj_set_style_bg_color(row_btn, lvgl_color(LVGL_COLOR_KBD_BUTTON_PRESSED),
                static_cast<lv_style_selector_t>(
                    static_cast<uint32_t>(LV_PART_MAIN) | static_cast<uint32_t>(LV_STATE_PRESSED)));
            lv_obj_set_style_radius(row_btn, 3, LV_PART_MAIN);
            lv_obj_set_style_shadow_width(row_btn, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(row_btn, 0, LV_PART_MAIN);
            lv_obj_set_style_pad_all(row_btn, 2, LV_PART_MAIN);

            auto* ssid_copy = new std::string(ssid);
            lv_obj_set_user_data(row_btn, ssid_copy);
            lv_obj_add_event_cb(row_btn, [](lv_event_t* e) {
                auto* btn      = static_cast<lv_obj_t*>(lv_event_get_target(e));
                auto* ssid_str = static_cast<std::string*>(lv_obj_get_user_data(btn));
                auto* self     = static_cast<Connections_Popup*>(lv_event_get_user_data(e));
                if (ssid_str) {
                    LOG_INFO("WiFi: user selected SSID: ", *ssid_str);
                    if (self && self->m_on_select) {
                        self->m_on_select(*ssid_str);
                    }
                    delete ssid_str;
                    lv_obj_set_user_data(btn, nullptr);
                }
            }, LV_EVENT_CLICKED, this);

            lv_obj_t* row_lbl = lv_label_create(row_btn);
            lv_label_set_text(row_lbl, ssid.c_str());
            lv_obj_set_style_text_font(row_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
            lv_obj_set_style_text_color(row_lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
            lv_obj_align(row_lbl, LV_ALIGN_LEFT_MID, 4, 0);

            y += ROW_H;
        }
    }

    // Close hint
    lv_obj_t* hint = lv_label_create(m_wifi_menu);
    lv_label_set_text(hint, "ESC to cancel");
    lv_obj_set_style_text_font(hint, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(hint, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, 0);
}

/************************************/
/*   cleanup_wifi_menu_children     */
/************************************/
void Connections_Popup::cleanup_wifi_menu_children() {
    if (!m_wifi_menu) {
        return;
    }
    const uint32_t count = lv_obj_get_child_count(m_wifi_menu);
    for (uint32_t i = 0; i < count; ++i) {
        lv_obj_t* child = lv_obj_get_child(m_wifi_menu, static_cast<int32_t>(i));
        if (child) {
            auto* ssid_str = static_cast<std::string*>(lv_obj_get_user_data(child));
            if (ssid_str) {
                delete ssid_str;
                lv_obj_set_user_data(child, nullptr);
            }
        }
    }
}

} // namespace ovb::gui
