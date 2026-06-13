/**
 * @file    enums.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Centralized enumeration definitions with string conversion
 *
 * Shared enumerations used across multiple modules. Each enum includes
 * to_string() and from_string() for config file serialization.
 */
#pragma once

// C++ Standard Libraries
#include <optional>
#include <string>
#include <string_view>

namespace ovb::core {

/**
 * @brief Angle mode for trigonometric functions
 */
enum class Angle_Mode {
    Degrees,
    Radians
};

/**
 * @brief Convert Angle_Mode to string
 */
constexpr std::string_view to_string(Angle_Mode mode) {
    switch (mode) {
        case Angle_Mode::Degrees:
            return "degrees";
        case Angle_Mode::Radians:
            return "radians";
    }
    return "unknown";
}

/**
 * @brief Parse Angle_Mode from string
 */
constexpr std::optional<Angle_Mode> angle_mode_from_string(std::string_view str) {
    if (str == "degrees" || str == "deg") {
        return Angle_Mode::Degrees;
    }
    if (str == "radians" || str == "rad") {
        return Angle_Mode::Radians;
    }
    return std::nullopt;
}

/**
 * @brief Number format for result display
 */
enum class Number_Format {
    Auto,       // Automatically choose based on magnitude
    Fixed,      // Fixed decimal places
    Scientific  // Scientific notation
};

/**
 * @brief Convert Number_Format to string
 */
constexpr std::string_view to_string(Number_Format format) {
    switch (format) {
        case Number_Format::Auto:
            return "auto";
        case Number_Format::Fixed:
            return "fixed";
        case Number_Format::Scientific:
            return "scientific";
    }
    return "unknown";
}

/**
 * @brief Parse Number_Format from string
 */
constexpr std::optional<Number_Format> number_format_from_string(std::string_view str) {
    if (str == "auto") {
        return Number_Format::Auto;
    }
    if (str == "fixed") {
        return Number_Format::Fixed;
    }
    if (str == "scientific" || str == "sci") {
        return Number_Format::Scientific;
    }
    return std::nullopt;
}

/**
 * @brief Display theme preference
 */
enum class Theme {
    Dark,
    Light,
    Auto
};

/**
 * @brief Convert Theme to string
 */
constexpr std::string_view to_string(Theme theme) {
    switch (theme) {
        case Theme::Dark:
            return "dark";
        case Theme::Light:
            return "light";
        case Theme::Auto:
            return "auto";
    }
    return "unknown";
}

/**
 * @brief Parse Theme from string
 */
constexpr std::optional<Theme> theme_from_string(std::string_view str) {
    if (str == "dark") {
        return Theme::Dark;
    }
    if (str == "light") {
        return Theme::Light;
    }
    if (str == "auto") {
        return Theme::Auto;
    }
    return std::nullopt;
}

/**
 * @brief Timezone mode for clock display
 */
enum class Timezone_Mode {
    Auto,       // Use system timezone
    UTC,        // UTC/GMT
    Custom      // User-specified timezone
};

/**
 * @brief Convert Timezone_Mode to string
 */
constexpr std::string_view to_string(Timezone_Mode mode) {
    switch (mode) {
        case Timezone_Mode::Auto:
            return "auto";
        case Timezone_Mode::UTC:
            return "utc";
        case Timezone_Mode::Custom:
            return "custom";
    }
    return "unknown";
}

/**
 * @brief Parse Timezone_Mode from string
 */
constexpr std::optional<Timezone_Mode> timezone_mode_from_string(std::string_view str) {
    if (str == "auto") {
        return Timezone_Mode::Auto;
    }
    if (str == "utc" || str == "UTC" || str == "gmt" || str == "GMT") {
        return Timezone_Mode::UTC;
    }
    if (str == "custom") {
        return Timezone_Mode::Custom;
    }
    return std::nullopt;
}

} // namespace ovb::core
