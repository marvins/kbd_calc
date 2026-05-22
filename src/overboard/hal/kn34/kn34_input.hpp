/**
 * @file   kn34_input.hpp
 * @author Marvin Smith
 * @date   2025-10-18
 * @brief  KISNT KN34 hardware input driver
 *
 * Reads key matrix from the KN34 and generates Key_Event messages.
 * Platform-specific implementation for embedded targets.
 */

#pragma once

// Project Libraries
#include <overboard/hal/i_input.hpp>
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal::kn34 {

/**
 * @brief KN34 hardware input implementation
 *
 * Maps physical key matrix positions to logical key indices
 * based on the Grid_Layout configuration.
 */
class KN34_Input : public I_Input {
    public:
        explicit KN34_Input(const core::Grid_Layout& layout);

        bool poll(Key_Event& out_event) override;
        bool should_quit() const override;
        void pump() override;

    private:
        const core::Grid_Layout& m_layout;
        bool m_quit = false;

        // Platform-specific: scan matrix and return key index or -1
        int scan_matrix() const;
};

} // namespace ovb::hal::kn34
