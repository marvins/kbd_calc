/**
 * @file    kn34_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   KN34 hardware display implementation
 *
 * Stub I_Display implementation for the embedded KN34 target.
 * Will drive an SPI LCD and keyboard via LVGL once hardware
 * bring-up is complete.
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_display.hpp>

namespace ovb::hal::kn34 {

/**
 * @brief KN34 hardware display
 *
 * Implements I_Display for the embedded RP2350 target.
 * All methods are currently stubs pending hardware bring-up.
 */
class KN34_Display : public I_Display {
    public:
        KN34_Display() = default;
        ~KN34_Display() override = default;

        /// @brief Refresh LCD section from engine state (TODO: SPI LCD)
        void refresh() override;

        /// @brief Re-render keyboard buttons for current layer (TODO: LVGL)
        void update_layer() override;

        /// @brief Drive LVGL render loop (TODO: embedded tick)
        void render() override;
};

} // namespace ovb::hal::kn34
