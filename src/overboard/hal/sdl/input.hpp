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

        bool poll(Key_Event& out_event) override;
        bool should_quit() const override;
        void pump() override;

    private:
        bool                    m_quit = false;
        std::queue<Key_Event>   m_event_queue;
        SDL_Keymap              m_keymap;
};

} // namespace ovb::hal::sdl
