/**
 * @file    display.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Raspberry Pi Pico hardware display implementation
 *
 * Stub I_Display implementation for the RP2350 Pico target.
 * Will drive an SPI LCD via LVGL once hardware bring-up is complete.
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_display.hpp>

namespace ovb::hal::pico {

/**
 * @brief Pico hardware display
 *
 * Implements I_Display for the embedded RP2350 target.
 * All methods are currently stubs pending hardware bring-up.
 */
class Pico_Display : public I_Display {

    public:

        /**
         * @brief Constructor
         */
        Pico_Display() = default;

        /**
         * @brief Destructor
         */
        ~Pico_Display() override = default;

        /**
         * @brief Refresh LCD section from engine state (TODO: SPI LCD)
         */
        void refresh() override;

        /**
         * @brief Re-render keyboard buttons for current layer (TODO: LVGL)
         */
        void update_layer() override;

        /// @brief Drive LVGL render loop (TODO: embedded tick)
        void render() override;
};

} // namespace ovb::hal::pico
