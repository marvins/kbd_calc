/**
 * @file    settings_manager.hpp
 * @author  Marvin Smith
 * @date    2026-06-24
 *
 * @brief   Application settings manager with lifecycle and observer support
 *
 * Wraps Settings_Tree and I_Settings_Store to provide:
 * - Single load at startup (no repeated disk I/O)
 * - Dirty tracking for unsaved changes
 * - Observer pattern for change notifications
 * - Dependency injection (no singletons)
 * - Flexible key/value API
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/hal/i_settings_store.hpp>
#include <overboard/hal/settings_tree.hpp>

namespace ovb::core {

/**
 * @brief Application settings manager
 *
 * Manages the lifecycle of application settings:
 * - Loads settings once at startup
 * - Provides key/value access via Settings_Tree API
 * - Tracks dirty state for unsaved changes
 * - Notifies observers when settings change
 * - Saves settings on demand
 *
 * Designed for dependency injection - pass as shared_ptr to components.
 */
class Settings_Manager {

    public:

        /**
         * @brief Construct and load settings from store
         * @param store Platform-specific settings store (takes ownership)
         */
        explicit Settings_Manager(std::unique_ptr<hal::I_Settings_Store> store);

        /**
         * @brief Destructor
         */
        ~Settings_Manager() = default;

        // Non-copyable, movable
        Settings_Manager(const Settings_Manager&) = delete;
        Settings_Manager& operator=(const Settings_Manager&) = delete;
        Settings_Manager(Settings_Manager&&) = default;
        Settings_Manager& operator=(Settings_Manager&&) = default;

        /**
         * @brief Reload settings from disk
         * @return true if reload succeeded
         */
        bool reload();

        /**
         * @brief Save settings to disk
         * @return true if save succeeded
         */
        bool save();

        /**
         * @brief Check if settings have unsaved changes
         */
        bool is_dirty() const { return m_dirty; }

        // ═══════════════════════════════════════════════════════════
        // Key/Value API - delegates to Settings_Tree
        // ═══════════════════════════════════════════════════════════

        /**
         * @brief Get value at path, return nullopt if not found
         * @param path Dot-notation path (e.g., "status.clock_update_ms")
         */
        template<typename T>
        std::optional<T> get(const std::string& path) const {
            return m_tree.get<T>(path);
        }

        /**
         * @brief Get value at path, return default if not found
         * @param path Dot-notation path
         * @param default_val Value to return if path not found
         */
        template<typename T>
        T get(const std::string& path, T default_val) const {
            return m_tree.get<T>(path, default_val);
        }

        /**
         * @brief Set value at path, mark dirty, notify observers
         * @param path Dot-notation path
         * @param value Value to set
         */
        template<typename T>
        void set(const std::string& path, T value) {
            m_tree.set(path, value);
            m_dirty = true;
            notify_observers(path);
        }

        /**
         * @brief Check if path exists
         */
        bool has(const std::string& path) const {
            return m_tree.has(path);
        }

        /**
         * @brief Remove value at path
         */
        void remove(const std::string& path) {
            m_tree.remove(path);
            m_dirty = true;
            notify_observers(path);
        }

        /**
         * @brief Access underlying tree (for advanced use)
         */
        const hal::Settings_Tree& tree() const { return m_tree; }

        // ═══════════════════════════════════════════════════════════
        // Observer Pattern - notify on changes
        // ═══════════════════════════════════════════════════════════

        /**
         * @brief Callback signature: void(const std::string& changed_path)
         */
        using Change_Callback = std::function<void(const std::string& path)>;

        /**
         * @brief Register observer for all changes
         * @param cb Callback invoked when any setting changes
         * @return Observer ID for later removal
         */
        size_t add_observer(Change_Callback cb);

        /**
         * @brief Register observer for specific path prefix
         * @param path_prefix Only notify for changes under this path (e.g., "status.")
         * @param cb Callback invoked when matching setting changes
         * @return Observer ID for later removal
         */
        size_t add_observer(const std::string& path_prefix, Change_Callback cb);

        /**
         * @brief Remove observer by ID
         */
        void remove_observer(size_t observer_id);

    private:

        struct Observer {
            size_t          id;
            std::string     path_prefix;  // Empty = observe all
            Change_Callback callback;
        };

        std::unique_ptr<hal::I_Settings_Store> m_store;
        hal::Settings_Tree                     m_tree;
        bool                                   m_dirty { false };

        std::vector<Observer> m_observers;
        size_t                m_next_observer_id { 0 };

        void notify_observers(const std::string& changed_path);
};

} // namespace ovb::core
