/**
 * @file    stdout_logger.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Stdout logger implementation
 */
#include <overboard/log/stdout_logger.hpp>

// C++ Standard Libraries
#include <chrono>
#include <cstdio>
#include <memory>

namespace ovb::log {

// Static unique_ptr for singleton instance
std::unique_ptr<Stdout_Logger> Stdout_Logger::s_instance;

Stdout_Logger::Stdout_Logger(Log_Level min_level)
    : m_min_level(min_level) {}

Stdout_Logger& Stdout_Logger::instance() {
    if (!s_instance) {
        s_instance = std::make_unique<Stdout_Logger>(Log_Level::Debug);
    }
    return *s_instance;
}

void Stdout_Logger::initialize(Log_Level level) {
    s_instance = std::make_unique<Stdout_Logger>(level);
}

uint64_t Stdout_Logger::now_us() const {
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
}

const char* Stdout_Logger::level_tag(Log_Level level) {
    switch (level) {
        case Log_Level::Trace: return "TRACE";
        case Log_Level::Debug: return "DEBUG";
        case Log_Level::Info:  return "INFO";
        case Log_Level::Warn:  return "WARN";
        case Log_Level::Error: return "ERROR";
        default:               return "???";
    }
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

} // namespace ovb::log
