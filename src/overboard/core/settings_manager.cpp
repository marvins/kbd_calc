/**
 * @file    settings_manager.cpp
 * @author  Marvin Smith
 * @date    2026-06-24
 *
 * @brief   Settings_Manager implementation
 */
#include <overboard/core/settings_manager.hpp>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::core {

/*********************************/
/*         Constructor           */
/*********************************/
Settings_Manager::Settings_Manager(std::unique_ptr<hal::I_Settings_Store> store)
    : m_store(std::move(store)) {
    
    if (!m_store) {
        LOG_ERROR("Settings_Manager: null store provided");
        return;
    }

    // Load settings from store
    if (!m_store->load(m_tree)) {
        LOG_WARN("Settings_Manager: failed to load settings, using empty tree");
    }

    m_dirty = false;
}

/*********************************/
/*            Reload             */
/*********************************/
bool Settings_Manager::reload() {
    if (!m_store) {
        LOG_ERROR("Settings_Manager::reload: no store available");
        return false;
    }

    hal::Settings_Tree new_tree;
    if (!m_store->load(new_tree)) {
        LOG_ERROR("Settings_Manager::reload: failed to load settings");
        return false;
    }

    m_tree = std::move(new_tree);
    m_dirty = false;

    // Notify all observers that everything may have changed
    notify_observers("");

    LOG_DEBUG("Settings_Manager: reloaded settings from disk");
    return true;
}

/*********************************/
/*             Save              */
/*********************************/
bool Settings_Manager::save() {
    if (!m_store) {
        LOG_ERROR("Settings_Manager::save: no store available");
        return false;
    }

    if (!m_store->save(m_tree)) {
        LOG_ERROR("Settings_Manager::save: failed to save settings");
        return false;
    }

    m_dirty = false;
    LOG_DEBUG("Settings_Manager: saved settings to disk");
    return true;
}

/*********************************/
/*        Add Observer (all)     */
/*********************************/
size_t Settings_Manager::add_observer(Change_Callback cb) {
    return add_observer("", std::move(cb));
}

/*********************************/
/*    Add Observer (filtered)    */
/*********************************/
size_t Settings_Manager::add_observer(const std::string& path_prefix, Change_Callback cb) {
    const size_t id = m_next_observer_id++;
    m_observers.push_back(Observer{id, path_prefix, std::move(cb)});
    LOG_DEBUG("Settings_Manager: registered observer ", id, 
              path_prefix.empty() ? " (all)" : " for prefix: " + path_prefix);
    return id;
}

/*********************************/
/*       Remove Observer         */
/*********************************/
void Settings_Manager::remove_observer(size_t observer_id) {
    auto it = std::remove_if(m_observers.begin(), m_observers.end(),
        [observer_id](const Observer& obs) {
            return obs.id == observer_id;
        });
    
    if (it != m_observers.end()) {
        m_observers.erase(it, m_observers.end());
        LOG_DEBUG("Settings_Manager: removed observer ", observer_id);
    }
}

/*********************************/
/*      Notify Observers         */
/*********************************/
void Settings_Manager::notify_observers(const std::string& changed_path) {
    for (const auto& observer : m_observers) {
        // If observer has no prefix filter, or changed path starts with prefix
        if (observer.path_prefix.empty() || 
            changed_path.find(observer.path_prefix) == 0) {
            observer.callback(changed_path);
        }
    }
}

} // namespace ovb::core
