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
#include <overboard/gui/keyboard_view.hpp>
#include <overboard/gui/lcd_section.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/sdl/lvgl_theme.hpp>

using namespace ovb::hal::sdl;

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct App_View::Impl {
    std::unique_ptr<LCD_Section>   lcd;
    std::unique_ptr<Keyboard_View> keyboard_view;

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
                    const ovb::core::Grid_Layout&   layout,
                    const ovb::math::Calc_Engine&   engine,
                    const ovb::core::Layer_Manager& layers )
    : m_impl(std::make_unique<Impl>())
{
    // LCD container (top section)
    m_impl->lcd_container = lv_obj_create(root);
    lv_obj_set_size(m_impl->lcd_container, hal::LCD_WIDTH, hal::LCD_HEIGHT);
    lv_obj_align(m_impl->lcd_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->lcd_container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->lcd_container, LV_OBJ_FLAG_SCROLLABLE);

    m_impl->lcd = std::make_unique<LCD_Section>(engine, layers);
    m_impl->lcd->build(m_impl->lcd_container);

    // Keyboard container (bottom section)
    m_impl->kbd_container = lv_obj_create(root);
    lv_obj_set_size(m_impl->kbd_container, hal::KBD_WIDTH, hal::KBD_HEIGHT);
    lv_obj_align(m_impl->kbd_container, LV_ALIGN_TOP_LEFT, 0, hal::LCD_HEIGHT);
    lv_obj_set_style_bg_color(m_impl->kbd_container, lvgl_color(LVGL_COLOR_KBD_BG), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->kbd_container, LV_OBJ_FLAG_SCROLLABLE);

    m_impl->keyboard_view = std::make_unique<Keyboard_View>(
        m_impl->kbd_container, layout, layers, hal::KBD_WIDTH, hal::KBD_HEIGHT);
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
    m_impl->keyboard_view->update_layer();
}

/****************************/
/*          Render          */
/****************************/
void App_View::render() {
    lv_timer_handler();
}

/************************************/
/*        Set Key Callback          */
/************************************/
void App_View::set_key_callback( void (*cb)(int key_index, void* user_data),
                                 void* user_data ) {
    m_impl->keyboard_view->set_click_callback(cb, user_data);
}

} // namespace ovb::gui
