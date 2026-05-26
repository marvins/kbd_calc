/**
 * @file    i_logger.hpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Logger interface for platform abstraction
 *
 * Core logging interface that can be implemented for
 * different targets (stdout, file, hardware UART, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <format>
#include <sstream>
#include <string>
#include <string_view>

// Project Libraries
#include <overboard/log/log_level.hpp>

namespace ovb::log {

/**
 * @brief   Logger interface for platform abstraction
 */
class I_Logger {

    public:
        virtual ~I_Logger() = default;

        /**
         * @brief Log a message at the specified level
         * @param level Log severity level
         * @param message Message to log
         */
        virtual void log(Log_Level level, std::string_view message) = 0;

        /**
         * @brief Log a formatted message at the specified level
         * @param level Log severity level
         * @param fmt Format string
         * @param args Format arguments
         */
        template<typename... Args>
        void log(Log_Level level, std::string_view fmt, Args&&... args) {
            log(level, format_impl(fmt, std::forward<Args>(args)...));
        }

        /**
         * @brief Log a trace-level message
         * @param msg Message to log
         */
        void trace(std::string_view msg) { log(Log_Level::Trace, msg); }

        /**
         * @brief Log a debug-level message
         * @param msg Message to log
         */
        void debug(std::string_view msg) { log(Log_Level::Debug, msg); }

        /**
         * @brief Log an info-level message
         * @param msg Message to log
         */
        void info (std::string_view msg) { log(Log_Level::Info,  msg); }

        /**
         * @brief Log a warning-level message
         * @param msg Message to log
         */
        void warn (std::string_view msg) { log(Log_Level::Warn,  msg); }

        /**
         * @brief Log an error-level message
         * @param msg Message to log
         */
        void error(std::string_view msg) { log(Log_Level::Error, msg); }

        /**
         * @brief Log a formatted trace-level message
         * @param fmt Format string
         * @param args Format arguments
         */
        template<typename... Args>
        void trace(std::string_view fmt, Args&&... args) { log(Log_Level::Trace, fmt, std::forward<Args>(args)...); }

        /**
         * @brief Log a formatted debug-level message
         * @param fmt Format string
         * @param args Format arguments
         */
        template<typename... Args>
        void debug(std::string_view fmt, Args&&... args) { log(Log_Level::Debug, fmt, std::forward<Args>(args)...); }

        /**
         * @brief Log a formatted info-level message
         * @param fmt Format string
         * @param args Format arguments
         */
        template<typename... Args>
        void info (std::string_view fmt, Args&&... args) { log(Log_Level::Info,  fmt, std::forward<Args>(args)...); }

        /**
         * @brief Log a formatted warning-level message
         * @param fmt Format string
         * @param args Format arguments
         */
        template<typename... Args>
        void warn (std::string_view fmt, Args&&... args) { log(Log_Level::Warn,  fmt, std::forward<Args>(args)...); }

        /**
         * @brief Log a formatted error-level message
         * @param fmt Format string
         * @param args Format arguments
         */
        template<typename... Args>
        void error(std::string_view fmt, Args&&... args) { log(Log_Level::Error, fmt, std::forward<Args>(args)...); }

        /**
         * @brief Get current time in microseconds
         * @return Current time in microseconds
         */
        virtual uint64_t now_us() const = 0;

    private:
        /**
         * @brief Format implementation entry point
         * @param fmt Format string
         * @param args Format arguments
         * @return Formatted string
         */
        template<typename... Args>
        static std::string format_impl(std::string_view fmt, Args&&... args) {
            return format_impl_recursive(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Recursive format implementation
         * @param fmt Format string
         * @param first First argument
         * @param args Remaining arguments
         * @return Formatted string
         */
        template<typename T, typename... Args>
        static std::string format_impl_recursive(std::string_view fmt, T&& first, Args&&... args) {
            std::ostringstream oss;
            oss << first;
            return oss.str() + format_impl_recursive(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Base case for recursive format implementation
         * @param fmt Format string
         * @return Format string as-is
         */
        static std::string format_impl_recursive(std::string_view fmt) {
            return std::string(fmt);
        }
};

} // namespace ovb::log
