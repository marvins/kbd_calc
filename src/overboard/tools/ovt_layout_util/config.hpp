/**
 * @file    config.hpp
 * @author  Marvin Smith
 * @date    2026-05-30
 *
 * @brief   Configuration for ovt_layout_util application
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <optional>
#include <string>

// Project Libraries
#include <overboard/log/i_logger.hpp>

namespace ovb::tools::lutil {

/**
 * @brief Application configuration
 */
class Config {

    public:

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
        static void print_usage(const std::filesystem::path& program_path);

        /**
         * @return Path to config folder
         */
        inline const std::filesystem::path& config_folder() const { return m_config_folder; }

        /**
         * @return Path to output PNG file
         */
        inline const std::filesystem::path& output_path() const { return m_output_path; }

        /**
         * @return Log severity level
         */
        inline log::Log_Level log_level() const { return m_log_level; }

        /**
         * @return true if help was requested
         */
        inline bool help_requested() const { return m_help_requested; }

    private:

        /**
         * @brief Private constructor
         */
        Config() = default;

        /// @brief Path to config folder
        std::filesystem::path m_config_folder;

        /// @brief Path to output PNG file
        std::filesystem::path m_output_path;

        /// @brief Log severity level
        log::Log_Level m_log_level = log::Log_Level::Info;

        /// @brief Help requested flag
        bool m_help_requested = false;
};

} // namespace ovb::tools::lutil
