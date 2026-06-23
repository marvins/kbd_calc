/**
 * @file    analog_clock.hpp
 * @author  Marvin Smith
 * @date    2026-06-21
 *
 * @brief   Analog clock widget with hour/minute/second hands
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
 * @brief Analog clock widget
 *
 * Displays a circular clock face with hour, minute, and second hands.
 * Includes 12, 3, 6, 9 numerals and tick marks.
 */
class Analog_Clock : public I_Clock_Widget {
    public:

        /**
         * @brief Constructor
         */
        Analog_Clock();

        /**
         * @brief Destructor
         */
        ~Analog_Clock() override;

        /**
         * @brief Create the analog clock UI
         * @param parent Parent LVGL object
         */
        void create(lv_obj_t* parent) override;

        /**
         * @brief Update the analog clock with current time
         * @param tm Current time structure
         */
        void update(const std::tm& tm) override;

        /**
         * @brief Destroy the analog clock UI
         */
        void destroy() override;

        /**
         * @brief Get the LVGL container object
         * @return Container object or nullptr
         */
        lv_obj_t* get_container() const override { return m_circle_bg; }

    private:

        /// @brief Clock size in pixels
        static constexpr int CLOCK_SIZE = 220;

        /// @brief Circular background object
        lv_obj_t*   m_circle_bg = nullptr;

        /// @brief Hour tick lines (12 ticks at 5-minute intervals)
        lv_obj_t*   m_hour_ticks[12] = { nullptr };

        /// @brief Minute tick lines (60 ticks)
        lv_obj_t*   m_minute_ticks[60] = { nullptr };

        /// @brief Hour needle object
        lv_obj_t*   m_hour_needle = nullptr;

        /// @brief Minute needle object
        lv_obj_t*   m_minute_needle = nullptr;

        /// Second needle object
        lv_obj_t*   m_second_needle = nullptr;

        static void update_hands(lv_obj_t* hour, lv_obj_t* minute, lv_obj_t* second, const std::tm& tm);
};

} // namespace ovb::gui::widgets
