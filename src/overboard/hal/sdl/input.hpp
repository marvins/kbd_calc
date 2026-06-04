#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <queue>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_input.hpp>
#include <overboard/hal/sdl/keymap.hpp>

namespace ovb::hal::sdl {

class SDL_Input : public ovb::hal::I_Input {
    public:
        /**
         * @brief Construct SDL input handler
         */
        SDL_Input() = default;

        /**
         * @brief Access the keyboard keymap for customization
         * @return Reference to the SDL keymap
         */
        SDL_Keymap& keymap() { return m_keymap; }

        /**
         * @brief Set quit flag (called from event filter)
         */
        void set_quit(bool quit) { m_quit = quit; }

        /**
         * @brief Push event to queue (called from event filter)
         */
        void push_event(const Key_Event& event) { m_event_queue.push(event); }

        /**
         * @brief Check if shift is currently pressed
         */
        bool is_shift_pressed() const { return m_shift_pressed; }

        /**
         * @brief Check if control is currently pressed
         */
        bool is_control_pressed() const { return m_control_pressed; }

        /**
         * @brief Check if alt is currently pressed
         */
        bool is_alt_pressed() const { return m_alt_pressed; }

        /**
         * @brief Set shift state (called from event filter)
         */
        void set_shift_pressed(bool pressed) { m_shift_pressed = pressed; }

        /**
         * @brief Set control state (called from event filter)
         */
        void set_control_pressed(bool pressed) { m_control_pressed = pressed; }

        /**
         * @brief Set alt state (called from event filter)
         */
        void set_alt_pressed(bool pressed) { m_alt_pressed = pressed; }

        bool poll(Key_Event& out_event) override;
        bool should_quit() const override;
        void pump() override;

    private:
        bool                    m_quit = false;
        bool                    m_shift_pressed = false;
        bool                    m_control_pressed = false;
        bool                    m_alt_pressed = false;
        std::queue<Key_Event>   m_event_queue;
        SDL_Keymap              m_keymap;
};

// Public function to set up signal handlers
void setup_signal_handlers();

} // namespace ovb::hal::sdl
