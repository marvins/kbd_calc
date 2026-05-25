/**
 * @file    sdl_input.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   SDL input handler for keyboard and mouse events
 */
#include <overboard/hal/sdl/sdl_input.hpp>

namespace ovb::hal::sdl {

bool SDL_Input::should_quit() const { return m_quit; }

/*****************************************/
/*         Pump SDL Events               */
/*****************************************/
void SDL_Input::pump() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            m_quit = true;
            return;
        }

        // Handle keyboard events (global - not tied to a specific window)
        if (ev.type == SDL_KEYDOWN && ev.key.repeat == 0) {
            // Handle Command-Q on macOS for quit
#ifdef __APPLE__
            if (ev.key.keysym.sym == SDLK_q && (ev.key.keysym.mod & KMOD_GUI)) {
                m_quit = true;
                return;
            }
#endif
            auto key_idx = m_keymap.get_key_index(ev.key.keysym.scancode);
            if (key_idx.has_value()) {
                m_event_queue.push({key_idx.value(), Key_Event_Type::Press});
            } else {
                // Not a mapped key - push back to queue for LVGL to handle
                SDL_PushEvent(&ev);
            }
        } else if (ev.type == SDL_KEYUP) {
            auto key_idx = m_keymap.get_key_index(ev.key.keysym.scancode);
            if (key_idx.has_value()) {
                m_event_queue.push({key_idx.value(), Key_Event_Type::Release});
            } else {
                // Not a mapped key - push back to queue for LVGL to handle
                SDL_PushEvent(&ev);
            }
        } else {
            // Non-keyboard event (mouse, window, etc.) - push back for LVGL
            SDL_PushEvent(&ev);
        }
    }
}

/*****************************************/
/*         Poll the keyboard events      */
/*****************************************/
bool SDL_Input::poll(Key_Event& out_event) {
    if (m_event_queue.empty()) return false;
    out_event = m_event_queue.front();
    m_event_queue.pop();
    return true;
}

} // namespace ovb::hal::sdl
