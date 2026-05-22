/**
 * @file lvgl_keyboard_display.cpp
 * @author Marvin Smith
 * @date 2026-05-20
 * @brief LVGL window owner for the keyboard display
 */
#include <overboard/hal/sdl/lvgl_keyboard_display.hpp>

// C++ Standard Libraries
#include <stdexcept>

namespace ovb::hal::sdl {

/****************************/
/*  Constructor/Destructor  */
/****************************/

LVGL_Keyboard_Display::LVGL_Keyboard_Display(const std::string& title, int width, int height)
    : m_width(width), m_height(height)
{
    m_display = lv_sdl_window_create(width, height);
    if (!m_display) {
        throw std::runtime_error("LVGL_Keyboard_Display: lv_sdl_window_create failed");
    }
    lv_sdl_window_set_title(m_display, title.c_str());

    // Run the timer handler once so the SDL window is actually created
    // before we query its handle
    lv_timer_handler();

    auto* renderer = static_cast<SDL_Renderer*>(lv_sdl_window_get_renderer(m_display));
    m_sdl_window = renderer ? SDL_RenderGetWindow(renderer) : nullptr;

    // Clear default screen padding
    lv_display_set_default(m_display);
    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);
    lv_obj_set_style_border_width(lv_screen_active(), 0, 0);
    lv_obj_set_style_bg_color( lv_screen_active(),
                               lv_color_hex(0x1E1E1E),
                               LV_PART_MAIN );
}

/****************************/
/*     Accessors            */
/****************************/
int    LVGL_Keyboard_Display::width()     const { return m_width;  }
int    LVGL_Keyboard_Display::height()    const { return m_height; }

uint32_t LVGL_Keyboard_Display::window_id() const {
    return m_sdl_window ? SDL_GetWindowID(m_sdl_window) : 0;
}

lv_obj_t* LVGL_Keyboard_Display::screen() const {
    lv_display_set_default(m_display);
    return lv_screen_active();
}

/****************************/
/*    I_Display stubs       */
/****************************/

void LVGL_Keyboard_Display::clear(Color /*c*/)                                                           {}
void LVGL_Keyboard_Display::draw_pixel(ovb::core::Point<int> /*pos*/, Color /*c*/)                       {}
void LVGL_Keyboard_Display::draw_rect(ovb::core::Point<int> /*pos*/, ovb::core::Point<int> /*size*/,
                                       Color /*c*/, bool /*filled*/)                                      {}
void LVGL_Keyboard_Display::draw_text(ovb::core::Point<int> /*pos*/, const std::string& /*text*/,
                                       Color /*fg*/, Color /*bg*/, int /*scale*/)                         {}

void LVGL_Keyboard_Display::flush() {
    lv_timer_handler();
}

} // namespace ovb::hal::sdl
