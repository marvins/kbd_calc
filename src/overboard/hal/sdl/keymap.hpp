/**
 * @file sdl_keymap.hpp
 * @author Marvin Smith
 * @date 2026-05-22
 * @brief SDL keyboard to calculator key mapping
 *
 * Maps Input_Key codes (hardware-agnostic) to calculator key indices.
 * This is SDL simulator-specific and separate from the RP2350 HAL.
 *
 * The mapping is loaded from keymap.json which uses Input_Key names
 * (e.g., "KEY_1", "NUMPAD_7", "F1") for hardware portability.
 */
#pragma once

// C++ Standard Libraries
#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <string>

// Project Libraries
#include <overboard/core/input_key.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL to calculator key mapping
 *
 * Maps Input_Key codes to calculator key indices (0-29).
 * -1 indicates no mapping.
 */
class SDL_Keymap {
    public:
        /// Number of mappable keys in calculator grid
        static constexpr int KEY_COUNT = 40;

        /// Default constructor sets up standard mapping
        SDL_Keymap();

        /**
         * @brief Get calculator key index for Input_Key
         * @param input_key Hardware-agnostic input key
         * @return Key index (0-39) or std::nullopt if not mapped
         */
        std::optional<int> get_key_index(core::Input_Key input_key) const;

        /**
         * @brief Bind an Input_Key to a calculator key index
         * @param input_key Hardware-agnostic input key
         * @param key_index Calculator key index (0-39), or -1 to unbind
         */
        void bind(core::Input_Key input_key, int key_index);

        /**
         * @brief Unbind an Input_Key
         * @param input_key Input key to unbind
         */
        void unbind(core::Input_Key input_key);

        /**
         * @brief Clear all bindings (all keys unmapped)
         */
        void reset_to_defaults();

        /**
         * @brief Load bindings from a pre-built name->index map
         * @param input_key_map Map of Input_Key name strings to key indices
         *
         * Clears all existing bindings, then applies the provided map.
         * Unrecognised key name strings are silently ignored.
         */
        void load_from_map(const std::map<std::string, int>& input_key_map);

    private:
        // Map Input_Key to calculator key index
        std::array<int, static_cast<size_t>(core::Input_Key::CALC_ENTER) + 1> m_input_key_map;
};

} // namespace ovb::hal::sdl
