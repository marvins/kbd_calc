/**
 * @file    clock_widget.hpp
 * @author  Marvin Smith
 * @date    2026-06-21
 *
 * @brief   Base clock widget interface
 */
#pragma once

// C++ Standard Libraries
#include <ctime>

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::gui::widgets {

/**
 * @brief Base clock widget interface
 *
 * All clock widgets must implement this interface to be swappable
 * in the status page.
 */
class I_Clock_Widget {
    public:
        virtual ~I_Clock_Widget() = default;

        /**
         * @brief Create the clock widget UI
         * @param parent LVGL parent object
         */
        virtual void create(lv_obj_t* parent) = 0;

        /**
         * @brief Update the clock display
         * @param tm Current time
         */
        virtual void update(const std::tm& tm) = 0;

        /**
         * @brief Destroy the clock widget UI
         */
        virtual void destroy() = 0;

        /**
         * @brief Get the LVGL container object
         * @return Container object or nullptr
         */
        virtual lv_obj_t* get_container() const = 0;
};

} // namespace ovb::gui::widgets
