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

// SDL event filter to intercept keyboard events
static int SDLCALL event_filter(void* userdata, SDL_Event* event) {
    auto* input = static_cast<SDL_Input*>(userdata);

    if (event->type == SDL_QUIT) {
        input->set_quit(true);
        return 0; // Don't pass to LVGL
    }

    // Handle keyboard events
    if (event->type == SDL_KEYDOWN && event->key.repeat == 0) {
        auto input_key = scancode_to_input_key(event->key.keysym.scancode);
        LOG_DEBUG("KEYDOWN: scancode=", std::to_string(event->key.keysym.scancode),
                  ", input_key=", input_key_to_string(input_key));

        // Track modifier state
        if (input_key == core::Input_Key::SHIFT) {
            input->set_shift_pressed(true);
        } else if (input_key == core::Input_Key::CONTROL) {
            input->set_control_pressed(true);
        } else if (input_key == core::Input_Key::ALT) {
            input->set_alt_pressed(true);
        }

        // Handle Command-Q on macOS for quit
#ifdef __APPLE__
        if (event->key.keysym.sym == SDLK_q && (event->key.keysym.mod & KMOD_GUI)) {
            input->set_quit(true);
            return 0;
        }
#else
        // Handle Ctrl-C on Linux/Windows for quit
        if (event->key.keysym.sym == SDLK_c && (event->key.keysym.mod & KMOD_CTRL)) {
            input->set_quit(true);
            return 0;
        }
#endif
        auto key_idx = input->keymap().get_key_index(input_key);
        if (key_idx.has_value()) {
            LOG_DEBUG("  -> Mapped to key_index=", std::to_string(key_idx.value()));
            input->push_event({Key_Event_Kind::Action, key_idx.value(), 0, Key_Event_Type::Press});
            input->suppress_next_text();
            return 0; // Don't pass to LVGL, we handled it
        } else {
            LOG_DEBUG("  -> NOT MAPPED, will emit Text via SDL_TEXTINPUT");
            return 1; // Let SDL_TEXTINPUT fire for unmapped keys
        }
    } else if (event->type == SDL_KEYUP) {
        auto input_key = scancode_to_input_key(event->key.keysym.scancode);

        // Track modifier state
        if (input_key == core::Input_Key::SHIFT) {
            input->set_shift_pressed(false);
        } else if (input_key == core::Input_Key::CONTROL) {
            input->set_control_pressed(false);
        } else if (input_key == core::Input_Key::ALT) {
            input->set_alt_pressed(false);
        }

        auto key_idx = input->keymap().get_key_index(input_key);
        if (key_idx.has_value()) {
            input->push_event({Key_Event_Kind::Action, key_idx.value(), 0, Key_Event_Type::Release});
            return 0; // Don't pass to LVGL, we handled it
        }
        return 1; // Pass to LVGL

    }

    // Text
    else if (event->type == SDL_TEXTINPUT) {
        // SDL_TEXTINPUT gives us Shift-resolved UTF-8 — convert to UTF-32
        // and emit a Text event for unmapped keys.
        if (input->consume_text_suppression()) {
            return 0;
        }
        const char* utf8 = event->text.text;
        if (utf8[0] != '\0') {
            // Decode first codepoint from UTF-8
            char32_t cp = 0;
            const auto b0 = static_cast<uint8_t>(utf8[0]);
            if (b0 < 0x80) {
                cp = b0;
            } else if ((b0 & 0xE0) == 0xC0) {
                cp = static_cast<char32_t>(b0 & 0x1F) << 6 |
                     static_cast<char32_t>(static_cast<uint8_t>(utf8[1]) & 0x3F);
            } else if ((b0 & 0xF0) == 0xE0) {
                cp = static_cast<char32_t>(b0 & 0x0F) << 12 |
                     static_cast<char32_t>(static_cast<uint8_t>(utf8[1]) & 0x3F) << 6 |
                     static_cast<char32_t>(static_cast<uint8_t>(utf8[2]) & 0x3F);
            } else {
                cp = static_cast<char32_t>(b0 & 0x07) << 18 |
                     static_cast<char32_t>(static_cast<uint8_t>(utf8[1]) & 0x3F) << 12 |
                     static_cast<char32_t>(static_cast<uint8_t>(utf8[2]) & 0x3F) << 6 |
                     static_cast<char32_t>(static_cast<uint8_t>(utf8[3]) & 0x3F);
            }
            LOG_DEBUG("SDL_TEXTINPUT: codepoint=", std::to_string(static_cast<uint32_t>(cp)));
            input->push_event({Key_Event_Kind::Text, -1, cp, Key_Event_Type::Press});
        }
        return 0;
    }

    // Pass all other events (mouse, window, etc.) to LVGL
    return 1;
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

    // Register event filter on first call
    static bool filter_registered = false;
    if (!filter_registered) {
        SDL_SetEventFilter(event_filter, this);
        filter_registered = true;
    }

    // Pump SDL events - the event filter intercepts keyboard events
    // and lets mouse/window events pass through to LVGL's event watcher
    SDL_PumpEvents();
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
