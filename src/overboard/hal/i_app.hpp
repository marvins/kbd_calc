/**
 * @file   i_app.hpp
 * @author Marvin
 * @date   2025-10-18
 * @brief  Platform application interface
 *
 * Abstracts platform-specific application lifecycle (window management,
 * event loops, etc.) to keep main() platform-agnostic.
 */

#pragma once

// C++ Standard Libraries
#include <memory>
#include <stdexcept>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_display.hpp>
#include <overboard/hal/i_input.hpp>

namespace ovb::hal {

/**
 * @brief Platform application interface
 *
 * Encapsulates platform-specific initialization, event loop, and
 * resource management. Implementations handle window creation,
 * input handling, and display management for each platform.
 */
class I_App {
    public:

        /**
         * @brief Virtual destructor for proper inheritance
         */
        virtual ~I_App() = default;

        /**
         * @brief Initialize the application (create windows, setup hardware)
         * @return true if initialization was successful, false otherwise
         */
        virtual bool init() = 0;

        /**
         * @brief Run the main event loop (returns when quit requested)
         */
        virtual void run() = 0;

        /**
         * @brief Check if quit was requested
         * @return true if quit was requested, false otherwise
         */
        virtual bool should_quit() const = 0;

        /**
         * @brief Get the unified display (LCD + keyboard)
         * @return Reference to the platform display
         */
        virtual I_Display& get_display() = 0;

        /**
         * @brief Get the input interface (only for embedded targets with physical matrix)
         * @return Reference to the input interface
         * @throws std::runtime_error if the platform doesn't support input
         */
        virtual ovb::hal::I_Input& get_input() {
            throw std::runtime_error("get_input not implemented for this platform");
        }
};

} // namespace ovb::hal
