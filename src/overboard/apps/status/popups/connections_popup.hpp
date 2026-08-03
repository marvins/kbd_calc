/**
 * @file    connections_popup.hpp
 * @author  Marvin Smith
 * @date    2026-07-12
 *
 * @brief   Connections popup for the Status page
 *
 * Displays current WiFi status, a Turn On/Off toggle, and a
 * "Select WiFi" submenu that lists nearby networks.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/hal/i_system_info.hpp>

namespace ovb::gui {

/**
 * @brief Self-contained connections popup with embedded WiFi network menu
 *
 * Owns its LVGL objects.  Constructed once and kept alive for the lifetime
 * of the status page.  Call show() to make it visible and refresh its
 * content; hide() conceals it without destroying the LVGL tree.
 */
class Connections_Popup {
    public:
        /**
         * @brief Callback fired when the user selects a network from the scan list
         * @param ssid  The chosen SSID string
         */
        using On_Select_Cb = std::function<void(const std::string& ssid)>;

        /**
         * @brief Construct the popup (initially hidden)
         * @param system_info  Platform system info used to query / control WiFi
         * @param on_select    Called when a network row is tapped (may be null)
         */
        Connections_Popup(hal::I_System_Info& system_info, On_Select_Cb on_select);

        /**
         * @brief Destructor — deletes all owned LVGL objects
         */
        ~Connections_Popup();

        /**
         * @brief Show the popup, refreshing WiFi status from the system
         */
        void show();

        /**
         * @brief Hide the popup without destroying it
         */
        void hide();

        /**
         * @brief Returns true when the popup is currently visible
         */
        bool is_visible() const;

        // Non-copyable, non-movable (LVGL objects hold raw `this` pointers)
        Connections_Popup(const Connections_Popup&)            = delete;
        Connections_Popup& operator=(const Connections_Popup&) = delete;
        Connections_Popup(Connections_Popup&&)                 = delete;
        Connections_Popup& operator=(Connections_Popup&&)      = delete;

        /**
         * @brief Returns true while the WiFi network submenu is open
         */
        bool wifi_menu_visible() const;


        /**
         * @brief Show the WiFi network scan submenu
         */
        void show_wifi_menu();

        /**
         * @brief Close the WiFi network scan submenu (popup stays open)
         */
        void hide_wifi_menu();

    private:
        void build_popup();
        void refresh();
        void build_wifi_menu();
        void cleanup_wifi_menu_children();

        hal::I_System_Info& m_system_info;
        On_Select_Cb        m_on_select;

        lv_obj_t* m_popup      = nullptr;
        lv_obj_t* m_status_lbl = nullptr;
        lv_obj_t* m_toggle_btn = nullptr;
        lv_obj_t* m_toggle_lbl = nullptr;
        lv_obj_t* m_wifi_menu  = nullptr;
};

} // namespace ovb::gui
