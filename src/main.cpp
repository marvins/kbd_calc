/**
 * @file    main.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Main entry point for the calculator application.
 */

// C++ Standard Libraries
#include <iostream>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/app_factory.hpp>

using namespace ovb;

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        // Use KISNT KN34 asymmetric layout (8×7 grid with split sections)
        core::Grid_Layout kn34_layout = core::Grid_Layout::kn34();

        // Create platform-specific application via factory
        auto app = hal::App_Factory::create(kn34_layout);

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
