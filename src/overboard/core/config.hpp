/**
 * @file    config.hpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Application configuration from command-line arguments
 */

#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <optional>
#include <string>

// Project Libraries
#include <overboard/log/i_logger.hpp>

namespace ovb::core {

/**
 * @brief Application configuration
 *
 * Holds configuration parsed from command-line arguments.
 */
class Config {
    public:
        static constexpr std::string_view DEFAULT_LAYOUT_PATH  = "data/MF34.json";
        static constexpr std::string_view DEFAULT_KEYMAP_PATH  = "data/MF34.keymap.json";
        static constexpr std::string_view DEFAULT_LAYERS_PATH  = "data/MF34.layers.json";

        /**
         * @brief Parse command-line arguments
         * @param argc Argument count
         * @param argv Argument values
         * @return Config instance, or nullopt if parsing failed
         */
        static std::optional<Config> parse(int argc, char* argv[]);

        /**
         * @brief Print usage information
         * @param program_path Path to the program executable
         */
        static void print_usage( const std::filesystem::path& program_path );

        /**
         * @brief Parse log level string to enum
         * @param level_str Log level string (debug, info, warn, error)
         * @return Log level enum, defaults to Info if unknown
         */
        static log::Log_Level parse_log_level( std::string_view level_str);

        /// @return Path to VIA layout JSON file
        inline const std::filesystem::path& layout_path() const { return m_layout_path; }

        /// @return Path to keymap JSON file (scancodes)
        inline const std::filesystem::path& keymap_path() const { return m_keymap_path; }

        /// @return Path to layers JSON file (key-code assignments per layer)
        inline const std::filesystem::path& layers_path() const { return m_layers_path; }

        /// @return Log severity level
        inline log::Log_Level log_level() const { return m_log_level; }

        /// @return true if help was requested
        inline bool help_requested() const { return m_help_requested; }

        /// @return Program name
        inline const std::filesystem::path& program_name() const { return m_program_name; }

    private:
        Config() = default;

        std::filesystem::path m_program_name;
        std::filesystem::path m_layout_path;
        std::filesystem::path m_keymap_path;
        std::filesystem::path m_layers_path;
        log::Log_Level m_log_level;
        bool m_help_requested = false;
};

} // namespace ovb::core
