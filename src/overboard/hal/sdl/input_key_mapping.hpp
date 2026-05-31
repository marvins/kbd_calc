/**
 * @file    input_key_mapping.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   SDL Scancode to Input_Key mapping
 *
 * Provides the translation layer from SDL-specific scancodes
 * to hardware-agnostic Input_Key codes.
 */
#pragma once

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop

// Project Libraries
#include <overboard/core/input_key.hpp>

namespace ovb::hal::sdl {

/**
 * @brief Convert SDL scancode to Input_Key
 * @param scancode SDL scancode
 * @return Corresponding Input_Key, or Input_Key::NONE if not mapped
 */
core::Input_Key scancode_to_input_key(SDL_Scancode scancode);

/**
 * @brief Convert Input_Key to SDL scancode (for testing/simulation)
 * @param key Input_Key
 * @return SDL scancode, or SDL_SCANCODE_UNKNOWN if not mapped
 */
SDL_Scancode input_key_to_scancode(core::Input_Key key);

} // namespace ovb::hal::sdl
