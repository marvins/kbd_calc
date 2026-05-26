/**
 * @file    log_level.cpp
 * @author  Marvin Smith
 * @date    5/25/2026
 *
 * @brief   Log level enumeration and utilities
 */
#include <overboard/log/log_level.hpp>

namespace ovb::log {

/**
 * @brief Convert Log_Level to string
 */
const char* to_string(Log_Level level) {
    switch (level) {
        case Log_Level::Trace: return "Trace";
        case Log_Level::Debug: return "Debug";
        case Log_Level::Info:  return "Info";
        case Log_Level::Warn:  return "Warn";
        case Log_Level::Error: return "Error";
        default: return "Unknown";
    }
}

} // namespace ovb::log
