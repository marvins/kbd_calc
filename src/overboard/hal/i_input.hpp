/**
 * @file    i_input.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Abstract input interface and key event types
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>

// Project Libraries
#include <overboard/core/input_key.hpp>

namespace ovb::hal {

enum class Key_Event_Type {
    Press,
    Release
};

/**
 * @brief Discriminates between the three event paths.
 *
 * - Action: a mapped physical key via keyboard.json; key_index is valid.
 * - Direct_Action: a standard keyboard key with direct Input_Key->Action_Code mapping; input_key is valid.
 * - Text:   an unmapped key whose resolved character should be forwarded
 *           to the active panel as raw text; codepoint is valid (UTF-32).
 */
enum class Key_Event_Kind {
    Action,
    Direct_Action,
    Text
};

struct Key_Event {
    Key_Event_Kind kind      { Key_Event_Kind::Action };
    int            key_index { -1 };
    char32_t       codepoint { 0 };
    Key_Event_Type type      { Key_Event_Type::Press };
    core::Input_Key input_key { core::Input_Key::NONE };
};

class I_Input {
    public:
        virtual ~I_Input() = default;

        virtual bool poll(Key_Event& out_event) = 0;
        virtual bool should_quit() const = 0;
        virtual void pump() = 0;
};

} // namespace ovb::hal
