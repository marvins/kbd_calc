/**
 * @file target.hpp
 * @brief Build-time target selection
 *
 * Includes the appropriate target configuration based on build flags.
 * Use TARGET_SDL or TARGET_SK30 to select the hardware target.
 */

#pragma once

// Project Libraries
#ifdef TARGET_SDL
    #include <overboard/hal/config/target_sdl.hpp>
#elif defined(TARGET_SK30)
    #include <overboard/hal/config/target_sk30.hpp>
#else
    #error "No target defined. Define TARGET_SDL or TARGET_SK30."
#endif

namespace ovb::hal::config {

/// @brief Target-specific layout factory function
using Layout_Factory = core::Grid_Layout(*)();

} // namespace ovb::hal::config
