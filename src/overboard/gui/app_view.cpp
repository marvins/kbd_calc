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
#include <overboard/gui/keyboard_display.hpp>
#include <overboard/gui/lcd_section.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct App_View::Impl {
    std::unique_ptr<LCD_Section>     lcd;
    std::unique_ptr<Keyboard_Display> keyboard_display;

    lv_obj_t* lcd_container = nullptr;
    lv_obj_t* kbd_container = nullptr;
};

/*******************************/
/*          Destructor         */
/*******************************/
App_View::~App_View() = default;

/*******************************/
/*          Constructor        */
/*******************************/
App_View::App_View( lv_obj_t*                       root,
                    [[maybe_unused]] const ovb::core::Grid_Layout&   layout,
                    const ovb::math::Calc_Engine&   engine,
                    const ovb::core::Layer_Manager& layers )
    : m_impl(std::make_unique<Impl>())
{
    LOG_TRACE("App_View: Applying baseline screen styling");
    // Apply baseline screen styling (zeroing LVGL defaults)
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_bg_color(root, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);

    LOG_TRACE("App_View: Creating LCD container");
    // LCD container (top section)
    m_impl->lcd_container = lv_obj_create(root);
    lv_obj_set_size(m_impl->lcd_container, hal::LCD_WIDTH, hal::LCD_HEIGHT);
    lv_obj_align(m_impl->lcd_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->lcd_container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->lcd_container, LV_OBJ_FLAG_SCROLLABLE);

    LOG_TRACE("App_View: Creating LCD_Section");
    m_impl->lcd = std::make_unique<LCD_Section>(engine, layers);
    LOG_TRACE("App_View: Building LCD_Section");
    m_impl->lcd->build(m_impl->lcd_container);
    LOG_TRACE("App_View: LCD_Section built successfully");

#if SHOW_KEYBOARD_UI
    LOG_TRACE("App_View: Creating keyboard container");
    // Keyboard container (bottom section)
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
/*          Refresh         */
/****************************/
void App_View::refresh() {
    m_impl->lcd->refresh();
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
