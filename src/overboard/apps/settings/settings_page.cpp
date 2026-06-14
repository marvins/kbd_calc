/**
 * @file    settings_page.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Settings page panel implementation
 */
#include <overboard/apps/settings/settings_page.hpp>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Settings_Page::Impl {
    Back_Cb                      on_back;
    lv_obj_t*                    container  = nullptr;
    std::unique_ptr<Header_Bar>  header;
    std::unique_ptr<Footer_Bar>  footer;

    Impl(Back_Cb cb) : on_back(std::move(cb)) {}
};

/*******************************/
/*          Constructor        */
/*******************************/
Settings_Page::Settings_Page(Back_Cb on_back)
    : m_impl(std::make_unique<Impl>(std::move(on_back))) {}

/*******************************/
/*          Destructor         */
/*******************************/
Settings_Page::~Settings_Page() = default;

/*******************************/
/*           Activate          */
/*******************************/
void Settings_Page::activate(lv_obj_t* parent) {
    LOG_DEBUG("Settings_Page: activating");

    m_impl->container = lv_obj_create(parent);
    lv_obj_set_size(m_impl->container, lv_pct(100), lv_pct(100));
    lv_obj_align(m_impl->container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->container, LV_OBJ_FLAG_SCROLLABLE);

    // Use explicit dimensions - parent may not be laid out yet
    const int width = hal::LCD_WIDTH;

    // Header bar
    m_impl->header = std::make_unique<Header_Bar>(m_impl->container, width);
    m_impl->header->set_app_name("Settings");

    // Placeholder content
    const int content_y = Header_Bar::HEIGHT + 8;
    lv_obj_t* lbl = lv_label_create(m_impl->container);
    lv_label_set_text(lbl, "Settings (TODO)");
    lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 16, content_y);

    // Footer bar — decorative only, navigation via ESCAPE
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Settings_Page::deactivate() {
    LOG_DEBUG("Settings_Page: deactivating");
    m_impl->footer.reset();
    m_impl->header.reset();
    if (m_impl->container) {
        lv_obj_del(m_impl->container);
        m_impl->container = nullptr;
    }
}

/*******************************/
/*        Handle Input         */
/*******************************/
bool Settings_Page::handle_input(core::Action_Code action) {
    if (action == core::Action_Code::ESCAPE && m_impl->on_back) {
        m_impl->on_back();
    }
    return true;
}

/*******************************/
/*           Refresh           */
/*******************************/
void Settings_Page::refresh() {
    // Static page — nothing to update
}

} // namespace ovb::gui
