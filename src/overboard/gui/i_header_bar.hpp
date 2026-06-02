/**
 * @file    i_header_bar.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Abstract header bar interface
 *
 * Displayed at the top of the LCD panel. Shows the active panel name
 * and optional system info (time, battery) as stubs for now.
 */
#pragma once

// C++ Standard Libraries
#include <string>

namespace ovb::gui {

/**
 * @brief Header bar interface
 *
 * Implementations render a bar at the top of the panel area showing
 * the current application/panel name and system status.
 */
class I_Header_Bar {

    public:

        /**
         * @brief Virtual destructor
         */
        virtual ~I_Header_Bar() = default;

        /**
         * @brief Set the displayed application/panel name
         */
        virtual void set_app_name(const std::string& name) = 0;

        /**
         * @brief Refresh the rendered bar (call after set_* changes)
         */
        virtual void refresh() = 0;
};

} // namespace ovb::gui
