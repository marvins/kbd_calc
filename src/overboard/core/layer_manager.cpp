#include <overboard/core/layer_manager.hpp>

Layer_Manager::Layer_Manager(const Keymap& keymap)
    : m_keymap(keymap), m_active_layer(0) {}

int Layer_Manager::active_layer() const {
    return m_active_layer;
}

void Layer_Manager::next_layer() {
    m_active_layer = (m_active_layer + 1) % m_keymap.layer_count();
}

void Layer_Manager::prev_layer() {
    const int count = m_keymap.layer_count();
    m_active_layer = (m_active_layer - 1 + count) % count;
}

void Layer_Manager::set_layer(int index) {
    if (index >= 0 && index < m_keymap.layer_count())
        m_active_layer = index;
}

const Layer& Layer_Manager::current_layer() const {
    return m_keymap.get_layer(static_cast<std::size_t>(m_active_layer));
}

const Key_Def& Layer_Manager::key_at(int key_index) const {
    return m_keymap.get_key(static_cast<std::size_t>(m_active_layer),
                            static_cast<std::size_t>(key_index));
}
