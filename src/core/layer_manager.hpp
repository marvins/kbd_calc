#pragma once

#include "keymap.hpp"

class Layer_Manager {
    public:
        explicit Layer_Manager(const Keymap& keymap);

        int            active_layer() const;
        void           next_layer();
        void           prev_layer();
        void           set_layer(int index);

        const Layer&   current_layer() const;
        const Key_Def& key_at(int key_index) const;

    private:
        const Keymap& m_keymap;
        int           m_active_layer;
};
