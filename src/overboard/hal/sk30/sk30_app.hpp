/**
 * @file sk30_app.hpp
 * @brief Womier SK30 hardware application implementation
 *
 * Embedded application for Womier SK30 hardware. Handles
 * hardware initialization, key matrix scanning, and display
 * management for the embedded target.
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_app.hpp>
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal::sk30 {

/**
 * @brief SK30 hardware application implementation
 *
 * Manages embedded hardware lifecycle for the Womier SK30.
 * Handles key matrix scanning and display updates.
 */
class SK30_App : public I_App {
    public:
        explicit SK30_App(const core::Grid_Layout& layout);

        bool init() override;
        void run() override;
        bool should_quit() const override;
        I_Display& get_keyboard_display() override;
        I_Display& get_lcd_display() override;
        I_Input& get_input() override;

    private:
    // TODO: Platform-specific display and input implementations
    // For embedded targets, these will likely be hardware-specific
    // (SPI LCD, GPIO matrix scan, etc.)
};

} // namespace ovb::hal::sk30
