/**
 * @file     keymap.cpp
 * @author   Marvin Smith
 * @date     2026-05-22
 * @brief    SDL keyboard to calculator key mapping implementation
 *
 * Uses hardware-agnostic Input_Key codes for portability across
 * different HAL implementations (SDL, PicoCalc, etc.).
 */
#include <overboard/hal/sdl/keymap.hpp>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

/********************************/
/*          Constructor         */
/********************************/
SDL_Keymap::SDL_Keymap() {
    m_input_key_map.fill(-1);
}

/********************************/
/*       Public Methods         */
/********************************/
std::optional<int> SDL_Keymap::get_key_index(core::Input_Key input_key) const {
    auto idx = static_cast<size_t>(input_key);
    if (idx >= m_input_key_map.size()) {
        return std::nullopt;
    }
    int key_idx = m_input_key_map[idx];
    if (key_idx < 0) {
        return std::nullopt;
    }
    return key_idx;
}

/********************************/
/*         Bind Input Key       */
/********************************/
void SDL_Keymap::bind(core::Input_Key input_key, int key_index) {
    auto idx = static_cast<size_t>(input_key);
    if (idx < m_input_key_map.size()) {
        m_input_key_map[idx] = key_index;
    }
}

/********************************/
/*        Unbind Input Key      */
/********************************/
void SDL_Keymap::unbind(core::Input_Key input_key) {
    auto idx = static_cast<size_t>(input_key);
    if (idx < m_input_key_map.size()) {
        m_input_key_map[idx] = -1;
    }
}

/********************************/
/*      Reset to Defaults       */
/********************************/
void SDL_Keymap::reset_to_defaults() {
    m_input_key_map.fill(-1);
}

/********************************/
/*          Load Map            */
/********************************/
void SDL_Keymap::load_from_map(const std::map<std::string, int>& input_key_map) {
    m_input_key_map.fill(-1);
    for (const auto& [name, key_index] : input_key_map) {
        auto input_key = core::string_to_input_key(name);
        if (input_key != core::Input_Key::NONE) {
            bind(input_key, key_index);
            LOG_DEBUG("Loaded input_key: ", name, " -> key_index=", std::to_string(key_index));
        } else {
            LOG_ERROR("Failed to map input_key name: ", name);
        }
    }
    LOG_DEBUG("Total input_keys loaded: ", std::to_string(input_key_map.size()));
}

} // namespace ovb::hal::sdl
