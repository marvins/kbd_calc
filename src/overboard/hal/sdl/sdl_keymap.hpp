/**
 * @file sdl_keymap.hpp
 * @author Marvin Smith
 * @date 2026-05-22
 * @brief SDL keyboard to calculator key mapping
 *
 * Maps physical SDL keyboard keys to calculator key indices.
 * This is SDL simulator-specific and separate from the RP2350 HAL.
 *
 * Default mapping:
 *   - Arrow keys: CURSOR_LEFT/RIGHT/UP/DOWN
 *   - qwe/asd/zxc: Top-left 3x3 grid of calculator keys
 *
 * Future: Load from config file for customization.
 */
#pragma once

// C++ Standard Libraries
#include <array>
#include <cstdint>
#include <optional>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop

namespace ovb::hal::sdl {

/**
 * @brief SDL to calculator key mapping
 *
 * Maps SDL scancodes to calculator key indices (0-29).
 * -1 indicates no mapping.
 */
class SDL_Keymap {
    public:
        /// Number of mappable keys in calculator grid
        static constexpr int KEY_COUNT = 30;

        /// Default constructor sets up standard mapping
        SDL_Keymap();

        /**
         * @brief Get calculator key index for SDL scancode
         * @param scancode SDL scancode
         * @return Key index (0-29) or std::nullopt if not mapped
         */
        std::optional<int> get_key_index(SDL_Scancode scancode) const;

        /**
         * @brief Get calculator key index for SDL keycode
         * @param keycode SDL keycode
         * @return Key index (0-29) or std::nullopt if not mapped
         */
        std::optional<int> get_key_index_from_keycode(SDL_Keycode keycode) const;

        /**
         * @brief Bind a scancode to a calculator key index
         * @param scancode SDL scancode
         * @param key_index Calculator key index (0-29), or -1 to unbind
         */
        void bind(SDL_Scancode scancode, int key_index);

        /**
         * @brief Unbind a scancode
         * @param scancode SDL scancode to unbind
         */
        void unbind(SDL_Scancode scancode);

        /**
         * @brief Reset to default mapping
         *
         * Default layout:
         *   Arrows: Cursor keys
         *   qwe/asd/zxc: Top-left 3x3 (keys 0-8)
         *   0-9: Digit keys mapped to numpad positions
         */
        void reset_to_defaults();

    private:
        // Map SDL scancode to calculator key index
        std::array<int, SDL_NUM_SCANCODES> m_scancode_map;
};

} // namespace ovb::hal::sdl
