/**
 * @file   app_factory.hpp
 * @author Marvin
 * @date   2025-10-18
 * @brief  Platform application factory
 *
 * Creates platform-specific I_App implementations based on build target.
 * Centralizes all #ifdef logic to keep main() and other code clean.
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <memory>

// Project Libraries
#include <overboard/hal/i_app.hpp>
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal {

/**
 * @brief Factory for creating platform-specific applications
 *
 * Uses compile-time target selection (TARGET_SDL, TARGET_SK30, etc.)
 * to instantiate the appropriate I_App implementation. All #ifdef
 * logic is centralized here to keep the rest of the codebase clean.
 */
class App_Factory {
    public:
        /**
         * @brief Create a platform-specific application instance
         * @param layout Keyboard layout for the target device
         * @param keymap_path Path to JSON keymap file
         * @return Unique pointer to the created application
         *
         * The implementation is selected at compile time based on
         * the TARGET_* define passed from CMake.
         */
        static std::unique_ptr<I_App> create( const core::Grid_Layout&     layout,
                                              const std::filesystem::path& layout_path,
                                              const std::filesystem::path& keymap_path );
};

} // namespace ovb::hal
