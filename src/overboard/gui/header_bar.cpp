/**
 * @file    header_bar.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   LVGL header bar implementation
 */
#include <overboard/gui/header_bar.hpp>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>

namespace ovb::gui {

/*******************************/
/*          Constructor        */
/*******************************/
Header_Bar::Header_Bar(lv_obj_t* parent, int width, hal::I_System_Info& system_info)
    : m_system_info(&system_info)
{
    m_bar = lv_obj_create(parent);
    lv_obj_set_size(m_bar, width, HEIGHT);
    lv_obj_align(m_bar, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_bar, lvgl_color(LVGL_COLOR_KBD_HEADER), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_bar, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(m_bar, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(m_bar, false);

    // Left: app name label
    m_label = lv_label_create(m_bar);
    lv_label_set_text(m_label, "");
    lv_obj_set_style_text_color(m_label, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    lv_obj_set_style_text_font(m_label, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_align(m_label, LV_ALIGN_LEFT_MID, 4, 0);

    // Right: icon tray — horizontal flex row pinned to the right edge
    m_icon_tray = lv_obj_create(m_bar);
    lv_obj_set_size(m_icon_tray, LV_SIZE_CONTENT, HEIGHT - 4);
    lv_obj_align(m_icon_tray, LV_ALIGN_RIGHT_MID, -2, 0);
    lv_obj_set_style_bg_opa(m_icon_tray, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(m_icon_tray, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_icon_tray, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(m_icon_tray, 3, LV_PART_MAIN);
    lv_obj_set_scrollable(m_icon_tray, false);
    lv_obj_set_layout(m_icon_tray, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_icon_tray, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(m_icon_tray,
                          LV_FLEX_ALIGN_END,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    // Auto-refresh timer for system info (WiFi/Bluetooth status)
    m_refresh_timer = lv_timer_create([](lv_timer_t* timer) {
        auto* header = static_cast<Header_Bar*>(timer->user_data);
        if (header && !header->m_destroying) {
            header->refresh();
        }
    }, 5000, this);  // Refresh every 5 seconds

    // WiFi icon
    m_icon_wifi = lv_label_create(m_icon_tray);
    lv_label_set_text(m_icon_wifi, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(m_icon_wifi, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_icon_wifi, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);

    // Bluetooth icon
    m_icon_bt = lv_label_create(m_icon_tray);
    lv_label_set_text(m_icon_bt, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_style_text_font(m_icon_bt, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_icon_bt, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);

    // Battery icon
    m_icon_batt = lv_label_create(m_icon_tray);
    lv_label_set_text(m_icon_batt, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_font(m_icon_batt, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_icon_batt, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
}

/*******************************/
/*          Destructor         */
/*******************************/
Header_Bar::~Header_Bar() {
    m_destroying = true;
    if (m_refresh_timer && lv_display_get_next(nullptr) != nullptr) {
        lv_timer_del(m_refresh_timer);
        m_refresh_timer = nullptr;
    }
}

/*******************************/
/*        Set App Name         */
/*******************************/
void Header_Bar::set_app_name(const std::string& name) {
    m_app_name = name;
    if (m_label) {
        lv_label_set_text(m_label, name.c_str());
    }
}

/*******************************/
/*           Refresh           */
/*******************************/
void Header_Bar::refresh() {
    if (!m_bar) {
        return;
    }

    const auto sys = m_system_info->get_info();

    // WiFi icon
    if (m_icon_wifi) {
        lv_obj_set_hidden(m_icon_wifi, false);
        if (m_system_info->has_wifi() && sys.wifi.has_value()) {
            if (sys.wifi->connected) {
                lv_label_set_text(m_icon_wifi, LV_SYMBOL_WIFI);
                lv_obj_set_style_text_color(m_icon_wifi,
                    lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
            } else {
                lv_label_set_text(m_icon_wifi, LV_SYMBOL_WARNING);
                lv_obj_set_style_text_color(m_icon_wifi,
                    lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
            }
        } else {
            // WiFi not available
            lv_label_set_text(m_icon_wifi, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(m_icon_wifi,
                lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
        }
    }

    // Bluetooth icon
    if (m_icon_bt) {
        lv_obj_set_hidden(m_icon_bt, false);
        if (m_system_info->has_bluetooth() && sys.bluetooth.has_value()) {
            lv_obj_set_style_text_color(m_icon_bt,
                sys.bluetooth->connected
                    ? lvgl_color(LVGL_COLOR_ACCENT_BLUE)
                    : lvgl_color(LVGL_COLOR_TEXT_MUTED),
                LV_PART_MAIN);
        } else {
            // Bluetooth not available
            lv_obj_set_style_text_color(m_icon_bt,
                lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
        }
    }

    // Battery icon
    if (m_icon_batt) {
        lv_obj_set_hidden(m_icon_batt, false);
        if (m_system_info->has_battery() && sys.battery_percent.has_value()) {
            const int pct = *sys.battery_percent;
            const char* sym = (pct > 75) ? LV_SYMBOL_BATTERY_FULL
                            : (pct > 50) ? LV_SYMBOL_BATTERY_3
                            : (pct > 25) ? LV_SYMBOL_BATTERY_2
                            : (pct > 10) ? LV_SYMBOL_BATTERY_1
                                         : LV_SYMBOL_BATTERY_EMPTY;
            // Show charging symbol if USB connected
            if (sys.usb.has_value() && sys.usb->connected) {
                sym = LV_SYMBOL_CHARGE;
            }
            lv_label_set_text(m_icon_batt, sym);
            lv_obj_set_style_text_color(m_icon_batt,
                (pct <= 10 && !(sys.usb.has_value() && sys.usb->connected))
                    ? lvgl_color(LVGL_COLOR_TEXT_MUTED)
                    : lvgl_color(LVGL_COLOR_TEXT_SECONDARY),
                LV_PART_MAIN);
        } else {
            // Battery not available
            lv_label_set_text(m_icon_batt, LV_SYMBOL_BATTERY_FULL);
            lv_obj_set_style_text_color(m_icon_batt,
                lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
        }
    }

    lv_obj_invalidate(m_bar);
}

/*******************************/
/*            Get Obj          */
/*******************************/
lv_obj_t* Header_Bar::get_obj() const {
    return m_bar;
}

} // namespace ovb::gui
