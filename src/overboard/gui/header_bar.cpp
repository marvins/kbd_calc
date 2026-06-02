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
/*            Impl             */
/*******************************/
struct Header_Bar::Impl {
    lv_obj_t* bar   = nullptr;
    lv_obj_t* label = nullptr;
    std::string app_name;
};

/*******************************/
/*          Constructor        */
/*******************************/
Header_Bar::Header_Bar(lv_obj_t* parent, int width)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->bar = lv_obj_create(parent);
    lv_obj_set_size(m_impl->bar, width, HEIGHT);
    lv_obj_align(m_impl->bar, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->bar, lvgl_color(LVGL_COLOR_KBD_HEADER), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->bar, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->bar, LV_OBJ_FLAG_SCROLLABLE);

    m_impl->label = lv_label_create(m_impl->bar);
    lv_label_set_text(m_impl->label, "");
    lv_obj_set_style_text_color(m_impl->label, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    lv_obj_set_style_text_font(m_impl->label, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_align(m_impl->label, LV_ALIGN_LEFT_MID, 4, 0);
}

/*******************************/
/*          Destructor         */
/*******************************/
Header_Bar::~Header_Bar() = default;

/*******************************/
/*        Set App Name         */
/*******************************/
void Header_Bar::set_app_name(const std::string& name) {
    m_impl->app_name = name;
    if (m_impl->label) {
        lv_label_set_text(m_impl->label, name.c_str());
    }
}

/*******************************/
/*           Refresh           */
/*******************************/
void Header_Bar::refresh() {
    if (m_impl->label) {
        lv_obj_invalidate(m_impl->bar);
    }
}

} // namespace ovb::gui
