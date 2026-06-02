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

namespace ovb::hal {

enum class Key_Event_Type {
    Press,
    Release
};

/**
 * @brief Discriminates between the two event paths.
 *
 * - Action: a mapped physical key; key_index is valid, codepoint is unused.
 * - Text:   an unmapped key whose resolved character should be forwarded
 *           to the active panel as raw text; codepoint is valid (UTF-32),
 *           key_index is unused.
 */
enum class Key_Event_Kind {
    Action,
    Text
};

struct Key_Event {
    Key_Event_Kind kind      { Key_Event_Kind::Action };
    int            key_index { -1 };
    char32_t       codepoint { 0 };
    Key_Event_Type type      { Key_Event_Type::Press };
};

class I_Input {
    public:
        virtual ~I_Input() = default;

        virtual bool poll(Key_Event& out_event) = 0;
        virtual bool should_quit() const = 0;
        virtual void pump() = 0;
};

} // namespace ovb::hal
