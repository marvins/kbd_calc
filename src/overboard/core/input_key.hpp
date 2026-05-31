/**
 * @file    input_key.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Physical input key codes
 *
 * Defines all physical keys that can be pressed on the keyboard.
 * These represent the actual hardware inputs, not what they do.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <string>

namespace ovb::core {

/**
 * @brief Physical input key enumeration
 *
 * Represents actual keys on a standard keyboard (Qt-style).
 * These are physical inputs, not calculator actions.
 */
enum class Input_Key : uint16_t {
    NONE = 0,

    // Escape and special keys
    ESCAPE,
    TAB,
    BACKSPACE,
    RETURN,
    ENTER = RETURN,
    SPACE,

    // Modifiers
    SHIFT,
    CONTROL,
    ALT,
    META,

    // Navigation
    LEFT,
    RIGHT,
    UP,
    DOWN,
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    INSERT,
    DELETE,

    // Function keys
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // Digits (top row)
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    // Letters
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    // Numpad
    NUMPAD_0,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    NUMPAD_ADD,
    NUMPAD_SUBTRACT,
    NUMPAD_MULTIPLY,
    NUMPAD_DIVIDE,
    NUMPAD_DECIMAL,
    NUMPAD_ENTER,

    // Symbols (main keyboard)
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    PERIOD,
    COMMA,
    SEMICOLON,
    COLON,
    EXCLAMATION,
    QUESTION,
    QUOTE,
    DOUBLE_QUOTE,
    APOSTROPHE,
    GRAVE,
    TILDE,
    BACKSLASH,
    BAR,
    BRACKET_LEFT,
    BRACKET_RIGHT,
    BRACE_LEFT,
    BRACE_RIGHT,
    PAREN_LEFT,
    PAREN_RIGHT,
    LESS,
    GREATER,
    EQUAL,
    PERCENT,
    DOLLAR,
    HASH,
    AT,
    CARET,
    AMPERSAND,
    UNDERSCORE,

    // Calculator-specific physical keys (if present)
    CALC_CLEAR,        // Dedicated C/CE key
    CALC_ENTER,      // Dedicated =/Enter key
};

/**
 * @brief Convert string to input key
 * @param str String representation
 * @return Input key value, or Input_Key::NONE if not found
 */
Input_Key string_to_input_key(const std::string& str);

/**
 * @brief Convert input key to string for testing/debugging
 * @param key Input key
 * @return String representation (e.g., "ESCAPE", "KEY_A", "F1")
 */
std::string input_key_to_string(Input_Key key);

} // namespace ovb::core

