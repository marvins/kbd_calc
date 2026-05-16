#include "stdout_logger.hpp"

#include <cstdio>
#include <chrono>

Stdout_Logger::Stdout_Logger(Log_Level min_level)
    : m_min_level(min_level) {}

uint64_t Stdout_Logger::now_us() const {
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
}

const char* Stdout_Logger::level_tag(Log_Level level) {
    switch (level) {
        case Log_Level::Debug: return "DBG";
        case Log_Level::Info:  return "INF";
        case Log_Level::Warn:  return "WRN";
        case Log_Level::Error: return "ERR";
    }
    return "???";
}

void Stdout_Logger::log(Log_Level level, std::string_view message) {
    if (level < m_min_level) return;

    const uint64_t us = now_us();
    const uint64_t secs  = us / 1'000'000;
    const uint64_t frac  = (us % 1'000'000) / 1'000;

    std::printf("[%4llu.%03llus] [%s] %.*s\n",
        static_cast<unsigned long long>(secs),
        static_cast<unsigned long long>(frac),
        level_tag(level),
        static_cast<int>(message.size()),
        message.data());
}
