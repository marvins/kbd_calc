/**
 * @file     keymap.cpp
 * @author   Marvin Smith
 * @date     2026-05-22
 * @brief    SDL keyboard to calculator key mapping implementation
 */
#include <overboard/hal/sdl/keymap.hpp>

namespace ovb::hal::sdl {

/********************************/
/*          Constructor         */
/********************************/
SDL_Keymap::SDL_Keymap() {
    m_scancode_map.fill(-1);
}

/********************************/
/*       Public Methods         */
/********************************/
std::optional<int> SDL_Keymap::get_key_index(SDL_Scancode scancode) const {
    if (scancode < 0 || scancode >= SDL_NUM_SCANCODES) {
        return std::nullopt;
    }
    int idx = m_scancode_map[scancode];
    if (idx < 0 || idx >= KEY_COUNT) {
        return std::nullopt;
    }
    return idx;
}

/********************************/
/*    Keycode -> Key Index      */
/********************************/
std::optional<int> SDL_Keymap::get_key_index_from_keycode(SDL_Keycode keycode) const {
    // Convert keycode to scancode for lookup
    SDL_Scancode scancode = SDL_GetScancodeFromKey(keycode);
    return get_key_index(scancode);
}

/********************************/
/*         Bind Scancode        */
/********************************/
void SDL_Keymap::bind(SDL_Scancode scancode, int key_index) {
    if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
        m_scancode_map[scancode] = key_index;
    }
}

/********************************/
/*        Unbind Scancode       */
/********************************/
void SDL_Keymap::unbind(SDL_Scancode scancode) {
    if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
        m_scancode_map[scancode] = -1;
    }
}

/********************************/
/*      Reset to Defaults       */
/********************************/
void SDL_Keymap::reset_to_defaults() {
    m_scancode_map.fill(-1);
}

/********************************/
/*          Load Map            */
/********************************/
void SDL_Keymap::load_from_map(const std::map<std::string, int>& scancode_map) {
    m_scancode_map.fill(-1);
    for (const auto& [name, key_index] : scancode_map) {
        SDL_Scancode sc = SDL_GetScancodeFromName(name.c_str() + 14); // strip "SDL_SCANCODE_"
        if (sc != SDL_SCANCODE_UNKNOWN) {
            bind(sc, key_index);
        }
    }
}

} // namespace ovb::hal::sdl
