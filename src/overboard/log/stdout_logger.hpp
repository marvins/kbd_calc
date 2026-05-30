/**
 * @file    stdout_logger.hpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Stdout logger implementation
 *
 * Hardware-agnostic stdout-based logger. Works on any platform
 * with stdout available (SDL, desktop Linux, etc.).
 * For embedded targets without stdout, implement I_Logger
 * using UART or other hardware output.
 */
#pragma once

// C++ Standard Libraries
#include <memory>

// Project Libraries
#include <overboard/log/i_logger.hpp>

namespace ovb::log {

/**
 * @brief Stdout-based logger implementation
 *
 * Outputs log messages to stdout with timestamps and level tags.
 * Thread-safe for single-threaded use (no internal locking).
 */
class Stdout_Logger : public I_Logger {
    public:
        /**
         * @brief Create logger with minimum output level
         * @param min_level Minimum level to output (Debug=verbose, Error=quiet)
         */
        explicit Stdout_Logger( Log_Level min_level = Log_Level::Debug );

        /**
         * @brief Log a message at the specified level
         * @param level   Severity level
         * @param message Message content
         */
        void log( Log_Level level,
                  std::string_view message ) override;

        /**
         * @brief Log a message at the specified level with source location
         * @param level   Severity level
         * @param loc     Source location
         * @param message Message content
         */
        void log( Log_Level level,
                  std::source_location loc,
                  std::string_view message ) override;

        /**
         * @brief Get current timestamp in microseconds
         * @return Microseconds since steady_clock epoch
         */
        uint64_t now_us() const override;

        // Singleton accessor - creates default logger if none set
        static Stdout_Logger& instance();

        // Initialize with custom log level (call once from main)
        static void initialize( Log_Level level);

    private:

        /// @brief Minimum log level for output
        Log_Level m_min_level;

        /// @brief Singleton instance
        static std::unique_ptr<Stdout_Logger> s_instance;

        /// @brief Get level tag for logging
        static const char* level_tag( Log_Level level );
};

} // namespace ovb::log

// Convenience macros for accessing the singleton logger with source location
#define LOG_TRACE(...) ovb::log::Stdout_Logger::instance().trace( std::source_location::current(),  __VA_ARGS__ )
#define LOG_DEBUG(...) ovb::log::Stdout_Logger::instance().debug( std::source_location::current(),  __VA_ARGS__ )
#define LOG_INFO(...)  ovb::log::Stdout_Logger::instance().info( std::source_location::current(),   __VA_ARGS__ )
#define LOG_WARN(...)  ovb::log::Stdout_Logger::instance().warn( std::source_location::current(),   __VA_ARGS__ )
#define LOG_ERROR(...) ovb::log::Stdout_Logger::instance().error( std::source_location::current(),  __VA_ARGS__ )

