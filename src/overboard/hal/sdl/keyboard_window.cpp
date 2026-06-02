/**
 * @file    keyboard_window.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   SDL keyboard window implementation
 */
#include <overboard/hal/sdl/keyboard_window.hpp>

// C++ Standard Libraries
#include <stdexcept>

// Overboard Libraries
#include <overboard/log/stdout_logger.hpp>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>
#include <lvgl/src/drivers/sdl/lv_sdl_mouse.h>
#include <lvgl/src/drivers/sdl/lv_sdl_window.h>

namespace ovb::hal::sdl {

/**
 * @brief PImpl (Pointer to Implementation) structure for Keyboard_Window
 */
struct Keyboard_Window::Impl {
    SDL_Window*   sdl_window = nullptr;
    lv_display_t* lv_display = nullptr;
    lv_obj_t*     screen     = nullptr;
};

/***************************/
/*        Constructor        */
/***************************/
Keyboard_Window::Keyboard_Window( const std::string& title, int width, int height )
    : m_impl(std::make_unique<Impl>())
{
    LOG_DEBUG( "Keyboard_Window: Creating SDL window: ", width, " x ", height );
    m_impl->lv_display = lv_sdl_window_create(width, height);
    if (!m_impl->lv_display) {
        throw std::runtime_error( "Keyboard_Window: lv_sdl_window_create failed" );
    }

    m_impl->sdl_window = lv_sdl_window_get_window(m_impl->lv_display);
    SDL_SetWindowTitle(m_impl->sdl_window, title.c_str());

    m_impl->screen = lv_display_get_screen_active(m_impl->lv_display);

    // Create a dedicated mouse indev for this window and bind it to its display
    lv_indev_t* mouse_indev = lv_sdl_mouse_create();
    lv_indev_set_display(mouse_indev, m_impl->lv_display);

    lv_timer_handler();
}

/******************************/
/*         Destructor         */
/******************************/
Keyboard_Window::~Keyboard_Window() = default;

/***********************************/
/*            Window ID            */
/***********************************/
uint32_t Keyboard_Window::window_id() const {
    return m_impl->sdl_window ? SDL_GetWindowID(m_impl->sdl_window) : 0;
}

/*************************/
/*         Screen        */
/*************************/
lv_obj_t* Keyboard_Window::screen() const {
    return m_impl->screen;
}

} // namespace ovb::hal::sdl
