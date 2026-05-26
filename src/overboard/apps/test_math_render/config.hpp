/**
 * @file    config.hpp
 * @author  Marvin Smith
 * @date    5/25/2026
 *
 * @brief Configuration for the test_math_render application
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <string>

// Project Libraries
#include <overboard/log/i_logger.hpp>

namespace ovb {


class Config {

    public:

        /**
         * Output Path
         */
        inline std::filesystem::path output_path() const { return m_output_path; }

        /**
         * Expression to render
         */
        inline std::string expression() const { return m_expression; }

        /**
         * Font path (empty for 5x7)
         */
        inline std::filesystem::path font_path() const { return m_font_path; }

        /**
         * Debug boxes enabled
         */
        inline bool debug_boxes() const { return m_debug_boxes; }

        /**
         * Scale factor
         */
        inline float scale() const { return m_scale; }

        /**
         * Use 5x7 font
         */
        inline bool use_5x7() const { return m_use_5x7; }

        /**
         * Canvas width
         */
        inline int width() const { return m_width; }

        /**
         * Canvas height
         */
        inline int height() const { return m_height; }

        /**
         * Log Severity
         */
        inline log::Log_Level log_severity() const { return m_log_severity; }

        /**
         * Print usage information
         */
        void usage() const;

        /**
         * Configure logging based on configuration
         */
        void configure_logging();

        /**
         * Convert configuration to string
         */
        std::string to_string() const;

        /**
         * Parse command line arguments
         * @param argc Number of arguments
         * @param argv Array of arguments
         * @param envp Array of environment variables
         * @return Config object
         */
        static Config parse_command_line( int argc, char* argv[], char** envp );

    private:

        /// Application Name
        std::filesystem::path m_app_name;

        /// Log Severity
        log::Log_Level m_log_severity;

        /// Output Path
        std::filesystem::path m_output_path;

        /// Expression to render
        std::string m_expression;

        /// Font path (empty for 5x7)
        std::filesystem::path m_font_path;

        /// Debug boxes enabled
        bool m_debug_boxes = false;

        /// Scale factor
        float m_scale = 1.0f;

        /// Use 5x7 font
        bool m_use_5x7 = true;

        /// Canvas width
        int m_width = 60;

        /// Canvas height
        int m_height = 60;

}; // End of Config class

} // End of ovb namespace