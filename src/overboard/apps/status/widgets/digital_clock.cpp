/**
 * @file    digital_clock.cpp
 * @author  Marvin Smith
 * @date    2026-06-21
 *
 * @brief   Digital clock widget implementation
 */
#include <overboard/apps/status/widgets/digital_clock.hpp>

// C++ Standard Libraries
#include <iomanip>
#include <sstream>

namespace ovb::gui::widgets {

/*******************************/
/*         Constructor         */
/*******************************/
Digital_Clock::Digital_Clock() = default;

/*******************************/
/*          Destructor         */
/*******************************/
Digital_Clock::~Digital_Clock() {
    destroy();
}

/*******************************/
/*            Create           */
/*******************************/
void Digital_Clock::create(lv_obj_t* parent) {
    m_container = lv_obj_create(parent);
    lv_obj_set_size(m_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(m_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(m_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_container, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(m_container, false);

    // Use flex layout to stack labels vertically
    lv_obj_set_layout(m_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(m_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(m_container, 2, LV_PART_MAIN);

    // Time label
    m_time_label = lv_label_create(m_container);
    lv_obj_set_style_text_font(m_time_label, LVGL_FONT_LARGE, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_time_label, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_set_style_text_align(m_time_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // Date label
    m_date_label = lv_label_create(m_container);
    lv_obj_set_style_text_font(m_date_label, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_date_label, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    lv_obj_set_style_text_align(m_date_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
}

/*******************************/
/*            Update           */
/*******************************/
void Digital_Clock::update(const std::tm& tm) {
    // Create mutable copy for std::put_time (requires non-const pointer)
    std::tm tm_copy = tm;

    if (m_time_label) {
        std::ostringstream oss;
        oss << std::put_time(&tm_copy, "%H:%M:%S");
        lv_label_set_text(m_time_label, oss.str().c_str());
    }

    if (m_date_label) {
        std::ostringstream oss;
        oss << std::put_time(&tm_copy, "%A, %B %d");
        lv_label_set_text(m_date_label, oss.str().c_str());
    }
}

/*******************************/
/*           Destroy           */
/*******************************/
void Digital_Clock::destroy() {
    if (m_container) {
        lv_obj_del(m_container);
        m_container = nullptr;
    }
    m_time_label = nullptr;
    m_date_label = nullptr;
}

} // namespace ovb::gui::widgets
