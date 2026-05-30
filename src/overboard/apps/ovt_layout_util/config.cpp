/**
 * @file    config.cpp
 * @author  Marvin Smith
 * @date    2026-05-30
 *
 * @brief   Configuration implementation for ovt_layout_util application
 */
#include <overboard/apps/ovt_layout_util/config.hpp>

// C++ Standard Libraries
#include <iostream>
#include <string_view>

namespace ovb::apps::lutil {

/*********************************/
/*          Print Usage          */
/*********************************/
void Config::print_usage(const std::filesystem::path& program_path) {
    std::cerr << "Usage: " << program_path.filename()
              << " [-v|-vv] <config_folder> <output.png>\n"
              << "  -v        Enable debug logging\n"
              << "  -vv       Enable trace logging\n"
              << "  --help    Show this help message\n";
}

/****************************************/
/*      Parse Command-Line Options      */
/****************************************/
std::optional<Config> Config::parse(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "-v") {
            config.m_log_level = log::Log_Level::Debug;
        } else if (arg == "-vv") {
            config.m_log_level = log::Log_Level::Trace;
        } else if (arg == "--help" || arg == "-h") {
            config.m_help_requested = true;
            return config;
        } else if (config.m_config_folder.empty()) {
            config.m_config_folder = arg;
        } else if (config.m_output_path.empty()) {
            config.m_output_path = arg;
        }
    }

    return config;
}

} // namespace ovb::apps::lutil
