/**
 * @file    function_menu_popup.cpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Function menu popup implementation
 */

// C++ Standard Libraries
#include <algorithm>

// Project Libraries
#include <overboard/font/font_selector.hpp>
#include <overboard/gui/function_menu_popup.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

// External font declaration
LV_FONT_DECLARE(lv_font_superscript);

namespace ovb::gui {

/****************************/
/*      Implementation      */
/****************************/
struct Function_Menu_Popup::Impl {
    lv_obj_t*              parent;
    lv_obj_t*              container;
    lv_obj_t*              list;
    std::string            title;
    std::vector<Function_Menu_Item> items;
    std::vector<lv_obj_t*> item_labels;  // Track menu item labels for scrolling
    Select_Cb              on_select;
    int                    selected_index;
    bool                   visible;

    Impl(lv_obj_t*                     p,
         const std::string&            t,
         const std::vector<Function_Menu_Item>& itms,
         Select_Cb                     cb)
        : parent(p)
        , container(nullptr)
        , list(nullptr)
        , title(t)
        , items(itms)
        , on_select(std::move(cb))
        , selected_index(0)
        , visible(false)
    {
        LOG_DEBUG("Function_Menu_Popup::Impl: constructor, itms.size()=" + std::to_string(itms.size()) + ", items.size()=" + std::to_string(items.size()));
    }
};

/****************************/
/*       Constructor        */
/****************************/
Function_Menu_Popup::Function_Menu_Popup(lv_obj_t*                     parent,
                                         const std::string&            title,
                                         const std::vector<Function_Menu_Item>& items,
                                         Select_Cb                     on_select)
    : m_impl(std::make_unique<Impl>(parent, title, items, std::move(on_select)))
{
    LOG_DEBUG("Function_Menu_Popup: constructor complete, items.size()=" + std::to_string(m_impl->items.size()));
}

/****************************/
/*        Destructor        */
/****************************/
Function_Menu_Popup::~Function_Menu_Popup() {
    hide();
}

/****************************/
/*          Show            */
/****************************/
void Function_Menu_Popup::show() {
    if (m_impl->visible) return;

    // Create modal container - position flush with footer bar
    m_impl->container = lv_obj_create(m_impl->parent);
    lv_obj_set_size(m_impl->container, LV_PCT(90), 180);
    lv_obj_align(m_impl->container, LV_ALIGN_BOTTOM_MID, 0, -28);  // -28 = Footer_Bar::HEIGHT
    lv_obj_set_style_bg_color(m_impl->container, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->container, lvgl_color(LVGL_COLOR_ACCENT_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->container, 8, LV_PART_MAIN);

    // Title label - small region at top
    constexpr int TITLE_HEIGHT = 30;
    lv_obj_t* title_label = lv_label_create(m_impl->container);
    lv_label_set_text(title_label, m_impl->title.c_str());
    lv_obj_set_style_text_font(title_label, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_label, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_set_height(title_label, TITLE_HEIGHT);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 8);

    // Create list - full width, positioned below title
    m_impl->list = lv_obj_create(m_impl->container);
    lv_obj_set_size(m_impl->list, LV_PCT(100), 180 - TITLE_HEIGHT);
    lv_obj_set_pos(m_impl->list, 0, TITLE_HEIGHT);
    lv_obj_set_flex_flow(m_impl->list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(m_impl->list, 8, LV_PART_MAIN);
    lv_obj_set_style_bg_color(m_impl->list, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->list, 0, LV_PART_MAIN);
    lv_obj_set_scroll_dir(m_impl->list, LV_DIR_VER);  // Enable vertical scrolling
    lv_obj_set_scrollbar_mode(m_impl->list, LV_SCROLLBAR_MODE_AUTO);

    // Add menu items
    m_impl->item_labels.clear();
    for (size_t i = 0; i < m_impl->items.size(); ++i) {
        lv_obj_t* item = lv_label_create(m_impl->list);
        lv_label_set_text(item, m_impl->items[i].label.c_str());

        // Use custom font for math symbols
        if (font::requires_custom_font(m_impl->items[i].label)) {
            lv_obj_set_style_text_font(item, &lv_font_superscript, LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_font(item, LVGL_FONT_DEFAULT, LV_PART_MAIN);
        }

        lv_obj_set_style_pad_all(item, 8, LV_PART_MAIN);

        // Highlight selected item
        if (i == static_cast<size_t>(m_impl->selected_index)) {
            lv_obj_set_style_bg_color(item, lvgl_color(LVGL_COLOR_ACCENT_BLUE), LV_PART_MAIN);
            lv_obj_set_style_bg_opa(item, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_text_color(item, lv_color_white(), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(item, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        }

        m_impl->item_labels.push_back(item);
    }

    // Scroll to selected item
    if (m_impl->selected_index >= 0 && m_impl->selected_index < static_cast<int>(m_impl->item_labels.size())) {
        lv_obj_scroll_to_view(m_impl->item_labels[static_cast<size_t>(m_impl->selected_index)], LV_ANIM_OFF);
    }

    m_impl->visible = true;
    LOG_DEBUG("Function_Menu_Popup: shown - ", m_impl->title);
}

/****************************/
/*          Hide            */
/****************************/
void Function_Menu_Popup::hide() {
    if (!m_impl->visible) return;

    if (m_impl->container) {
        lv_obj_del(m_impl->container);
        m_impl->container = nullptr;
        m_impl->list = nullptr;
    }

    m_impl->item_labels.clear();
    m_impl->visible = false;
    LOG_DEBUG("Function_Menu_Popup: hidden");
}

/****************************/
/*      Handle Input        */
/****************************/
bool Function_Menu_Popup::handle_input(core::Input_Key key) {
    if (!m_impl->visible) return false;

    switch (key) {
        case core::Input_Key::UP:
            if (m_impl->selected_index > 0) {
                m_impl->selected_index--;
                LOG_DEBUG("Function_Menu_Popup: UP - selected_index=", std::to_string(m_impl->selected_index));
                render();
            }
            return true;

        case core::Input_Key::DOWN:
            if (m_impl->selected_index < static_cast<int>(m_impl->items.size()) - 1) {
                m_impl->selected_index++;
                LOG_DEBUG("Function_Menu_Popup: DOWN - selected_index=", std::to_string(m_impl->selected_index));
                render();
            }
            return true;

        case core::Input_Key::RETURN:
            // Enter/Return selects menu item
            if (m_impl->on_select && m_impl->selected_index >= 0 &&
                m_impl->selected_index < static_cast<int>(m_impl->items.size())) {
                auto idx = static_cast<size_t>(m_impl->selected_index);
                core::Action_Code action = m_impl->items[idx].action;
                LOG_DEBUG("Function_Menu_Popup: selected - ", m_impl->items[idx].label);
                hide();
                m_impl->on_select(action);
            }
            return true;

        case core::Input_Key::ESCAPE:
            hide();
            return true;

        default:
            return false;
    }
}

/****************************/
/*         Render           */
/****************************/
void Function_Menu_Popup::render() {
    if (!m_impl->visible || !m_impl->list) return;

    // Re-render list with updated selection
    lv_obj_clean(m_impl->list);
    m_impl->item_labels.clear();

    for (size_t i = 0; i < m_impl->items.size(); ++i) {
        lv_obj_t* item = lv_label_create(m_impl->list);
        lv_label_set_text(item, m_impl->items[i].label.c_str());

        // Use custom font for math symbols
        if (font::requires_custom_font(m_impl->items[i].label)) {
            lv_obj_set_style_text_font(item, &lv_font_superscript, LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_font(item, LVGL_FONT_DEFAULT, LV_PART_MAIN);
        }

        lv_obj_set_style_pad_all(item, 8, LV_PART_MAIN);

        // Highlight selected item
        if (i == static_cast<size_t>(m_impl->selected_index)) {
            lv_obj_set_style_bg_color(item, lvgl_color(LVGL_COLOR_ACCENT_BLUE), LV_PART_MAIN);
            lv_obj_set_style_bg_opa(item, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_text_color(item, lv_color_white(), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(item, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
        }

        m_impl->item_labels.push_back(item);
    }

    // Scroll to selected item with animation
    if (m_impl->selected_index >= 0 && m_impl->selected_index < static_cast<int>(m_impl->item_labels.size())) {
        lv_obj_scroll_to_view(m_impl->item_labels[static_cast<size_t>(m_impl->selected_index)], LV_ANIM_ON);
    }
}

/****************************/
/*         Items            */
/****************************/
const std::vector<Function_Menu_Item>& Function_Menu_Popup::items() const {
    return m_impl->items;
}

} // namespace ovb::gui
