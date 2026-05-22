/**
 * @file kn34_app.hpp
 * @brief KISNT KN34 hardware application implementation
 *
 * Embedded application for KISNT KN34 hardware. Handles
 * hardware initialization, key matrix scanning, and display
 * management for the embedded target.
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_app.hpp>
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal::kn34 {

/**
 * @brief KN34 hardware application implementation
 *
 * Manages embedded hardware lifecycle for the KISNT KN34.
 * Handles key matrix scanning and display updates.
 */
class KN34_App : public I_App {
    public:
        explicit KN34_App(const core::Grid_Layout& layout);

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

} // namespace ovb::hal::kn34
