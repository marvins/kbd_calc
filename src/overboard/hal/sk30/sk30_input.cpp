/**
 * @file sk30_input.cpp
 * @brief Womier SK30 hardware input driver implementation
 *
 * Platform-specific implementation for scanning the key matrix
 * and generating key events.
 */

// Project Libraries
#include <overboard/hal/sk30/sk30_input.hpp>

// Platform-specific includes (conditional compilation)
#ifdef PICO_PLATFORM
    #include <hardware/gpio.h>
#endif

namespace ovb::hal::sk30 {

SK30_Input::SK30_Input(const core::Grid_Layout& layout)
    : m_layout(layout) {}

bool SK30_Input::should_quit() const {
    return m_quit;
}

void SK30_Input::pump() {
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

bool SK30_Input::poll(Key_Event& out_event) {
    // Placeholder: return false (no events)
    (void)out_event;
    return false;
}

int SK30_Input::scan_matrix() const {
    // Platform-specific key matrix scanning
    // Returns key index or -1 if no key pressed
    return -1;
}

} // namespace ovb::hal::sk30
