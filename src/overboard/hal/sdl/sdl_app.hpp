/**
 * @file    sdl_app.hpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   SDL simulator application implementation
 *
 * Implements I_App for the SDL simulator target. Handles SDL initialization,
 * window creation, event loop, and cleanup. All SDL-specific logic is
 * encapsulated here to keep main() platform-agnostic.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>

// Project Libraries
#include <overboard/hal/i_app.hpp>
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/math/calc_engine.hpp>
#include <overboard/hal/sdl/sdl_keyboard.hpp>
#include <overboard/hal/sdl/sdl_lcd_display.hpp>
#include <overboard/hal/sdl/sdl_input.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL simulator application implementation
 *
 * Manages the SDL lifecycle for desktop simulation. Creates keyboard
 * and LCD windows, handles the main event loop, and routes key events
 * to the calculator engine.
 */
class SDL_App : public I_App {
    public:
        /**
         * @brief Factory method to create and initialize SDL application
         * @param layout Keyboard layout for the target device
         * @return Unique pointer to initialized SDL_App, or nullptr on failure
         */
        static std::unique_ptr<SDL_App> create(const core::Grid_Layout& layout);

        ~SDL_App() override;

        bool init() override;
        void run() override;
        bool should_quit() const override;
        I_Display& get_keyboard_display() override;
        I_Display& get_lcd_display() override;
        I_Input& get_input() override;

    private:

        /**
         * @brief Private constructor - use create() factory method
         * @param layout Keyboard layout reference
         */
        explicit SDL_App(const core::Grid_Layout& layout);

        const core::Grid_Layout& m_layout;
        bool m_initialized = false;
        bool m_should_quit = false;

        // Core components
        Keymap m_keymap;
        core::Layer_Manager m_layers;
        math::Calc_Engine m_engine;

        // SDL HAL components (created during init)
        std::unique_ptr<SDL_Keyboard>    m_keyboard;
        std::unique_ptr<SDL_LCD_Display> m_lcd_display;
        std::unique_ptr<SDL_Input>       m_input;

        // Window dimensions
        static constexpr int KBD_W  = 480;
        static constexpr int KBD_H  = 480;
        static constexpr int HDR_H       = 22;
        static constexpr int MARGIN_LEFT = 20;
        static constexpr int MARGIN_TOP  = 16;
};

} // namespace ovb::hal::sdl
