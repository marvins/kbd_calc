/**
 * @file sdl_keymap.cpp
 * @author Marvin Smith
 * @date 2026-05-22
 * @brief SDL keyboard to calculator key mapping implementation
 */
#include <overboard/hal/sdl/sdl_keymap.hpp>

namespace ovb::hal::sdl {

SDL_Keymap::SDL_Keymap() {
    reset_to_defaults();
}

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

std::optional<int> SDL_Keymap::get_key_index_from_keycode(SDL_Keycode keycode) const {
    // Convert keycode to scancode for lookup
    SDL_Scancode scancode = SDL_GetScancodeFromKey(keycode);
    return get_key_index(scancode);
}

void SDL_Keymap::bind(SDL_Scancode scancode, int key_index) {
    if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
        m_scancode_map[scancode] = key_index;
    }
}

void SDL_Keymap::unbind(SDL_Scancode scancode) {
    if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
        m_scancode_map[scancode] = -1;
    }
}

void SDL_Keymap::reset_to_defaults() {
    // Initialize all to unmapped
    m_scancode_map.fill(-1);

    // Arrow keys -> Cursor keys (indices 28, 29, 27, 26 based on Basic layer layout)
    // From keymap.hpp Basic layer:
    //   Row 4: { KC::CURSOR_UP, ... }
    //   Row 5: { KC::CURSOR_LEFT, KC::CURSOR_DOWN, KC::CURSOR_RIGHT }
    // Indices: 27=CURSOR_UP, 28=CURSOR_LEFT, 29=CURSOR_DOWN, 26=CURSOR_RIGHT (checking layout...)
    //
    // Actually let me recalculate based on the grid layout in keymap.hpp:
    // Row 0 (7 keys): indices 0-6
    // Row 1 (7 keys): indices 7-13
    // Row 2 (6 keys): indices 14-19
    // Row 3 (4 keys): indices 20-23
    // Row 4 (3 keys): indices 24-26
    // Row 5 (3 keys): indices 27-29
    //
    // Basic layer mapping from keymap.hpp:
    // Row 4: { KC::CURSOR_UP, KL::CURSOR_UP } -> index 24
    // Row 5: { KC::CURSOR_LEFT, ... } -> index 27
    //        { KC::CURSOR_DOWN, ... } -> index 28
    //        { KC::CURSOR_RIGHT, ... } -> index 29
    // Wait, Row 5 has 3 keys: LEFT, DOWN, RIGHT -> indices 27, 28, 29
    // Row 4 has: UP, 0, DECIMAL -> indices 24, 25, 26

    m_scancode_map[SDL_SCANCODE_LEFT]  = 27;  // CURSOR_LEFT
    m_scancode_map[SDL_SCANCODE_RIGHT] = 29;  // CURSOR_RIGHT
    m_scancode_map[SDL_SCANCODE_UP]    = 24;  // CURSOR_UP
    m_scancode_map[SDL_SCANCODE_DOWN]  = 28;  // CURSOR_DOWN

    // qwe/asd/zxc -> Top-left 3x3 grid (indices 0-8)
    // Row 0: indices 0-6
    // Row 1: indices 7-13
    // Row 2: indices 14-19 (only 6 keys, but we want first 3)
    //
    // First 3 columns of first 3 rows:
    // Row 0: q, w, e -> indices 0, 1, 2
    // Row 1: a, s, d -> indices 7, 8, 9
    // Row 2: z, x, c -> indices 14, 15, 16

    // Top row
    m_scancode_map[SDL_SCANCODE_Q] = 0;
    m_scancode_map[SDL_SCANCODE_W] = 1;
    m_scancode_map[SDL_SCANCODE_E] = 2;

    // Middle row
    m_scancode_map[SDL_SCANCODE_A] = 7;
    m_scancode_map[SDL_SCANCODE_S] = 8;
    m_scancode_map[SDL_SCANCODE_D] = 9;

    // Bottom row
    m_scancode_map[SDL_SCANCODE_Z] = 14;
    m_scancode_map[SDL_SCANCODE_X] = 15;
    m_scancode_map[SDL_SCANCODE_C] = 16;

    // Number keys 0-9 mapped to calculator digits
    // From Basic layer:
    //   Index 25: DIGIT_0
    //   Index 20: DIGIT_1
    //   Index 21: DIGIT_2
    //   Index 22: DIGIT_3
    //   Index 17: DIGIT_4
    //   Index 18: DIGIT_5
    //   Index 19: DIGIT_6
    //   Index 10: DIGIT_7
    //   Index 11: DIGIT_8
    //   Index 12: DIGIT_9

    m_scancode_map[SDL_SCANCODE_0] = 25;
    m_scancode_map[SDL_SCANCODE_1] = 20;
    m_scancode_map[SDL_SCANCODE_2] = 21;
    m_scancode_map[SDL_SCANCODE_3] = 22;
    m_scancode_map[SDL_SCANCODE_4] = 17;
    m_scancode_map[SDL_SCANCODE_5] = 18;
    m_scancode_map[SDL_SCANCODE_6] = 19;
    m_scancode_map[SDL_SCANCODE_7] = 10;
    m_scancode_map[SDL_SCANCODE_8] = 11;
    m_scancode_map[SDL_SCANCODE_9] = 12;

    // Additional useful mappings
    m_scancode_map[SDL_SCANCODE_RETURN] = 23;    // EQUALS (index 23)
    m_scancode_map[SDL_SCANCODE_KP_ENTER] = 23;   // EQUALS
    m_scancode_map[SDL_SCANCODE_BACKSPACE] = 2;   // BACKSPACE (index 2)
    m_scancode_map[SDL_SCANCODE_DELETE] = 4;      // ALL_CLEAR at index 4
    m_scancode_map[SDL_SCANCODE_ESCAPE] = 4;      // ALL_CLEAR
    m_scancode_map[SDL_SCANCODE_PERIOD] = 26;     // DECIMAL
    m_scancode_map[SDL_SCANCODE_KP_PERIOD] = 26;    // DECIMAL

    // Operators (note: PLUS and ASTERISK require shift modifier, use numpad versions)
    m_scancode_map[SDL_SCANCODE_EQUALS] = 13;      // APPROX/+ at index 13 (requires shift for +)
    m_scancode_map[SDL_SCANCODE_KP_PLUS] = 13;
    m_scancode_map[SDL_SCANCODE_MINUS] = 6;       // NEGATE/- at index 6
    m_scancode_map[SDL_SCANCODE_KP_MINUS] = 6;
    m_scancode_map[SDL_SCANCODE_8] = 5;           // MULTIPLY placeholder (requires shift for *)
    m_scancode_map[SDL_SCANCODE_KP_MULTIPLY] = 5;
    m_scancode_map[SDL_SCANCODE_SLASH] = 3;       // DIVIDE placeholder
    m_scancode_map[SDL_SCANCODE_KP_DIVIDE] = 3;
}

} // namespace ovb::hal::sdl
