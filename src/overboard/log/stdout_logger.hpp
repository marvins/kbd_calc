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
        explicit Stdout_Logger(Log_Level min_level = Log_Level::Debug);

        /**
         * @brief Log a message at the specified level
         * @param level   Severity level
         * @param message Message content
         */
        void log(Log_Level level, std::string_view message) override;

        /**
         * @brief Get current timestamp in microseconds
         * @return Microseconds since steady_clock epoch
         */
        uint64_t now_us() const override;

    private:
        Log_Level m_min_level;

        static const char* level_tag(Log_Level level);
};

} // namespace ovb::log
