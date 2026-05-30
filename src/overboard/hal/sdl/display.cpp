/**
 * @file    display.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   SDL display implementation — single window with LCD and keyboard
 */
#include <overboard/hal/sdl/display.hpp>

// C++ Standard Libraries
#include <stdexcept>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>
#include <lvgl/src/drivers/sdl/lv_sdl_window.h>

// Project Libraries
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/sdl/lvgl_theme.hpp>

namespace ovb::hal::sdl {

/**
 * @brief PImpl (Pointer to Implementation) structure for Display
 *
 * Contains all LVGL handles and section managers. Using PImpl allows
 * changes to implementation without recompiling dependent code.
 */
struct Display::Impl {
    SDL_Window*   sdl_window = nullptr;
    lv_display_t* lv_display = nullptr;
};

/***************************/
/*        Constructor        */
/*****************************/
Display::Display( const std::string& title, int width, int height )
    : m_impl(std::make_unique<Impl>())
{
    m_impl->lv_display = lv_sdl_window_create(width, height);
    if (!m_impl->lv_display) {
        throw std::runtime_error("Display: lv_sdl_window_create failed");
    }
    lv_display_set_default(m_impl->lv_display);

    m_impl->sdl_window = lv_sdl_window_get_window(m_impl->lv_display);
    SDL_SetWindowTitle(m_impl->sdl_window, title.c_str());

    lv_sdl_mouse_create();
    lv_timer_handler();

    // Clear default screen styling
    lv_obj_t* scr = lv_screen_active();
    lv_obj_set_style_pad_all(scr, 0, 0);
    lv_obj_set_style_border_width(scr, 0, 0);
    lv_obj_set_style_bg_color(scr, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
}

/******************************/
/*         Destructor         */
/******************************/
Display::~Display() = default;

/***********************************/
/*            Window ID            */
/***********************************/
uint32_t Display::window_id() const {
    return m_impl->sdl_window ? SDL_GetWindowID(m_impl->sdl_window) : 0;
}

/*************************/
/*         Screen        */
/*************************/
lv_obj_t* Display::screen() const {
    return lv_screen_active();
}

} // namespace ovb::hal::sdl
