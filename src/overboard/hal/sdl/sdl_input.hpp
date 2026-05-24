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
#include <overboard/hal/sdl/sdl_keymap.hpp>

namespace ovb::hal::sdl {

class SDL_Input : public ovb::hal::I_Input {
    public:
        /**
         * @brief Construct SDL input handler
         * @param kbd_window_id SDL window ID to filter events to
         * @param kbd_width     Keyboard window pixel width
         * @param kbd_height    Keyboard window pixel height
         * @param layout        Grid layout for hit testing
         * @param header_height Header bar height in pixels
         * @param margin_left   Left margin in pixels
         * @param margin_top    Top margin in pixels
         */
        SDL_Input(uint32_t                      kbd_window_id,
                  int                           kbd_width,
                  int                           kbd_height,
                  const ovb::core::Grid_Layout& layout,
                  int header_height, int margin_left, int margin_top);

        /**
         * @brief Access the keyboard keymap for customization
         * @return Reference to the SDL keymap
         */
        SDL_Keymap& keymap() { return m_keymap; }

        bool poll(Key_Event& out_event) override;
        bool should_quit() const override;
        void pump() override;

        int  hit_test(int mouse_x, int mouse_y) const;

    private:
        uint32_t                        m_kbd_window_id;
        int                             m_kbd_width;
        int                             m_kbd_height;
        const ovb::core::Grid_Layout&   m_layout;
        int                             m_header_height;
        int                             m_margin_left;
        int                             m_margin_top;
        bool                            m_quit = false;
        std::queue<Key_Event>           m_event_queue;
        SDL_Keymap                      m_keymap;

        static constexpr int KEY_PAD = 4;
};

} // namespace ovb::hal::sdl
