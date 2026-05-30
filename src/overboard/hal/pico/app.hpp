/**
 * @file    app.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Raspberry Pi Pico hardware application implementation
 *
 * Embedded application for the RP2350 Pico target. Handles hardware
 * initialisation, key matrix scanning, and display management.
 */
#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_app.hpp>
#include <overboard/hal/pico/display.hpp>

namespace ovb::hal::pico {

/**
 * @brief Pico hardware application implementation
 *
 * Manages embedded hardware lifecycle for the RP2350 Pico target.
 * Handles key matrix scanning and display updates.
 */
class Pico_App : public I_App {

    public:

        /**
         * @brief Constructor
         *
         * @param layout Keyboard layout
         */
        explicit Pico_App( const core::Grid_Layout& layout );

        /**
         * @brief Initialize hardware
         *
         * @return true if initialization succeeded, false otherwise
         */
        bool init() override;

        /**
         * @brief Run the main event loop
         */
        void run() override;

        /**
         * @brief Check if the application should quit
         *
         * @return true if the application should quit, false otherwise
         */
        bool should_quit() const override;

        /**
         * @brief Get the display interface
         *
         * @return Reference to the display interface
         */
        I_Display& get_display() override;

        /**
         * @brief Get the input interface
         *
         * @return Reference to the input interface
         */
        I_Input&   get_input()   override;

    private:

        /// Display interface
        Pico_Display m_display;
};

} // namespace ovb::hal::pico
