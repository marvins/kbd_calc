/**
 * @file    panel_manager.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Panel_Manager implementation
 */
#include <overboard/gui/panel_manager.hpp>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*          Constructor        */
/*******************************/
Panel_Manager::Panel_Manager(lv_obj_t* parent)
    : m_parent(parent) {}

/*******************************/
/*       Register Panel        */
/*******************************/
int Panel_Manager::register_panel(std::unique_ptr<I_Panel> panel) {
    const int index = static_cast<int>(m_panels.size());
    LOG_DEBUG("Panel_Manager: registering panel '", panel->name(), "' at index ", std::to_string(index));
    m_panels.push_back(std::move(panel));
    return index;
}

/*******************************/
/*           Push              */
/*******************************/
void Panel_Manager::push(int index) {
    if (index < 0 || index >= static_cast<int>(m_panels.size())) {
        LOG_DEBUG("Panel_Manager::push: invalid index ", std::to_string(index));
        return;
    }

    if (!m_stack.empty()) {
        m_panels[static_cast<std::size_t>(m_stack.back())]->deactivate();
    }

    m_stack.push_back(index);
    LOG_DEBUG("Panel_Manager: activating panel '", m_panels[static_cast<std::size_t>(index)]->name(), "'");
    m_panels[static_cast<std::size_t>(index)]->activate(m_parent);
}

/*******************************/
/*           Pop               */
/*******************************/
void Panel_Manager::pop() {
    if (m_stack.size() <= 1) return;

    m_panels[static_cast<std::size_t>(m_stack.back())]->deactivate();
    m_stack.pop_back();

    const int top = m_stack.back();
    LOG_DEBUG("Panel_Manager: restoring panel '", m_panels[static_cast<std::size_t>(top)]->name(), "'");
    m_panels[static_cast<std::size_t>(top)]->activate(m_parent);
}

/*******************************/
/*        Handle Input         */
/*******************************/
bool Panel_Manager::handle_input(core::Action_Code action) {
    if (m_stack.empty()) return false;
    return m_panels[static_cast<std::size_t>(m_stack.back())]->handle_input(action);
}

/*******************************/
/*        Handle Text          */
/*******************************/
bool Panel_Manager::handle_text(char32_t codepoint) {
    if (m_stack.empty()) return false;
    return m_panels[static_cast<std::size_t>(m_stack.back())]->handle_text(codepoint);
}

/*******************************/
/*          Refresh            */
/*******************************/
void Panel_Manager::refresh() {
    if (m_stack.empty()) return;
    m_panels[static_cast<std::size_t>(m_stack.back())]->refresh();
}

/*******************************/
/*        Active Panel         */
/*******************************/
I_Panel* Panel_Manager::active_panel() {
    if (m_stack.empty()) return nullptr;
    return m_panels[static_cast<std::size_t>(m_stack.back())].get();
}

/*******************************/
/*        Panel Count          */
/*******************************/
int Panel_Manager::panel_count() const {
    return static_cast<int>(m_panels.size());
}

} // namespace ovb::gui
