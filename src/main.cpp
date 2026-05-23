/**
 * @file    main.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Main entry point for the calculator application.
 */

// C++ Standard Libraries
#include <iostream>

// Project Libraries
#include <overboard/core/config.hpp>
#include <overboard/hal/app_factory.hpp>
#include <overboard/hal/config/target.hpp>
#include <overboard/log/stdout_logger.hpp>

using namespace ovb;

int main(int argc, char* argv[]) {
    auto config_opt = core::Config::parse(argc, argv);
    if (!config_opt) {
        return 1;
    }

    const auto& config = config_opt.value();

    if (config.help_requested()) {
        return 0;
    }

    log::Stdout_Logger logger(config.log_level());

    try {
        core::Grid_Layout layout = hal::config::create_layout(
            config.layout_path(),
            config.mapping_path()
        );

        // Create platform-specific application via factory
        auto app = hal::App_Factory::create(layout);

        if (!app) {
            std::cerr << "Failed to create application\n";
            return 1;
        }

        // Run the main event loop
        app->run();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
