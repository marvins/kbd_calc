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
#include <filesystem>
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
         * @param keymap_path Path to JSON keymap file
         * @return Unique pointer to initialized SDL_App, or nullptr on failure
         */
        static std::unique_ptr<SDL_App> create(const core::Grid_Layout& layout,
                                               const std::filesystem::path& layout_path,
                                               const std::filesystem::path& keymap_path,
                                               const std::filesystem::path& layers_path);

        ~SDL_App() override;

        bool init() override;
        void run() override;
        bool should_quit() const override;
        I_Display& get_keyboard_display() override;
        I_Display& get_lcd_display() override;

        void handle_key(int key_index);

    private:

        /**
         * @brief Private constructor - use create() factory method
         * @param layout Keyboard layout reference
         */
        explicit SDL_App(const core::Grid_Layout& layout);

        const core::Grid_Layout&          m_layout;
        std::filesystem::path             m_layout_path;
        std::filesystem::path             m_keymap_path;
        std::filesystem::path             m_layers_path;
        bool                              m_initialized = false;
        bool                              m_should_quit = false;

        // Core components
        core::Keymap m_keymap;
        core::Layer_Manager m_layers;
        math::Calc_Engine m_engine;

        // SDL HAL components (created during init)
        std::unique_ptr<SDL_Keyboard>    m_keyboard;
        std::unique_ptr<SDL_LCD_Display> m_lcd_display;
        std::unique_ptr<SDL_Input>       m_input;

        SDL_Keymap m_sdl_keymap;

        static void on_key_clicked(int key_index, void* user_data);

        // Window dimensions
        static constexpr int KBD_W = 480;
        static constexpr int KBD_H = 480;
};

} // namespace ovb::hal::sdl
