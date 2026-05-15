#pragma once

#include <cstdint>

enum class Key_Event_Type {
    Press,
    Release
};

struct Key_Event {
    int            key_index;
    Key_Event_Type type;
};

class I_Input {
    public:
        virtual ~I_Input() = default;

        virtual bool poll(Key_Event& out_event) = 0;
        virtual bool should_quit() const = 0;
        virtual void pump() = 0;
};
