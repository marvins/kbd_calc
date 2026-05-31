/**
 * @file    sdl_input.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   SDL input handler for keyboard and mouse events
 */
#include <overboard/hal/sdl/input.hpp>

// C++ Standard Libraries
#include <atomic>
#include <csignal>
#include <cstring>

// Project Libraries
#include <overboard/hal/sdl/input_key_mapping.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

// Static flag for signal handling (Ctrl-C / SIGINT)
static std::atomic<bool> g_signal_quit{false};

static void signal_handler(int /*sig*/) {
    g_signal_quit.store(true, std::memory_order_relaxed);
}

// Public function to set up signal handlers
void setup_signal_handlers() {
    static bool initialized = false;
    if (!initialized) {
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0; // Don't use SA_RESTART to allow interruption

        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGTERM, &sa, nullptr);
        initialized = true;
    }
}

/********************************************/
/*          Check if We Should Quit         */
/********************************************/
bool SDL_Input::should_quit() const {
    // Check both SDL quit flag and signal flag
    return m_quit || g_signal_quit.load(std::memory_order_relaxed);
}

/*****************************************/
/*         Pump SDL Events               */
/*****************************************/
void SDL_Input::pump() {
    // Set up signal handlers on first call
    setup_signal_handlers();

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            m_quit = true;
            return;
        }

        // Handle keyboard events (global - not tied to a specific window)
        if (ev.type == SDL_KEYDOWN && ev.key.repeat == 0) {
            auto input_key = scancode_to_input_key(ev.key.keysym.scancode);
            LOG_DEBUG("KEYDOWN: scancode=", std::to_string(ev.key.keysym.scancode),
                      ", input_key=", input_key_to_string(input_key));
            // Handle Command-Q on macOS for quit
#ifdef __APPLE__
            if (ev.key.keysym.sym == SDLK_q && (ev.key.keysym.mod & KMOD_GUI)) {
                m_quit = true;
                return;
            }
#else
            // Handle Ctrl-C on Linux/Windows for quit
            if (ev.key.keysym.sym == SDLK_c && (ev.key.keysym.mod & KMOD_CTRL)) {
                m_quit = true;
                return;
            }
#endif
            auto key_idx = m_keymap.get_key_index(input_key);
            if (key_idx.has_value()) {
                LOG_DEBUG("  -> Mapped to key_index=", std::to_string(key_idx.value()));
                m_event_queue.push({key_idx.value(), Key_Event_Type::Press});
            } else {
                LOG_DEBUG("  -> NOT MAPPED");
                // Not a mapped key - push back to queue for LVGL to handle
                SDL_PushEvent(&ev);
            }
        } else if (ev.type == SDL_KEYUP) {
            auto input_key = scancode_to_input_key(ev.key.keysym.scancode);
            auto key_idx = m_keymap.get_key_index(input_key);
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
