/**
 * @file    test_main.cpp
 * @author  <author>
 * @date    2026-05-31
 *
 * @brief   Custom main for test executable with verbosity flags
 */

// C++ Standard Libraries
#include <iostream>
#include <string>

// Google Test
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>
#include <overboard/log/log_level.hpp>

// Global logger for tests
static ovb::log::Stdout_Logger* g_logger = nullptr;

int main(int argc, char** argv) {
    // Default log level is Info
    ovb::log::Log_Level log_level = ovb::log::Log_Level::Info;

    // Parse command-line arguments for verbosity flags
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v") {
            log_level = ovb::log::Log_Level::Debug;
        } else if (arg == "-vv") {
            log_level = ovb::log::Log_Level::Trace;
        }
    }

    // Create and initialize logger with specified level
    static ovb::log::Stdout_Logger logger(log_level);
    g_logger = &logger;

    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Run all tests
    return RUN_ALL_TESTS();
}
