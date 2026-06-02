/**
 * @file    status_page.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Status page panel implementation
 */
#include <overboard/gui/status_page.hpp>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>
#include <overboard/version.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Status_Page::Impl {
    const core::Layer_Manager&   layers;
    Dismiss_Cb                   on_dismiss;
    lv_obj_t*                    container  = nullptr;
    std::unique_ptr<Header_Bar>  header;
    std::unique_ptr<Footer_Bar>  footer;
    bool                         dismissed  = false;

    Impl(const core::Layer_Manager& l, Dismiss_Cb cb)
        : layers(l), on_dismiss(std::move(cb)) {}
};

/*******************************/
/*          Constructor        */
/*******************************/
Status_Page::Status_Page(const core::Layer_Manager& layers, Dismiss_Cb on_dismiss)
    : m_impl(std::make_unique<Impl>(layers, std::move(on_dismiss))) {}

/*******************************/
/*          Destructor         */
/*******************************/
Status_Page::~Status_Page() = default;

/*******************************/
/*           Activate          */
/*******************************/
void Status_Page::activate(lv_obj_t* parent) {
    LOG_DEBUG("Status_Page: activating");
    m_impl->dismissed = false;

    m_impl->container = lv_obj_create(parent);
    lv_obj_set_size(m_impl->container, lv_pct(100), lv_pct(100));
    lv_obj_align(m_impl->container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->container, LV_OBJ_FLAG_SCROLLABLE);

    // Header bar
    const int width = lv_obj_get_width(parent);
    m_impl->header = std::make_unique<Header_Bar>(m_impl->container, width);
    m_impl->header->set_app_name("kbd_calc — Status");

    // Info labels (inset from header + footer)
    const int content_y = Header_Bar::HEIGHT + 8;
    const std::string version_str = "v" + std::string(ovb::PROJECT_VERSION_DISPLAY);
    const std::string build_str   = "Built: " + std::string(ovb::BUILD_DATE);
    const std::string layers_str  = "Layers: " + std::to_string(m_impl->layers.layer_count());

    auto make_label = [&](const std::string& text, int y_offset) {
        lv_obj_t* lbl = lv_label_create(m_impl->container);
        lv_label_set_text(lbl, text.c_str());
        lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 16, content_y + y_offset);
    };

    make_label("kbd_calc",    0);
    make_label(version_str,  24);
    make_label(build_str,    48);
    make_label(layers_str,   72);

    // Footer bar — decorative only, navigation via ESCAPE
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Status_Page::deactivate() {
    LOG_DEBUG("Status_Page: deactivating");
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
bool Status_Page::handle_input(core::Action_Code action) {
    if (action == core::Action_Code::ESCAPE
        && !m_impl->dismissed
        && m_impl->on_dismiss)
    {
        m_impl->dismissed = true;
        m_impl->on_dismiss();
    }
    return true;
}

/*******************************/
/*           Refresh           */
/*******************************/
void Status_Page::refresh() {
    // Static page — nothing to update
}

/*******************************/
/*            Name             */
/*******************************/
std::string Status_Page::name() const {
    return "Status";
}

} // namespace ovb::gui
