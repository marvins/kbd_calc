/**
 * @file    input.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Raspberry Pi Pico hardware input driver
 *
 * Reads the key matrix and generates Key_Event messages.
 * Platform-specific implementation for the RP2350 embedded target.
 */
#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/hal/i_input.hpp>

namespace ovb::hal::pico {

/**
 * @brief Pico hardware input implementation
 *
 * Maps physical key matrix positions to logical key indices
 * based on the Grid_Layout configuration.
 */
class Pico_Input : public I_Input {

    public:

        /**
         * @brief Constructor
         *
         * @param layout Grid layout configuration
         */
        explicit Pico_Input( const core::Grid_Layout& layout );

        /**
         * @brief Poll for input events
         *
         * @param out_event Output event
         * @return true if event was generated
         * @return false if no event
         */
        bool poll( Key_Event& out_event ) override;

        /**
         * @brief Check if application should quit
         *
         * @return true if quit requested
         * @return false otherwise
         */
        bool should_quit() const override;

        /**
         * @brief Pump input events
         */
        void pump() override;

    private:

        /// @brief Scan the key matrix
        int scan_matrix() const;

        /// @brief Grid layout configuration
        const core::Grid_Layout& m_layout;

        /// @brief Quit flag
        bool                     m_quit = false;
};

} // namespace ovb::hal::pico
