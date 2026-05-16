#pragma once

#include <cstdint>
#include <string_view>

enum class Log_Level : uint8_t {
    Debug,
    Info,
    Warn,
    Error,
};

class I_Logger {
    public:
        virtual ~I_Logger() = default;

        virtual void log(Log_Level level, std::string_view message) = 0;

        void debug(std::string_view msg) { log(Log_Level::Debug, msg); }
        void info (std::string_view msg) { log(Log_Level::Info,  msg); }
        void warn (std::string_view msg) { log(Log_Level::Warn,  msg); }
        void error(std::string_view msg) { log(Log_Level::Error, msg); }

        virtual uint64_t now_us() const = 0;
};
