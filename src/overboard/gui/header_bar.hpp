/**
 * @file    header_bar.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   LVGL header bar — shows active panel name
 */
#pragma once

// C++ Standard Libraries
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/i_header_bar.hpp>
#include <overboard/hal/i_system_info.hpp>

namespace ovb::gui {

/**
 * @brief LVGL-backed header bar
 *
 * Renders a thin bar at the top of the panel container displaying
 * the active panel name left-aligned.
 */
class Header_Bar : public I_Header_Bar {

    public:

        static constexpr int HEIGHT { 20 };

        /**
         * @brief Construct and build the header bar
         * @param parent LVGL parent object
         * @param width  Width in pixels
         * @param system_info Reference to platform system info provider
         */
        Header_Bar( lv_obj_t* parent,
                    int width,
                    hal::I_System_Info& system_info);

        /**
         * @brief Destructor
         */
        ~Header_Bar() override;

        /**
         * @brief Set the application name to display
         * @param name Application name
         */
        void set_app_name(const std::string& name) override;

        /**
         * @brief Refresh the header bar
         */
        void refresh() override;

        /**
         * @brief Get the underlying LVGL object
         * @return LVGL object pointer
         */
        lv_obj_t* get_obj() const override;

    private:

        /// @brief Header bar object
        lv_obj_t*           m_bar         = nullptr;

        /// @brief Application name label
        lv_obj_t*           m_label       = nullptr;

        /// @brief Icon tray container (right-aligned)
        lv_obj_t*           m_icon_tray   = nullptr;  // flex-row container (right-aligned)

        /// @brief WiFi icon
        lv_obj_t*           m_icon_wifi   = nullptr;

        /// @brief Bluetooth icon
        lv_obj_t*           m_icon_bt     = nullptr;

        /// @brief Battery icon
        lv_obj_t*           m_icon_batt   = nullptr;

        /// @brief Application name
        std::string         m_app_name;

        /// @brief System info provider
        hal::I_System_Info* m_system_info = nullptr;

        /// @brief Auto-refresh timer for system info
        lv_timer_t*          m_refresh_timer = nullptr;

        /// @brief Flag to prevent refresh during destruction
        bool                 m_destroying = false;
};

} // namespace ovb::gui
