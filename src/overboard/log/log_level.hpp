/**
 * @file    log_level.hpp
 * @author  Marvin Smith
 * @date    5/25/2026
 *
 * @brief   Log level enumeration and utilities
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>

namespace ovb::log {

enum class Log_Level : uint8_t {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
};

/**
 * @brief Convert Log_Level to string
 */
const char* to_string(Log_Level level);

} // namespace ovb::log
