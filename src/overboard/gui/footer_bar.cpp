/**
 * @file    footer_bar.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   LVGL footer bar implementation
 */
#include <overboard/gui/footer_bar.hpp>

// C++ Standard Libraries
#include <array>
#include <string>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Footer_Bar::Impl {
    lv_obj_t* bar = nullptr;
    Slot_Cb   on_click;

    struct Slot {
        lv_obj_t* btn       = nullptr;
        lv_obj_t* key_label = nullptr;  ///< "F1" … "F5"
        lv_obj_t* sub_label = nullptr;  ///< Caller-supplied subtitle
    };
    std::array<Slot, SLOT_COUNT> slots;
};

/*******************************/
/*          Constructor        */
/*******************************/
Footer_Bar::Footer_Bar(lv_obj_t* parent, int width)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->bar = lv_obj_create(parent);
    lv_obj_set_size(m_impl->bar, width, HEIGHT);
    lv_obj_align(m_impl->bar, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->bar, lvgl_color(LVGL_COLOR_KBD_HEADER), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->bar, 0, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->bar, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_border_side(m_impl->bar, LV_BORDER_SIDE_TOP, LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->bar, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->bar, LV_OBJ_FLAG_SCROLLABLE);

    const int slot_w = width / SLOT_COUNT;

    for (int i = 0; i < SLOT_COUNT; ++i) {
        auto& slot = m_impl->slots[static_cast<std::size_t>(i)];

        slot.btn = lv_btn_create(m_impl->bar);
        lv_obj_set_size(slot.btn, slot_w - 2, HEIGHT - 4);
        lv_obj_align(slot.btn, LV_ALIGN_LEFT_MID, i * slot_w + 1, 0);
        lv_obj_set_style_bg_color(slot.btn, lvgl_color(LVGL_COLOR_KBD_BUTTON), LV_PART_MAIN);
        lv_obj_set_style_bg_color(slot.btn, lvgl_color(LVGL_COLOR_KBD_BUTTON_PRESSED), static_cast<lv_style_selector_t>(static_cast<uint32_t>(LV_PART_MAIN) | static_cast<uint32_t>(LV_STATE_PRESSED)));
        lv_obj_set_style_border_color(slot.btn, lvgl_color(LVGL_COLOR_BORDER_DARK), LV_PART_MAIN);
        lv_obj_set_style_border_width(slot.btn, 1, LV_PART_MAIN);
        lv_obj_set_style_radius(slot.btn, 3, LV_PART_MAIN);
        lv_obj_set_style_pad_all(slot.btn, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(slot.btn, 0, LV_PART_MAIN);
        lv_obj_set_user_data(slot.btn, reinterpret_cast<void*>(static_cast<intptr_t>(i)));
        lv_obj_add_event_cb(slot.btn, Footer_Bar::slot_event_cb, LV_EVENT_CLICKED, this);

        // "F1" key indicator (top, small)
        slot.key_label = lv_label_create(slot.btn);
        const std::string key_str = "F" + std::to_string(i + 1);
        lv_label_set_text(slot.key_label, key_str.c_str());
        lv_obj_set_style_text_font(slot.key_label, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(slot.key_label, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
        lv_obj_align(slot.key_label, LV_ALIGN_TOP_MID, 0, 0);

        // Subtitle label (centre)
        slot.sub_label = lv_label_create(slot.btn);
        lv_label_set_text(slot.sub_label, "");
        lv_obj_set_style_text_font(slot.sub_label, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(slot.sub_label, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        lv_obj_align(slot.sub_label, LV_ALIGN_BOTTOM_MID, 0, 0);
    }
}

/*******************************/
/*          Destructor         */
/*******************************/
Footer_Bar::~Footer_Bar() = default;

/*******************************/
/*         Set Label           */
/*******************************/
void Footer_Bar::set_label(int slot, const std::string& label) {
    if (slot < 0 || slot >= SLOT_COUNT) return;
    auto& s = m_impl->slots[static_cast<std::size_t>(slot)];
    if (s.sub_label) {
        lv_label_set_text(s.sub_label, label.c_str());
    }
}

/*******************************/
/*     Set Click Callback      */
/*******************************/
void Footer_Bar::set_click_callback(Slot_Cb cb) {
    m_impl->on_click = std::move(cb);
}

/*******************************/
/*           Refresh           */
/*******************************/
void Footer_Bar::refresh() {
    if (m_impl->bar) {
        lv_obj_invalidate(m_impl->bar);
    }
}

/*******************************/
/*       Slot Event CB         */
/*******************************/
void Footer_Bar::slot_event_cb(lv_event_t* e) {
    auto* bar = static_cast<Footer_Bar*>(lv_event_get_user_data(e));
    auto* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
    const int slot = static_cast<int>(reinterpret_cast<intptr_t>(lv_obj_get_user_data(btn)));
    LOG_DEBUG("Footer_Bar: slot ", std::to_string(slot), " clicked");
    if (bar->m_impl->on_click) {
        bar->m_impl->on_click(slot);
    }
}

} // namespace ovb::gui
