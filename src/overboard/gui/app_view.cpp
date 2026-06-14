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
#include <overboard/apps/app_registration.hpp>
#include <overboard/gui/app_menu.hpp>
#include <overboard/gui/app_registry.hpp>
#include <overboard/gui/keyboard_display.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

// Use panel indices from app registration
using ovb::apps::PANEL_STATUS;
using ovb::apps::PANEL_CALCULATOR;
using ovb::apps::PANEL_SETTINGS;
using ovb::apps::PANEL_MENU;

/*******************************/
/*            Impl             */
/*******************************/
struct App_View::Impl {
    std::unique_ptr<Panel_Manager>    panels;
    std::unique_ptr<Keyboard_Display> keyboard_display;
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

    // Create app registry and register all apps
    App_Registry registry;
    apps::register_all_apps(
        registry,
        engine,
        layers,
        *m_impl->panels);

    // Get apps and menu items from registry
    auto apps = registry.create_apps();
    auto menu_items = registry.get_menu_items();
    LOG_DEBUG("App_View: Created ", std::to_string(apps.size()), " apps, ", std::to_string(menu_items.size()), " menu items");

    // App_Menu owns apps, registers them with Panel_Manager, and handles selection
    auto menu = std::make_shared<App_Menu>(std::move(apps), *m_impl->panels, std::move(menu_items),
        [this](int panel_index) {
            m_impl->panels->push(panel_index);
        }, 0);
    m_impl->menu_idx = m_impl->panels->register_panel(menu);
    LOG_DEBUG("App_View: Registered menu panel at index ", std::to_string(m_impl->menu_idx));

    // Boot to menu (first item is Status)
    m_impl->panels->push(m_impl->menu_idx);
    LOG_DEBUG("App_View: Pushed menu panel");

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
