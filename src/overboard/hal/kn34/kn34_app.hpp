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
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_app.hpp>
#include <overboard/hal/kn34/kn34_display.hpp>

namespace ovb::hal::kn34 {

/**
 * @brief KN34 hardware application implementation
 *
 * Manages embedded hardware lifecycle for the KISNT KN34.
 * Handles key matrix scanning and display updates.
 */
class KN34_App : public I_App {
    public:

        /**
         * @brief Constructor
         *
         * @param layout Keyboard layout
         */
        explicit KN34_App(const core::Grid_Layout& layout);

        /**
         * @brief Initialize the application
         *
         * @return True if initialization was successful
         */
        bool       init()         override;

        /**
         * @brief Run the application loop
         */
        void       run()          override;

        /**
         * @brief Check if the application should quit
         *
         * @return True if the application should quit
         */
        bool       should_quit()  const override;

        /**
         * @brief Get the display interface
         *
         * @return Reference to the display interface
         */
        I_Display& get_display()  override;

        /**
         * @brief Get the input interface
         *
         * @return Reference to the input interface
         */
        I_Input&   get_input()    override;

    private:
        // TODO: Platform-specific display and input implementations
        // For embedded targets, these will likely be hardware-specific
        // (SPI LCD, GPIO matrix scan, etc.)
        KN34_Display m_display;
};

} // namespace ovb::hal::kn34
