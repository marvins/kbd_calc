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

    lv_obj_t*              panel_container = nullptr;
    lv_obj_t*              kbd_container   = nullptr;
    core::Layer_Manager*   layers          = nullptr;
    int                    base_layer      { 0 };    ///< Layer index before shift was engaged
    bool                   shift_active    { false };
};

/*******************************/
/*          Destructor         */
/*******************************/
App_View::~App_View() = default;

/*******************************/
/*          Constructor        */
/*******************************/
App_View::App_View( lv_obj_t*                      root,
                    [[maybe_unused]] const ovb::core::Grid_Layout&  layout,
                    ovb::math::Calc_Engine&        engine,
                    ovb::core::Layer_Manager&      layers )
    : m_impl(std::make_unique<Impl>())
{
    m_impl->layers = &layers;
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

// Only create keyboard in main window if KBD_HEIGHT > 0 AND no separate keyboard window
// (when KBD_WIN_HEIGHT > 0, SDL_App creates a separate Keyboard_Window instead)
#if SHOW_KEYBOARD_UI
    if (hal::KBD_HEIGHT > 0 && hal::KBD_WIN_HEIGHT == 0) {
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
    } else {
        LOG_TRACE("App_View: Skipping keyboard creation (separate keyboard window will be used)");
    }
#endif
    LOG_TRACE("App_View: Constructor complete");
}

/****************************/
/*       Handle Input       */
/****************************/
void App_View::handle_input(core::Action_Code action) {
    LOG_DEBUG("App_View::handle_input: action_code=", std::to_string(static_cast<int>(action)), " (", core::action_code_to_display(action), ")");

    // Handle modifier keys globally — switch layer and let display update via callback
    if (m_impl->layers) {
        const int layer_count = m_impl->layers->layer_count();
        switch (action) {
            case core::Action_Code::LEFT_SHIFT:
            case core::Action_Code::RIGHT_SHIFT: {
                if (!m_impl->shift_active) {
                    // Find a layer named "Shift" (index 1 by convention)
                    m_impl->base_layer   = m_impl->layers->active_layer();
                    m_impl->shift_active = true;
                    const int shift_idx  = (layer_count > 1) ? 1 : 0;
                    m_impl->layers->set_layer(shift_idx);
                } else {
                    m_impl->shift_active = false;
                    m_impl->layers->set_layer(m_impl->base_layer);
                }
                return;
            }
            case core::Action_Code::CAPS_LOCK: {
                // Toggle: if already on Caps layer (index 2), return to base; else go to Caps
                const int caps_idx = (layer_count > 2) ? 2 : 0;
                if (m_impl->layers->active_layer() == caps_idx) {
                    m_impl->layers->set_layer(0);
                } else {
                    m_impl->layers->set_layer(caps_idx);
                }
                m_impl->shift_active = false;
                return;
            }
            default:
                break;
        }
        // After any non-modifier key press, release shift (momentary behaviour)
        if (m_impl->shift_active) {
            m_impl->shift_active = false;
            m_impl->layers->set_layer(m_impl->base_layer);
        }
    }

    // Handle function key popups (F1-F5) - only in Calculator panel
    I_Panel* active = m_impl->panels->active_panel();
    if (active && active->name() == "Calculator") {
        switch (action) {
            case core::Action_Code::FUNC_1:
            case core::Action_Code::FUNC_2:
            case core::Action_Code::FUNC_3:
            case core::Action_Code::FUNC_4:
            case core::Action_Code::FUNC_5:
                // TODO: Show function key popup (implement popup management)
                LOG_DEBUG("Function key pressed - popup not yet implemented");
                return; // Don't forward to panel
            default:
                break;
        }
    }

    m_impl->panels->handle_input(action);
}

/****************************/
/*       Handle Text        */
/****************************/
bool App_View::handle_text(char32_t codepoint) {
    LOG_DEBUG("App_View::handle_text: codepoint=", std::to_string(static_cast<uint32_t>(codepoint)));
    return m_impl->panels->handle_text(codepoint);
}

/******************************/
/*     Handle Input Key       */
/******************************/
void App_View::handle_input_key(core::Input_Key key) {
    LOG_DEBUG("App_View::handle_input_key: key=", core::input_key_to_string(key));
    m_impl->panels->handle_input_key(key);
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

/****************************/
/*  Get Active Panel Label  */
/****************************/
std::string App_View::get_active_panel_label(int key_index) const {
    if (auto* panel = m_impl->panels->active_panel()) {
        return panel->get_custom_label(key_index);
    }
    return "";
}

/****************************/
/*  Set Panel Change Callback */
/****************************/
void App_View::set_panel_change_callback(std::function<void(I_Panel*)> cb) {
    m_impl->panels->set_panel_change_callback(std::move(cb));
}

} // namespace ovb::gui
