/**
 * @file   sdl_keyboard.hpp
 * @author Marvin Smith
 * @date   2026-05-21
 *
 * @brief  SDL keyboard manager — window + widget management
 *
 * Encapsulates the keyboard UI: SDL window creation,
 * widget tree management (buttons, labels), and layer/press state.
 * Independent of Display_Controller; main coordinates between
 * this keyboard manager and the LCD display controller.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/i_display.hpp>
#include <overboard/hal/sdl/sdl_keyboard_display.hpp>
#include <overboard/hal/sdl/sdl_keyboard_view.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL keyboard manager
 *
 * Owns the keyboard window and all LVGL widgets.
 * Provides high-level interface for layer switching
 * and key press visualization.
 */
class SDL_Keyboard {
    public:
        /**
         * @brief Create LVGL keyboard window with widget layout
         * @param title     Window title
         * @param width     Window width in pixels
         * @param height    Window height in pixels
         * @param layout    Grid layout for key positioning
         * @param layers    Layer manager for key labels
         */
        SDL_Keyboard( const std::string&            title,
                       int                           width,
                       int                           height,
                       const ovb::core::Grid_Layout& layout,
                       const core::Layer_Manager&    layers );

        /**
         * Destructor
         */
        virtual ~SDL_Keyboard() = default;

        /// @brief Window dimensions
        int  width()  const;
        int  height() const;

        /// @brief SDL window ID for event filtering
        uint32_t window_id() const;

        /// @brief Get the display interface
        I_Display& get_display();

        /// @brief Get the concrete keyboard display for LVGL callback wiring
        SDL_Keyboard_Display& get_keyboard_display();

        /// @brief Refresh key labels from current layer
        void update_layer();

        /// @brief Access the keyboard view for callback wiring
        SDL_Keyboard_View& view();

        /// @brief Drive LVGL render (call in main loop)
        void render();

    private:
        std::unique_ptr<SDL_Keyboard_Display> m_display;
        std::unique_ptr<SDL_Keyboard_View>    m_view;
};

} // namespace ovb::hal::sdl
