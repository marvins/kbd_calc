/**
 * @file    input.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Raspberry Pi Pico hardware input driver implementation
 */
#include <overboard/hal/pico/input.hpp>

// Platform-specific includes
#ifdef PICO_PLATFORM
    #include <hardware/gpio.h>
#endif

namespace ovb::hal::pico {

Pico_Input::Pico_Input(const core::Grid_Layout& layout)
    : m_layout(layout) {}

bool Pico_Input::should_quit() const {
    return m_quit;
}

void Pico_Input::pump() {
    // Platform-specific: read key matrix, queue events
    // This is a placeholder implementation

#ifdef PICO_PLATFORM
    // Raspberry Pi Pico implementation would go here
    // Scan the key matrix, detect presses/releases
#else
    // Simulation/placeholder: no-op
    (void)m_layout;  // Suppress unused warning for now
#endif
}

bool Pico_Input::poll(Key_Event& out_event) {
    // Placeholder: return false (no events)
    (void)out_event;
    return false;
}

int Pico_Input::scan_matrix() const {
    // Platform-specific key matrix scanning
    // Returns key index or -1 if no key pressed
    return -1;
}

} // namespace ovb::hal::pico
