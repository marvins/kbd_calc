/**
 * @file    settings_tree.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Hierarchical settings storage (property tree style)
 *
 * Simplified property tree API for application settings.
 * Supports dot-notation paths (e.g., "display.brightness") and
 * type-safe get/set with optional default values.
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// Third-Party Libraries
#include <tomlplusplus/toml.hpp>

namespace ovb::hal {

/**
 * @brief Hierarchical settings storage
 *
 * Wraps TOML with a simpler property-tree style API.
 * Supports dot-notation paths for nested access.
 */
class Settings_Tree {
    public:
        /**
         * @brief Setting value types
         */
        using Value = std::variant<
            bool,
            int,
            float,
            std::string,
            std::filesystem::path,
            std::vector<int>,
            std::vector<float>,
            std::vector<std::string>,
            std::vector<std::filesystem::path>
        >;

        /**
         * @brief Default constructor - empty tree
         */
        Settings_Tree() = default;

        /**
         * @brief Construct from TOML table
         */
        explicit Settings_Tree(toml::table table);

        /**
         * @brief Get value at path, return nullopt if not found
         * @param path Dot-notation path (e.g., "display.brightness")
         */
        template<typename T>
        std::optional<T> get(const std::string& path) const;

        /**
         * @brief Get value at path, return default if not found
         * @param path Dot-notation path
         * @param default_val Value to return if path not found or wrong type
         */
        template<typename T>
        T get(const std::string& path, T default_val) const;

        /**
         * @brief Set value at path, creating intermediate tables as needed
         * @param path Dot-notation path
         * @param value Value to set
         */
        template<typename T>
        void set(const std::string& path, T value);

        /**
         * @brief Check if path exists
         */
        bool has(const std::string& path) const;

        /**
         * @brief Remove value at path (if it exists)
         */
        void remove(const std::string& path);

        /**
         * @brief Get child tree at path
         * @return Settings_Tree containing subtree, or empty if not found
         */
        Settings_Tree child(const std::string& path) const;

        /**
         * @brief Merge another tree into this one (overwrite on conflict)
         */
        void merge(const Settings_Tree& other);

        /**
         * @brief Access underlying TOML table
         */
        toml::table&       data()       { return m_data; }
        const toml::table& data() const { return m_data; }

        /**
         * @brief Check if tree is empty
         */
        bool empty() const;

        /**
         * @brief Clear all settings
         */
        void clear();

    private:
        /**
         * @brief Navigate to node at path, return nullptr if not found
         */
        toml::node*       navigate(const std::string& path);
        const toml::node* navigate(const std::string& path) const;

        /**
         * @brief Navigate or create path, building tables as needed
         */
        toml::node* navigate_create(const std::string& path);

        toml::table m_data;
};

} // namespace ovb::hal
