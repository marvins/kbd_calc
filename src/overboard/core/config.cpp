/**
 * @file    config.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Application configuration implementation
 */
#include <overboard/core/config.hpp>

// C++ Standard Libraries
#include <iostream>
#include <string_view>

namespace ovb::core {

void Config::print_usage(const std::filesystem::path& program_path) {
    std::cerr << "Usage: " << program_path.filename()
              << " [--layout <path>] [--mapping <path>] [-v <level>] [--help]\n"
              << "  --layout  Path to VIA layout JSON  (default: " << DEFAULT_LAYOUT_PATH  << ")\n"
              << "  --mapping Path to key mapping JSON (default: " << DEFAULT_MAPPING_PATH << ")\n"
              << "  -v        Log severity level (debug, info, warn, error, default: info)\n"
              << "  --help    Show this help message\n";
}

log::Log_Level Config::parse_log_level(std::string_view level_str) {
    if (level_str == "debug") return log::Log_Level::Debug;
    if (level_str == "info")  return log::Log_Level::Info;
    if (level_str == "warn")  return log::Log_Level::Warn;
    if (level_str == "error") return log::Log_Level::Error;
    return log::Log_Level::Info; // default
}

/****************************************/
/*      Parse Command-Line Options      */
/****************************************/
std::optional<Config> Config::parse(int argc, char* argv[]) {
    Config config;
    config.m_program_name = argv[0];
    config.m_layout_path = DEFAULT_LAYOUT_PATH;
    config.m_mapping_path = DEFAULT_MAPPING_PATH;
    config.m_log_level = log::Log_Level::Info;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "--layout" && i + 1 < argc) {
            config.m_layout_path = argv[++i];
        } else if (arg == "--mapping" && i + 1 < argc) {
            config.m_mapping_path = argv[++i];
        } else if (arg == "-v" && i + 1 < argc) {
            config.m_log_level = Config::parse_log_level(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            config.m_help_requested = true;
            return config;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            Config::print_usage(config.m_program_name);
            return std::nullopt;
        }
    }

    return config;
}

} // namespace ovb::core
