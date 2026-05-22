/**
 * @file lvgl_keyboard.cpp
 * @author Marvin Smith
 * @date 2026-05-21
 * @brief LVGL keyboard manager implementation
 */
#include <overboard/hal/sdl/lvgl_keyboard.hpp>

// Project Libraries
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/sdl/lvgl_keyboard_display.hpp>
#include <overboard/hal/sdl/lvgl_keyboard_view.hpp>

namespace ovb::hal::sdl {

LVGL_Keyboard::LVGL_Keyboard( const std::string&            title,
                              int                           width,
                              int                           height,
                              const ovb::core::Grid_Layout& layout,
                              const core::Layer_Manager&    layers )
{
    m_display = std::make_unique<LVGL_Keyboard_Display>(title, width, height);
    m_view    = std::make_unique<LVGL_Keyboard_View>(
        m_display->screen(), layout, layers, width, height);
}

LVGL_Keyboard::~LVGL_Keyboard() = default;

int  LVGL_Keyboard::width()  const { return m_display->width();  }
int  LVGL_Keyboard::height() const { return m_display->height(); }

uint32_t LVGL_Keyboard::window_id() const {
    return m_display->window_id();
}

void LVGL_Keyboard::update_layer() {
    m_view->update_layer();
}

void LVGL_Keyboard::set_pressed(int key_index) {
    m_view->set_pressed(key_index);
}

void LVGL_Keyboard::clear_pressed() {
    m_view->clear_pressed();
}

void LVGL_Keyboard::render() {
    m_display->flush();
}

} // namespace ovb::hal::sdl
