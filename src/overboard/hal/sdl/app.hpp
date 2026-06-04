/**
 * @file    app.hpp
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
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/gui/keyboard_display.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/i_app.hpp>
#include <overboard/hal/sdl/display.hpp>
#include <overboard/hal/sdl/input.hpp>
#include <overboard/hal/sdl/keyboard_window.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL simulator application implementation
 *
 * Manages the SDL lifecycle for desktop simulation. Creates a single
 * merged window with LCD at top and keyboard at bottom, handles the
 * main event loop, and routes key events to the calculator engine.
 */
class SDL_App : public I_App {

    public:

        /// @brief Display width
        static constexpr int DISPLAY_WIDTH = ovb::hal::FULL_WIDTH;

        /// @brief Display height
        static constexpr int DISPLAY_HEIGHT = ovb::hal::FULL_HEIGHT;

        /**
         * @brief Factory method to create and initialize SDL application
         * @param layout Keyboard layout for the target device
         * @param layout_path Path to keyboard.json file
         * @return Unique pointer to initialized SDL_App, or nullptr on failure
         */
        static std::unique_ptr<SDL_App> create(const core::Grid_Layout& layout,
                                               const std::filesystem::path& layout_path);

        /**
         * Destructor
         */
        ~SDL_App() override;

        /**
         * @brief Initialize SDL application
         * @return true if initialization succeeded, false otherwise
         */
        bool init() override;

        /**
         * @brief Run the main event loop
         */
        void run() override;

        /**
         * @brief Check if the application should quit
         * @return true if application should quit, false otherwise
         */
        bool       should_quit() const override;
        I_Display& get_display()  override;

        /**
         * @brief Handle a key press event
         * @param key_index Index of the pressed key
         */
        void handle_key(int key_index);

    private:

        /**
         * @brief Private constructor - use create() factory method
         * @param layout Keyboard layout reference
         */
        explicit SDL_App(const core::Grid_Layout& layout);

        /// @brief Callback function for key clicks
        static void on_key_clicked(int key_index, void* user_data);

        /// @brief Reference to the keyboard layout
        const core::Grid_Layout&          m_layout;

        /// @brief Path to the layout file
        std::filesystem::path             m_layout_path;

        /// @brief Flag indicating if the application has been initialized
        bool                              m_initialized = false;
        bool                              m_should_quit = false;

        /// @brief Keymap for the calculator
        core::Keymap m_keymap;

        /// @brief Layer manager for the calculator
        core::Layer_Manager m_layers;

        /// @brief Calculator engine for mathematical operations
        math::Calc_Engine m_engine;

        /// @brief SDL window driver
        std::unique_ptr<Display>       m_display;

        /// @brief Separate keyboard window (PICOSDL only, when SHOW_KEYBOARD_UI=ON)
        std::unique_ptr<Keyboard_Window> m_keyboard_window;

        /// @brief Keyboard display widget (in separate window for PICOSDL)
        std::unique_ptr<gui::Keyboard_Display> m_keyboard_display;

        /// @brief LVGL application view (LCD + keyboard widgets)
        std::unique_ptr<gui::App_View> m_view;

        /// @brief SDL input for the calculator
        std::unique_ptr<SDL_Input>   m_input;

        /// @brief SDL keymap for the calculator
        SDL_Keymap m_sdl_keymap;

};

} // namespace ovb::hal::sdl
