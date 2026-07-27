/**
 * @file    analog_clock.cpp
 * @author  Marvin Smith
 * @date    2026-06-21
 *
 * @brief   Analog clock widget implementation
 */
#include <overboard/apps/status/widgets/analog_clock.hpp>

// C++ Standard Libraries
#include <cmath>

// Project Libraries
#include <overboard/gui/header_bar.hpp>

namespace ovb::gui::widgets {

/*******************************/
/*         Constructor         */
/*******************************/
Analog_Clock::Analog_Clock() = default;

/*******************************/
/*          Destructor         */
/*******************************/
Analog_Clock::~Analog_Clock() {
    destroy();
}

/*******************************/
/*            Create           */
/*******************************/
void Analog_Clock::create(lv_obj_t* parent) {

    // Circular white background with black border
    m_circle_bg = lv_obj_create(parent);
    lv_obj_set_size(m_circle_bg, CLOCK_SIZE, CLOCK_SIZE);
    lv_obj_set_style_bg_color(m_circle_bg, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(m_circle_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(m_circle_bg, CLOCK_SIZE / 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(m_circle_bg, 3, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_circle_bg, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_side(m_circle_bg, LV_BORDER_SIDE_FULL, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_circle_bg, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(m_circle_bg, false);

    // Hour ticks (12 thick ticks at 5-minute intervals)
    const float center = CLOCK_SIZE / 2.0f;
    const float hour_tick_radius = (CLOCK_SIZE / 2.0f) - 1.5f;  // Match border radius
    const float hour_tick_length = 15;

    static lv_point_precise_t hour_tick_points[12][2];

    for (int i = 0; i < 12; ++i) {
        m_hour_ticks[i] = lv_line_create(m_circle_bg);
        lv_obj_set_style_line_width(m_hour_ticks[i], 3, LV_PART_MAIN);
        lv_obj_set_style_line_color(m_hour_ticks[i], lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_line_rounded(m_hour_ticks[i], true, LV_PART_MAIN);

        // Start from 12 o'clock (270 degrees) and go clockwise every 30 degrees
        const float angle_deg = (static_cast<float>(i) * 30.0f) + 270.0f;
        const float angle_rad = angle_deg * 3.14159265f / 180.0f;
        const float inner_r = hour_tick_radius - hour_tick_length;
        const float outer_r = hour_tick_radius;

        hour_tick_points[i][0] = { static_cast<lv_value_precise_t>(center + inner_r * std::cos(angle_rad)),
                                    static_cast<lv_value_precise_t>(center + inner_r * std::sin(angle_rad)) };
        hour_tick_points[i][1] = { static_cast<lv_value_precise_t>(center + outer_r * std::cos(angle_rad)),
                                    static_cast<lv_value_precise_t>(center + outer_r * std::sin(angle_rad)) };
        lv_line_set_points(m_hour_ticks[i], hour_tick_points[i], 2);
    }

    // Minute ticks (60 thin ticks)
    const float minute_tick_radius = (CLOCK_SIZE / 2.0f) - 1.5f;  // Match border radius
    const float minute_tick_length = 10;

    static lv_point_precise_t minute_tick_points[60][2];

    for (int i = 0; i < 60; ++i) {
        // Skip hour positions (every 5 minutes)
        if (i % 5 == 0) continue;

        m_minute_ticks[i] = lv_line_create(m_circle_bg);
        lv_obj_set_style_line_width(m_minute_ticks[i], 1, LV_PART_MAIN);
        lv_obj_set_style_line_color(m_minute_ticks[i], lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_line_rounded(m_minute_ticks[i], true, LV_PART_MAIN);

        // Start from 12 o'clock (270 degrees) and go clockwise
        const float angle_deg = (static_cast<float>(i) * 6.0f) + 270.0f;
        const float angle_rad = angle_deg * 3.14159265f / 180.0f;
        const float inner_r = minute_tick_radius - minute_tick_length;
        const float outer_r = minute_tick_radius;

        minute_tick_points[i][0] = { static_cast<lv_value_precise_t>(center + inner_r * std::cos(angle_rad)),
                                      static_cast<lv_value_precise_t>(center + inner_r * std::sin(angle_rad)) };
        minute_tick_points[i][1] = { static_cast<lv_value_precise_t>(center + outer_r * std::cos(angle_rad)),
                                      static_cast<lv_value_precise_t>(center + outer_r * std::sin(angle_rad)) };
        lv_line_set_points(m_minute_ticks[i], minute_tick_points[i], 2);
    }

    // Center cap for the hands
    lv_obj_t* cap = lv_obj_create(m_circle_bg);
    lv_obj_set_size(cap, 12, 12);
    lv_obj_center(cap);
    lv_obj_set_style_radius(cap, 6, LV_PART_MAIN);
    lv_obj_set_style_bg_color(cap, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(cap, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(cap, false);

    // Hour hand line
    m_hour_needle = lv_line_create(m_circle_bg);
    lv_obj_set_style_line_width(m_hour_needle, 4, LV_PART_MAIN);
    lv_obj_set_style_line_color(m_hour_needle, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(m_hour_needle, true, LV_PART_MAIN);

    // Minute hand line
    m_minute_needle = lv_line_create(m_circle_bg);
    lv_obj_set_style_line_width(m_minute_needle, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(m_minute_needle, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(m_minute_needle, true, LV_PART_MAIN);

    // Second hand line
    m_second_needle = lv_line_create(m_circle_bg);
    lv_obj_set_style_line_width(m_second_needle, 1, LV_PART_MAIN);
    lv_obj_set_style_line_color(m_second_needle, lv_color_hex(0x0000FF), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(m_second_needle, true, LV_PART_MAIN);

    // Custom clock numerals
    auto add_numeral = [&](const char* text, int dx, int dy) {
        lv_obj_t* num = lv_label_create(m_circle_bg);
        lv_label_set_text(num, text);
        lv_obj_set_style_text_font(num, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(num, lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_align(num, LV_ALIGN_CENTER, dx, dy);
        return num;
    };
    const int label_radius = 82;
    add_numeral("12", 0, -label_radius);
    add_numeral("3", label_radius, 0);
    add_numeral("6", 0, label_radius);
    add_numeral("9", -label_radius, 0);
}

/*******************************/
/*            Update           */
/*******************************/
void Analog_Clock::update(const std::tm& tm) {
    if (m_circle_bg && m_hour_needle) {
        update_hands(m_hour_needle, m_minute_needle, m_second_needle, tm);
    }
}

/*******************************/
/*           Destroy           */
/*******************************/
void Analog_Clock::destroy() {
    m_circle_bg = nullptr;
    m_hour_needle = nullptr;
    m_minute_needle = nullptr;
    m_second_needle = nullptr;
}

/*******************************/
/*         Update Hands         */
/*******************************/
void Analog_Clock::update_hands(lv_obj_t* hour, lv_obj_t* minute, lv_obj_t* second, const std::tm& tm) {
    const float center = CLOCK_SIZE / 2.0f;
    const int hours = tm.tm_hour % 12;
    const int minutes = tm.tm_min;
    const int seconds = tm.tm_sec;

    const float hour_angle = (static_cast<float>(hours) * 30.0f + static_cast<float>(minutes) * 0.5f - 90.0f) * 3.14159265f / 180.0f;
    const float minute_angle = (static_cast<float>(minutes) * 6.0f - 90.0f) * 3.14159265f / 180.0f;
    const float second_angle = (static_cast<float>(seconds) * 6.0f - 90.0f) * 3.14159265f / 180.0f;

    const float hour_len = CLOCK_SIZE * 0.30f;
    const float minute_len = CLOCK_SIZE * 0.42f;
    const float second_len = CLOCK_SIZE * 0.45f;

    static lv_point_precise_t hour_pts[2];
    hour_pts[0] = { static_cast<lv_value_precise_t>(center), static_cast<lv_value_precise_t>(center) };
    hour_pts[1] = { static_cast<lv_value_precise_t>(center + hour_len * std::cos(hour_angle)),
                    static_cast<lv_value_precise_t>(center + hour_len * std::sin(hour_angle)) };
    lv_line_set_points(hour, hour_pts, 2);

    static lv_point_precise_t minute_pts[2];
    minute_pts[0] = { static_cast<lv_value_precise_t>(center), static_cast<lv_value_precise_t>(center) };
    minute_pts[1] = { static_cast<lv_value_precise_t>(center + minute_len * std::cos(minute_angle)),
                      static_cast<lv_value_precise_t>(center + minute_len * std::sin(minute_angle)) };
    lv_line_set_points(minute, minute_pts, 2);

    static lv_point_precise_t second_pts[2];
    second_pts[0] = { static_cast<lv_value_precise_t>(center), static_cast<lv_value_precise_t>(center) };
    second_pts[1] = { static_cast<lv_value_precise_t>(center + second_len * std::cos(second_angle)),
                      static_cast<lv_value_precise_t>(center + second_len * std::sin(second_angle)) };
    lv_line_set_points(second, second_pts, 2);
}

} // namespace ovb::gui::widgets
