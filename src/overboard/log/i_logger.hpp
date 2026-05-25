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
#include <cstdint>
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace ovb::log {

enum class Log_Level : uint8_t {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
};

/**
 * @brief   Logger interface for platform abstraction
 */
class I_Logger {

    public:
        virtual ~I_Logger() = default;

        virtual void log(Log_Level level, std::string_view message) = 0;

        template<typename... Args>
        void log(Log_Level level, std::string_view fmt, Args&&... args) {
            log(level, format_impl(fmt, std::forward<Args>(args)...));
        }

        void trace(std::string_view msg) { log(Log_Level::Trace, msg); }
        void debug(std::string_view msg) { log(Log_Level::Debug, msg); }
        void info (std::string_view msg) { log(Log_Level::Info,  msg); }
        void warn (std::string_view msg) { log(Log_Level::Warn,  msg); }
        void error(std::string_view msg) { log(Log_Level::Error, msg); }

        template<typename... Args>
        void trace(std::string_view fmt, Args&&... args) { log(Log_Level::Trace, fmt, std::forward<Args>(args)...); }
        template<typename... Args>
        void debug(std::string_view fmt, Args&&... args) { log(Log_Level::Debug, fmt, std::forward<Args>(args)...); }
        template<typename... Args>
        void info (std::string_view fmt, Args&&... args) { log(Log_Level::Info,  fmt, std::forward<Args>(args)...); }
        template<typename... Args>
        void warn (std::string_view fmt, Args&&... args) { log(Log_Level::Warn,  fmt, std::forward<Args>(args)...); }
        template<typename... Args>
        void error(std::string_view fmt, Args&&... args) { log(Log_Level::Error, fmt, std::forward<Args>(args)...); }

        virtual uint64_t now_us() const = 0;

    private:
        template<typename... Args>
        static std::string format_impl(std::string_view fmt, Args&&... args) {
            return format_impl_recursive(fmt, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        static std::string format_impl_recursive(std::string_view fmt, T&& first, Args&&... args) {
            std::ostringstream oss;
            oss << first;
            return oss.str() + format_impl_recursive(fmt, std::forward<Args>(args)...);
        }

        static std::string format_impl_recursive(std::string_view fmt) {
            return std::string(fmt);
        }
};

} // namespace ovb::log
