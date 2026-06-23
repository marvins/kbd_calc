/**
 * @file    solar_info.hpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   Solar information widget for the status page
 *
 * Displays sunrise, solar noon, sunset, current elevation, and a
 * visual day-arc progress indicator.
 */
#pragma once

// C++ Standard Libraries
#include <ctime>
#include <memory>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/solar_calc.hpp>

namespace ovb::gui::widgets {

/**
 * @brief Solar information display widget
 *
 * Shows a styled card containing:
 *  - A day-arc visualization showing sun position
 *  - Sunrise, solar noon, and sunset times
 *  - Current solar elevation angle
 *  - Day length
 */
class Solar_Info {
    public:

        /**
         * @brief Construct a new Solar_Info object
         */
        Solar_Info();

        /**
         * @brief Destroy the Solar_Info object
         */
        virtual ~Solar_Info();

        /**
         * @brief Create the widget UI under the given parent
         * @param parent LVGL parent object
         */
        void create(lv_obj_t* parent);

        /**
         * @brief Update displayed values for the given time
         * @param tm Current local time
         */
        void update(const std::tm& tm);

        /**
         * @brief Destroy the widget and free LVGL objects
         */
        void destroy();

        /**
         * @brief Get the LVGL container object
         * @return Container or nullptr
         */
        lv_obj_t* get_container() const;

        /**
         * @brief Set the geographic location for solar calculations
         * @param loc Location with latitude, longitude, timezone
         */
        void set_location(const core::Solar_Location& loc);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui::widgets
