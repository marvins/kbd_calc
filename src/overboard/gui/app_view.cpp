/**
 * @file    app_view.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LVGL application view implementation
 */
#include <overboard/gui/app_view.hpp>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/app_menu.hpp>
#include <overboard/gui/app_registry.hpp>
#include <overboard/gui/calculator_app.hpp>
#include <overboard/gui/keyboard_display.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/gui/settings_page.hpp>
#include <overboard/gui/status_page.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct App_View::Impl {
    std::unique_ptr<Panel_Manager>    panels;
    std::unique_ptr<Keyboard_Display> keyboard_display;
    int                               calc_panel_idx { -1 };
    int                               status_idx     { -1 };
    int                               settings_idx   { -1 };
    int                               menu_idx       { -1 };
    int                               prev_panel_idx { PANEL_STATUS };

    lv_obj_t* panel_container = nullptr;
    lv_obj_t* kbd_container   = nullptr;
};

/*******************************/
/*          Destructor         */
/*******************************/
App_View::~App_View() = default;

/*******************************/
/*          Constructor        */
/*******************************/
App_View::App_View( lv_obj_t*                      root,
                    const ovb::core::Grid_Layout&  layout,
                    ovb::math::Calc_Engine&        engine,
                    ovb::core::Layer_Manager&      layers )
    : m_impl(std::make_unique<Impl>())
{
    LOG_TRACE("App_View: Applying baseline screen styling");
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_bg_color(root, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);

    LOG_TRACE("App_View: Creating panel container");
    m_impl->panel_container = lv_obj_create(root);
    lv_obj_set_size(m_impl->panel_container, hal::LCD_WIDTH, hal::LCD_HEIGHT);
    lv_obj_align(m_impl->panel_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->panel_container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->panel_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->panel_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->panel_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->panel_container, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("App_View: Setting up Panel_Manager");
    m_impl->panels = std::make_unique<Panel_Manager>(m_impl->panel_container);

    // Register all panels
    // Calculator: ESCAPE returns to menu (pop)
    auto calc = std::make_unique<Calculator_App>(engine, layers, [this]() {
        m_impl->prev_panel_idx = PANEL_CALCULATOR;
        m_impl->panels->pop();
    });
    m_impl->calc_panel_idx = m_impl->panels->register_panel(std::move(calc));

    // Settings: ESCAPE returns to menu (pop)
    auto settings = std::make_unique<Settings_Page>([this]() {
        m_impl->prev_panel_idx = PANEL_SETTINGS;
        m_impl->panels->pop();
    });
    m_impl->settings_idx = m_impl->panels->register_panel(std::move(settings));

    // App_Menu: selection pushes the selected panel
    auto menu = std::make_unique<App_Menu>([this](const std::string& panel_name) {
        if (panel_name == "Calculator") {
            m_impl->panels->push(m_impl->calc_panel_idx);
        } else if (panel_name == "Settings") {
            m_impl->panels->push(m_impl->settings_idx);
        } else if (panel_name == "Status") {
            m_impl->panels->push(m_impl->status_idx);
        }
    }, m_impl->prev_panel_idx);
    m_impl->menu_idx = m_impl->panels->register_panel(std::move(menu));

    // Status: ESCAPE goes to menu (push menu panel)
    auto status = std::make_unique<Status_Page>(layers, [this]() {
        m_impl->prev_panel_idx = PANEL_STATUS;
        m_impl->panels->push(m_impl->menu_idx);
    });
    m_impl->status_idx = m_impl->panels->register_panel(std::move(status));

    // Boot to Status (menu accessible via ESCAPE from other panels)
    m_impl->panels->push(m_impl->status_idx);

#if SHOW_KEYBOARD_UI
    LOG_TRACE("App_View: Creating keyboard container");
    m_impl->kbd_container = lv_obj_create(root);
    lv_obj_set_size(m_impl->kbd_container, hal::KBD_WIDTH, hal::KBD_HEIGHT);
    lv_obj_align(m_impl->kbd_container, LV_ALIGN_TOP_LEFT, 0, hal::LCD_HEIGHT);
    lv_obj_set_style_bg_color(m_impl->kbd_container, lvgl_color(LVGL_COLOR_KBD_BG), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->kbd_container, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("App_View: Creating Keyboard_Display");
    m_impl->keyboard_display = std::make_unique<Keyboard_Display>(
        m_impl->kbd_container, layout, layers, hal::KBD_WIDTH, hal::KBD_HEIGHT);
#endif
    LOG_TRACE("App_View: Constructor complete");
}

/****************************/
/*       Handle Input       */
/****************************/
void App_View::handle_input(core::Action_Code action) {
    LOG_DEBUG("App_View::handle_input: action_code=", std::to_string(static_cast<int>(action)), " (", core::action_code_to_display(action), ")");
    m_impl->panels->handle_input(action);
}

/****************************/
/*       Handle Text        */
/****************************/
void App_View::handle_text(char32_t codepoint) {
    LOG_DEBUG("App_View::handle_text: codepoint=", std::to_string(static_cast<uint32_t>(codepoint)));
    m_impl->panels->handle_text(codepoint);
}

/****************************/
/*          Refresh         */
/****************************/
void App_View::refresh() {
    m_impl->panels->refresh();
}

/******************************/
/*        Update Layer        */
/******************************/
void App_View::update_layer() {
#if SHOW_KEYBOARD_UI
    if (m_impl->keyboard_display) {
        m_impl->keyboard_display->update_layer();
    }
#endif
}

/*****************************************/
/*       Set Key Click Callback          */
/*****************************************/
void App_View::set_key_click_callback([[maybe_unused]] std::function<void(int)> cb) {
#if SHOW_KEYBOARD_UI
    if (m_impl->keyboard_display) {
        m_impl->keyboard_display->set_click_callback(std::move(cb));
    }
#endif
}

/****************************/
/*          Render          */
/****************************/
void App_View::render() {
    lv_timer_handler();
}

} // namespace ovb::gui
