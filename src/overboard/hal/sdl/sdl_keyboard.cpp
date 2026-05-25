/**
 * @file    sdl_keyboard.cpp
 * @author  Marvin Smith
 * @date    2026-05-21
 *
 * @brief   SDL keyboard manager implementation
 */
#include <overboard/hal/sdl/sdl_keyboard.hpp>

// Project Libraries
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/sdl/sdl_keyboard_display.hpp>
#include <overboard/hal/sdl/sdl_keyboard_view.hpp>

namespace ovb::hal::sdl {

/*********************************/
/*          Constructor          */
/*********************************/
SDL_Keyboard::SDL_Keyboard( const std::string&            title,
                              int                           width,
                              int                           height,
                              const ovb::core::Grid_Layout& layout,
                              const core::Layer_Manager&    layers )
{
    m_display = std::make_unique<SDL_Keyboard_Display>(title, width, height);
    m_view    = std::make_unique<SDL_Keyboard_View>(
        m_display->screen(), layout, layers, width, height);
}

/*********************************/
/*          Getters              */
/*********************************/
int  SDL_Keyboard::width()  const { return m_display->width();  }
int  SDL_Keyboard::height() const { return m_display->height(); }

uint32_t SDL_Keyboard::window_id() const {
    return m_display->window_id();
}

/*********************************/
/*          Get Display Reference          */
/*********************************/
I_Display& SDL_Keyboard::get_display() {
    return *m_display;
}

/*********************************/
/*      Get Keyboard Display      */
/*********************************/
SDL_Keyboard_Display& SDL_Keyboard::get_keyboard_display() {
    return *m_display;
}

/*********************************/
/*          Layer Management     */
/*********************************/
void SDL_Keyboard::update_layer() {
    m_view->update_layer();
}

/*********************************/
/*          View Access          */
/*********************************/
SDL_Keyboard_View& SDL_Keyboard::view() {
    return *m_view;
}

/*********************************/
/*          Render               */
/*********************************/
void SDL_Keyboard::render() {
    m_display->flush();
}

} // namespace ovb::hal::sdl
