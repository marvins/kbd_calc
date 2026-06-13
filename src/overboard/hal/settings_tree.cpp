/**
 * @file    settings_tree.cpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Settings_Tree implementation
 */
#include "settings_tree.hpp"

// C++ Standard Libraries
#include <sstream>
#include <string_view>

namespace ovb::hal {

/****************************/
/*     Constructor          */
/****************************/
Settings_Tree::Settings_Tree(toml::table table)
    : m_data(std::move(table)) {
}

/****************************/
/*     Get (optional)       */
/****************************/
template<typename T>
std::optional<T> Settings_Tree::get(const std::string& path) const {
    const auto* node = navigate(path);
    if (!node) {
        return std::nullopt;
    }

    // Try to get as the requested type
    if constexpr (std::is_same_v<T, bool>) {
        if (auto val = node->value<bool>()) {
            return *val;
        }
    } else if constexpr (std::is_same_v<T, int>) {
        if (auto val = node->value<int64_t>()) {
            return static_cast<int>(*val);
        }
    } else if constexpr (std::is_same_v<T, float>) {
        if (auto val = node->value<double>()) {
            return static_cast<float>(*val);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (auto val = node->value<std::string>()) {
            return *val;
        }
    } else if constexpr (std::is_same_v<T, std::filesystem::path>) {
        if (auto val = node->value<std::string>()) {
            return std::filesystem::path(*val);
        }
    }

    return std::nullopt;
}

// Explicit instantiations
template std::optional<bool>        Settings_Tree::get<bool>(const std::string&) const;
template std::optional<int>         Settings_Tree::get<int>(const std::string&) const;
template std::optional<float>       Settings_Tree::get<float>(const std::string&) const;
template std::optional<std::string> Settings_Tree::get<std::string>(const std::string&) const;
template std::optional<std::filesystem::path> Settings_Tree::get<std::filesystem::path>(const std::string&) const;

/****************************/
/*     Get (with default)   */
/****************************/
template<typename T>
T Settings_Tree::get(const std::string& path, T default_val) const {
    return get<T>(path).value_or(default_val);
}

// Explicit instantiations
template bool        Settings_Tree::get<bool>(const std::string&, bool) const;
template int         Settings_Tree::get<int>(const std::string&, int) const;
template float       Settings_Tree::get<float>(const std::string&, float) const;
template std::string Settings_Tree::get<std::string>(const std::string&, std::string) const;
template std::filesystem::path Settings_Tree::get<std::filesystem::path>(const std::string&, std::filesystem::path) const;

/****************************/
/*     Set                  */
/****************************/
template<typename T>
void Settings_Tree::set(const std::string& path, T value) {
    // Split path into parent and final key
    size_t last_dot = path.rfind('.');

    toml::table* parent_table = &m_data;
    std::string key = path;

    if (last_dot != std::string::npos) {
        std::string parent_path = path.substr(0, last_dot);
        key = path.substr(last_dot + 1);

        // Navigate/create parent table
        std::istringstream iss(parent_path);
        std::string segment;

        while (std::getline(iss, segment, '.')) {
            auto it = parent_table->find(segment);
            if (it == parent_table->end()) {
                // Create new table
                auto [new_it, inserted] = parent_table->emplace(segment, toml::table());
                parent_table = new_it->second.as_table();
            } else if (it->second.is_table()) {
                // Use existing table
                parent_table = it->second.as_table();
            } else {
                // Replace non-table with table
                parent_table->insert_or_assign(segment, toml::table());
                parent_table = (*parent_table)[segment].as_table();
            }
            if (!parent_table) {
                return;
            }
        }
    }

    // Insert the value
    if constexpr (std::is_same_v<T, bool>) {
        parent_table->insert_or_assign(key, value);
    } else if constexpr (std::is_same_v<T, int>) {
        parent_table->insert_or_assign(key, static_cast<int64_t>(value));
    } else if constexpr (std::is_same_v<T, float>) {
        parent_table->insert_or_assign(key, static_cast<double>(value));
    } else if constexpr (std::is_same_v<T, std::string>) {
        parent_table->insert_or_assign(key, value);
    } else if constexpr (std::is_same_v<T, std::string_view>) {
        parent_table->insert_or_assign(key, std::string(value));
    } else if constexpr (std::is_same_v<T, const char*>) {
        parent_table->insert_or_assign(key, std::string(value));
    } else if constexpr (std::is_same_v<T, std::filesystem::path>) {
        parent_table->insert_or_assign(key, value.string());
    }
}

// Explicit instantiations
template void Settings_Tree::set<bool>(const std::string&, bool);
template void Settings_Tree::set<int>(const std::string&, int);
template void Settings_Tree::set<float>(const std::string&, float);
template void Settings_Tree::set<std::string>(const std::string&, std::string);
template void Settings_Tree::set<std::string_view>(const std::string&, std::string_view);
template void Settings_Tree::set<const char*>(const std::string&, const char*);
template void Settings_Tree::set<std::filesystem::path>(const std::string&, std::filesystem::path);

/****************************/
/*     Has                  */
/****************************/
bool Settings_Tree::has(const std::string& path) const {
    return navigate(path) != nullptr;
}

/****************************/
/*     Remove               */
/****************************/
void Settings_Tree::remove(const std::string& path) {
    // Split path to get parent and key
    size_t last_dot = path.rfind('.');
    if (last_dot == std::string::npos) {
        // Top-level key
        m_data.erase(path);
        return;
    }

    std::string parent_path = path.substr(0, last_dot);
    std::string key = path.substr(last_dot + 1);

    auto* parent = navigate(parent_path);
    if (!parent || !parent->is_table()) {
        return;
    }

    parent->as_table()->erase(key);
}

/****************************/
/*     Child                */
/****************************/
Settings_Tree Settings_Tree::child(const std::string& path) const {
    const auto* node = navigate(path);
    if (!node || !node->is_table()) {
        return Settings_Tree();
    }

    return Settings_Tree(*node->as_table());
}

/****************************/
/*     Merge                */
/****************************/
void Settings_Tree::merge(const Settings_Tree& other) {
    for (auto&& [key, val] : other.m_data) {
        m_data.insert_or_assign(key, val);
    }
}

/****************************/
/*     Empty                */
/****************************/
bool Settings_Tree::empty() const {
    return m_data.empty();
}

/****************************/
/*     Clear                */
/****************************/
void Settings_Tree::clear() {
    m_data.clear();
}

/****************************/
/*     Navigate (const)     */
/****************************/
const toml::node* Settings_Tree::navigate(const std::string& path) const {
    std::istringstream iss(path);
    std::string segment;

    const toml::node* current = &m_data;

    while (std::getline(iss, segment, '.')) {
        if (!current || !current->is_table()) {
            return nullptr;
        }

        const auto& table = *current->as_table();
        auto it = table.find(segment);
        if (it == table.end()) {
            return nullptr;
        }
        current = &it->second;
    }

    return current;
}

/****************************/
/*     Navigate (mutable)   */
/****************************/
toml::node* Settings_Tree::navigate(const std::string& path) {
    return const_cast<toml::node*>(
        static_cast<const Settings_Tree*>(this)->navigate(path)
    );
}

/****************************/
/*     Navigate Create      */
/****************************/
toml::node* Settings_Tree::navigate_create(const std::string& path) {
    std::istringstream iss(path);
    std::string segment;

    toml::table* current_table = &m_data;

    while (std::getline(iss, segment, '.')) {
        // Check if this is the final segment
        bool is_final = iss.eof();

        auto it = current_table->find(segment);

        if (is_final) {
            if (it == current_table->end()) {
                // Insert a placeholder value (we'll overwrite it)
                auto [new_it, inserted] = current_table->emplace(segment, 0);
                return &new_it->second;
            }
            return &it->second;
        }

        // Not final - ensure we have a table
        if (it == current_table->end()) {
            // Create new table
            auto [new_it, inserted] = current_table->emplace(segment, toml::table());
            current_table = new_it->second.as_table();
        } else if (it->second.is_table()) {
            // Use existing table
            current_table = it->second.as_table();
        } else {
            // Replace non-table with table
            current_table->insert_or_assign(segment, toml::table());
            current_table = (*current_table)[segment].as_table();
        }

        if (!current_table) {
            return nullptr;
        }
    }

    // Path was empty or had trailing dot - return the root table as a node
    return &m_data;
}

} // namespace ovb::hal
