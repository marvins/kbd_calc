/**
 * @file    display.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   SDL window driver
 *
 * Creates and owns the SDL window and LVGL display handle.
 * Platform HAL responsibility only — no widget management.
 * Consumers call screen() to get the LVGL root for GUI attachment.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string>

// Forward declarations
struct SDL_Window;

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::hal::sdl {

/**
 * @brief SDL window driver
 *
 * Owns the SDL window and LVGL display handle. Exposes the LVGL
 * active screen so the GUI layer can attach widgets to it.
 */
class Display {

    public:

        /**
         * @brief Create the SDL window and initialise LVGL
         *
         * @param title  Window title bar text
         * @param width  Total window width in pixels
         * @param height Total window height in pixels
         */
        Display( const std::string& title, int width, int height );

        ~Display();

        /// @brief SDL window ID for event filtering
        uint32_t window_id() const;

        /// @brief LVGL active screen — attach GUI widgets here
        lv_obj_t* screen() const;

    private:

        /// @brief PImpl (Pointer to Implementation) structure
        struct Impl;

        /// @brief Pointer to implementation
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::hal::sdl
