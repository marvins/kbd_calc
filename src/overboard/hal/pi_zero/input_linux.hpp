/**
 * @file    input_linux.hpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Linux input event handler
 *
 * Reads keyboard events from /dev/input/event* devices and maps them
 * to calculator key indices for the Pi Zero target.
 */
#pragma once

// C++ Standard Libraries
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/hal/i_input.hpp>

namespace ovb::hal::pi_zero {

/**
 * @brief Linux input event handler
 *
 * Opens input devices and polls for keyboard events. Maps
 * Linux input codes to calculator key indices.
 */
class Linux_Input : public I_Input {

    public:

        /**
         * @brief Constructor
         * @param device_paths Vector of input device paths (e.g., ["/dev/input/event0"])
         */
        explicit Linux_Input(const std::vector<std::string>& device_paths);

        /**
         * Destructor
         */
        ~Linux_Input() override;

        /**
         * @brief Poll for a key event (non-blocking)
         * @param out_event Filled with event data if available
         * @return true if an event was retrieved, false otherwise
         */
        bool poll(Key_Event& out_event) override;

        /**
         * @brief Check if quit was requested (Ctrl+C or similar)
         * @return true if quit was requested
         */
        bool should_quit() const override;

        /**
         * @brief Pump the event loop (process pending events)
         */
        void pump() override;

    private:

        /// @brief PImpl structure
        struct Impl;

        /// @brief Pointer to implementation
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::hal::pi_zero
