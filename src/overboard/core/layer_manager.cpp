/**
 * @file layer_manager.cpp
 * @author Marvin Smith
 * @date 2026-05-21
 * @brief Layer_Manager implementation
 */
#include <overboard/core/layer_manager.hpp>

namespace ovb::core {

/*****************************************/
/*          Constructor                  */
/*****************************************/
Layer_Manager::Layer_Manager(const Keymap& keymap)
    : m_keymap(keymap), m_active_layer(0) {}

/*****************************************/
/*          Get Active Layer             */
/*****************************************/
int Layer_Manager::active_layer() const {
    return m_active_layer;
}

/*****************************************/
/*          Get Layer Count              */
/*****************************************/
int Layer_Manager::layer_count() const {
    return m_keymap.layer_count();
}

/*****************************************/
/*       Register Layer Change CB        */
/*****************************************/
void Layer_Manager::on_layer_change(Layer_Change_Cb cb) {
    m_layer_change_callbacks.push_back(std::move(cb));
}

/******************************************/
/*        Notify Layer Change             */
/******************************************/
void Layer_Manager::notify_layer_change() {
    for (const auto& cb : m_layer_change_callbacks) {
        cb(m_active_layer);
    }
}

/*****************************************/
/*          Next Layer                   */
/*****************************************/
void Layer_Manager::next_layer() {
    m_active_layer = (m_active_layer + 1) % m_keymap.layer_count();
    notify_layer_change();
}

/*****************************************/
/*          Previous Layer               */
/*****************************************/
void Layer_Manager::prev_layer() {
    const int count = m_keymap.layer_count();
    m_active_layer = (m_active_layer - 1 + count) % count;
    notify_layer_change();
}

/*****************************************/
/*          Set Active Layer             */
/*****************************************/
void Layer_Manager::set_layer(int index) {
    if (index >= 0 && index < m_keymap.layer_count()) {
        m_active_layer = index;
        notify_layer_change();
    }
}

/*****************************************/
/*          Get Current Layer            */
/*****************************************/
const Layer& Layer_Manager::current_layer() const {
    return m_keymap.get_layer(static_cast<std::size_t>(m_active_layer));
}

/*****************************************/
/*        Get Action at Key Index        */
/*****************************************/
Action_Code Layer_Manager::action_at(int key_index) const {
    return m_keymap.get_action(static_cast<std::size_t>(m_active_layer),
                               static_cast<std::size_t>(key_index));
}

} // namespace ovb::core
