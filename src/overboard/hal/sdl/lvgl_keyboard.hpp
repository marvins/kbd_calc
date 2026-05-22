/**
 * @file lvgl_keyboard.hpp
 * @author Marvin Smith
 * @date 2026-05-21
 * @brief LVGL keyboard manager — window + widget management
 *
 * Encapsulates the complete LVGL keyboard UI: SDL window creation,
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

// Forward declarations — implementation detail
namespace ovb::hal::sdl {
    class LVGL_Keyboard_Display;
    class LVGL_Keyboard_View;
}

namespace ovb::hal::sdl {

/**
 * @brief LVGL keyboard manager
 *
 * Owns the keyboard window and all LVGL widgets.
 * Provides high-level interface for layer switching
 * and key press visualization.
 */
class LVGL_Keyboard {
    public:
        /**
         * @brief Create LVGL keyboard window with widget layout
         * @param title     Window title
         * @param width     Window width in pixels
         * @param height    Window height in pixels
         * @param layout    Grid layout for key positioning
         * @param layers    Layer manager for key labels
         */
        LVGL_Keyboard( const std::string&            title,
                       int                           width,
                       int                           height,
                       const ovb::core::Grid_Layout& layout,
                       const core::Layer_Manager&    layers );

        ~LVGL_Keyboard();

        /// @brief Window dimensions
        int  width()  const;
        int  height() const;

        /// @brief SDL window ID for event filtering
        uint32_t window_id() const;

        /// @brief Refresh key labels from current layer
        void update_layer();

        /// @brief Highlight key as pressed (-1 to clear)
        void set_pressed(int key_index);

        /// @brief Clear all press highlights
        void clear_pressed();

        /// @brief Drive LVGL render (call in main loop)
        void render();

    private:
        std::unique_ptr<LVGL_Keyboard_Display> m_display;
        std::unique_ptr<LVGL_Keyboard_View>    m_view;
};

} // namespace ovb::hal::sdl
