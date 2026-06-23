/**
 * @file    digital_clock.hpp
 * @author  Marvin Smith
 * @date    2026-06-21
 *
 * @brief   Digital clock widget with time and date display
 */
#pragma once

// C++ Standard Libraries
#include <ctime>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/apps/status/widgets/clock_widget.hpp>
#include <overboard/gui/lvgl_theme.hpp>

namespace ovb::gui::widgets {

/**
 * @brief Digital clock widget
 *
 * Displays time in HH:MM:SS format and date below it.
 */
class Digital_Clock : public I_Clock_Widget {
    public:

        /**
         * @brief Constructor
         */
        Digital_Clock();

        /**
         * @brief Destructor
         */
        ~Digital_Clock() override;

        /**
         * @brief Create the digital clock UI
         * @param parent Parent LVGL object
         */
        void create(lv_obj_t* parent) override;

        /**
         * @brief Update the digital clock with current time
         * @param tm Current time structure
         */
        void update(const std::tm& tm) override;

        /**
         * @brief Destroy the digital clock UI
         */
        void destroy() override;

        /**
         * @brief Get the LVGL container object
         * @return Container object or nullptr
         */
        lv_obj_t* get_container() const override { return m_container; }

    private:

        /// @brief Time label
        lv_obj_t*   m_time_label = nullptr;

        /// @brief Date label
        lv_obj_t*   m_date_label = nullptr;

        /// @brief Container object
        lv_obj_t*   m_container = nullptr;
};

} // namespace ovb::gui::widgets
