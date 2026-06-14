/**
 * @file    app_registry.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Application registry and factory
 *
 * Manages registration and creation of user-facing applications.
 * Each app self-registers and the registry handles instantiation
 * and menu item generation.
 */
#pragma once

// C++ Standard Libraries
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Project Libraries
#include <overboard/gui/i_app.hpp>

namespace ovb::gui {

/// @brief Sentinel value indicating no mnemonic hotkey
inline constexpr char NO_HOTKEY { '\0' };

/**
 * @brief Menu item information for App_Menu
 */
struct Menu_Item {
    std::string         name;    ///< App name (owns the string)
    std::string         icon;    ///< LVGL symbol/icon (owns the string)
    char                hotkey = NO_HOTKEY;  // '\0' if no hotkey
    int                 priority;
    int                 index;   ///< Registration order index
};

/**
 * @brief Application registry and factory
 *
 * Uses builder lambdas to allow each app to have unique constructor signatures.
 * The builder captures app-specific dependencies and creates the app on demand.
 */
class App_Registry {
    public:

        /// @brief Builder function type - captures args and creates app
        using Builder_Fn = std::function<std::shared_ptr<I_App>()>;

        /**
         * @brief Register an application with its builder and metadata
         * @param name App name for menu
         * @param icon LVGL symbol for menu
         * @param priority Menu sort order (lower first)
         * @param hotkey Mnemonic hotkey character or NO_HOTKEY
         * @param builder Lambda that creates the app (captures dependencies)
         */
        void register_app( std::string_view name,
                           std::string_view icon,
                           int priority,
                           char hotkey,
                           std::function<std::shared_ptr<I_App>()> builder ) {
            m_entries.push_back({ std::move(builder),
                                  std::string(name),
                                  std::string(icon),
                                  priority,
                                  hotkey });
        }

        /**
         * @brief Create all registered apps
         * @return Vector of created apps in registration order
         */
        std::vector<std::shared_ptr<I_App>> create_apps() const {
            std::vector<std::shared_ptr<I_App>> apps;
            apps.reserve(m_entries.size());

            for (const auto& entry : m_entries) {
                apps.push_back(entry.builder());
            }

            return apps;
        }

        /**
         * @brief Get menu items for App_Menu
         * @return Menu items sorted by priority
         */
        std::vector<Menu_Item> get_menu_items() const {
            std::vector<Menu_Item> items;
            items.reserve(m_entries.size());

            for (size_t i = 0; i < m_entries.size(); ++i) {
                const auto& e = m_entries[i];
                items.push_back({ e.name, e.icon, e.hotkey, e.priority, static_cast<int>(i) });
            }

            // Sort by priority
            std::sort(items.begin(), items.end(),
                      []( const auto& a, const auto& b ) { return a.priority < b.priority; });

            return items;
        }

        /**
         * @brief Get number of registered apps
         */
        size_t count() const { return m_entries.size(); }

        /**
         * @brief Clear all registrations
         */
        void clear() { m_entries.clear(); }

    private:

        /**
         * @brief Internal entry structure for registered applications
         */
        struct Entry {

            /// @brief Lambda that creates the app
            Builder_Fn        builder;
            /// @brief App name for menu
            std::string         name;
            /// @brief LVGL symbol
            std::string         icon;
            /// @brief Menu sort order
            int                 priority;
            /// @brief Mnemonic key
            char                hotkey;
        };

        /// @brief Vector of registered app entries
        std::vector<Entry> m_entries;

};// End of App_Registry class

} // namespace ovb::gui
