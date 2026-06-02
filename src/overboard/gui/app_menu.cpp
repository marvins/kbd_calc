/**
 * @file    app_menu.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Application menu panel implementation
 */
#include <overboard/gui/app_menu.hpp>

// C++ Standard Libraries
#include <string_view>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/gui/app_registry.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct App_Menu::Impl {
    Select_Cb                  on_select;
    int                        initial_selection;
    lv_obj_t*                  container  = nullptr;
    std::unique_ptr<Header_Bar> header;
    std::unique_ptr<Footer_Bar> footer;
    lv_obj_t*                  list       = nullptr;
    int                        selected   = 0;

    Impl(Select_Cb cb, int init) : on_select(std::move(cb)), initial_selection(init) {}
};

/*******************************/
/*          Constructor        */
/*******************************/
App_Menu::App_Menu(Select_Cb on_select, int initial_selection)
    : m_impl(std::make_unique<Impl>(std::move(on_select), initial_selection)) {}

/*******************************/
/*          Destructor         */
/*******************************/
App_Menu::~App_Menu() = default;

/*******************************/
/*           Activate          */
/*******************************/
void App_Menu::activate(lv_obj_t* parent) {
    LOG_DEBUG("App_Menu: activating");
    m_impl->selected = m_impl->initial_selection;

    m_impl->container = lv_obj_create(parent);
    lv_obj_set_size(m_impl->container, lv_pct(100), lv_pct(100));
    lv_obj_align(m_impl->container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->container, LV_OBJ_FLAG_SCROLLABLE);

    const int width = lv_obj_get_width(parent);

    // Header bar
    m_impl->header = std::make_unique<Header_Bar>(m_impl->container, width);
    m_impl->header->set_app_name("Menu");

    // Menu list (centered between header and footer)
    const int list_h = lv_obj_get_height(parent) - Header_Bar::HEIGHT - Footer_Bar::HEIGHT;
    m_impl->list = lv_list_create(m_impl->container);
    lv_obj_set_size(m_impl->list, width - 32, list_h - 32);
    lv_obj_align(m_impl->list, LV_ALIGN_TOP_MID, 0, Header_Bar::HEIGHT + 16);
    lv_obj_set_style_bg_color(m_impl->list, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->list, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->list, 8, LV_PART_MAIN);

    // Add menu items using registry
    for (int i = 0; i < MAIN_APP_COUNT; ++i) {
        Panel_Info info = get_panel_info(i);
        lv_obj_t* btn = lv_list_add_btn(m_impl->list, info.symbol.data(), info.name.data());
        // Store panel index in user data for selection callback
        lv_obj_set_user_data(btn, reinterpret_cast<void*>(static_cast<intptr_t>(i)));
    }

    // Highlight selected item after buttons are created
    update_selection();

    // Footer bar — decorative
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void App_Menu::deactivate() {
    LOG_DEBUG("App_Menu: deactivating");
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
bool App_Menu::handle_input(core::Action_Code action) {
    LOG_DEBUG("App_Menu::handle_input: action_code=", std::to_string(static_cast<int>(action)), " (", core::action_code_to_display(action), ")");
    switch (action) {
        case core::Action_Code::ESCAPE:
            // ESCAPE from menu pops back to previous panel
            return false; // Let Panel_Manager handle pop
        case core::Action_Code::CURSOR_DOWN:
            if (m_impl->selected < MAIN_APP_COUNT - 1) {
                m_impl->selected++;
                update_selection();
            }
            return true;
        case core::Action_Code::CURSOR_UP:
            if (m_impl->selected > 0) {
                m_impl->selected--;
                update_selection();
            }
            return true;
        case core::Action_Code::EVAL:
            select_current();
            return true;
        default:
            return false;
    }
}

/*******************************/
/*           Refresh           */
/*******************************/
void App_Menu::refresh() {
    // Static menu — nothing to update
}

/*******************************/
/*            Name             */
/*******************************/
std::string App_Menu::name() const {
    return "Menu";
}

/*******************************/
/*       Update Selection      */
/*******************************/
void App_Menu::update_selection() {
    if (!m_impl->list) {
        return;
    }

    // Iterate through list children to find buttons
    uint32_t child_count = lv_obj_get_child_cnt(m_impl->list);
    int button_idx = 0;
    for (uint32_t i = 0; i < child_count && button_idx < MAIN_APP_COUNT; ++i) {
        lv_obj_t* child = lv_obj_get_child(m_impl->list, static_cast<int32_t>(i));
        if (child && lv_obj_has_class(child, &lv_button_class)) {
            if (button_idx == m_impl->selected) {
                lv_obj_add_state(child, LV_STATE_FOCUSED);
                lv_obj_add_state(child, LV_STATE_PRESSED);
            } else {
                lv_obj_clear_state(child, LV_STATE_FOCUSED);
                lv_obj_clear_state(child, LV_STATE_PRESSED);
            }
            button_idx++;
        }
    }
}

/*******************************/
/*       Select Current        */
/*******************************/
void App_Menu::select_current() {
    if (!m_impl->list || !m_impl->on_select) {
        return;
    }

    if (m_impl->selected >= 0 && m_impl->selected < MAIN_APP_COUNT) {
        std::string_view name = panel_index_to_name(m_impl->selected);
        LOG_DEBUG("App_Menu: selecting panel '", std::string(name), "'");
        m_impl->on_select(std::string(name));
    }
}

} // namespace ovb::gui
