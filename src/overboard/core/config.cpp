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
              << " [--layout <path>] [--keymap <path>] [--layers <path>] [-v <level>] [--help]\n"
              << "  --layout  Path to VIA layout JSON or config folder (default: " << DEFAULT_CONFIG_PATH << ")\n"
              << "  --keymap  Path to keymap JSON          (default: <config folder>/keymap.json)\n"
              << "  --layers  Path to layers JSON          (default: <config folder>/layers.json)\n"
              << "  -v        Log severity level (trace, debug, info, warn, error, default: info)\n"
              << "  --help    Show this help message\n";
}

log::Log_Level Config::parse_log_level(std::string_view level_str) {
    if (level_str == "trace") return log::Log_Level::Trace;
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

    // Set default paths from config folder
    std::filesystem::path default_config_path = DEFAULT_CONFIG_PATH;
    config.m_layout_path  = default_config_path / "main.json";
    config.m_keymap_path  = default_config_path / "keymap.json";
    config.m_layers_path  = default_config_path / "layers.json";
    config.m_log_level    = log::Log_Level::Info;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "--layout" && i + 1 < argc) {
            std::filesystem::path layout_path = argv[++i];
            if (std::filesystem::is_directory(layout_path)) {
                config.m_layout_path = layout_path / "main.json";
                config.m_keymap_path = layout_path / "keymap.json";
                config.m_layers_path = layout_path / "layers.json";
            } else {
                config.m_layout_path = layout_path;
            }
        } else if (arg == "--keymap" && i + 1 < argc) {
            config.m_keymap_path = argv[++i];
        } else if (arg == "--layers" && i + 1 < argc) {
            config.m_layers_path = argv[++i];
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
