/**
 * @file    app.hpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Pi Zero DRM application implementation
 *
 * Implements I_App for the Pi Zero target using direct DRM/KMS display
 * and Linux input events. Runs on the console without X11.
 */
#pragma once

// C++ Standard Libraries
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <memory>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/i_app.hpp>
#include <overboard/hal/pi_zero/display_drm.hpp>
#include <overboard/hal/pi_zero/input_linux.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::hal::pi_zero {

/**
 * @brief Pi Zero DRM application implementation
 *
 * Manages the Linux DRM lifecycle for direct HDMI output. Creates the display,
 * handles the main event loop via Linux input, and routes key events to the
 * calculator engine.
 */
class PiZero_App : public I_App {

    public:

        /// @brief Display width
        static constexpr int DISPLAY_WIDTH = ovb::hal::FULL_WIDTH;

        /// @brief Display height
        static constexpr int DISPLAY_HEIGHT = ovb::hal::FULL_HEIGHT;

        /**
         * @brief Factory method to create and initialize Pi Zero application
         * @param layout Keyboard layout for the target device
         * @param layout_path Path to keyboard.json file
         * @return Unique pointer to initialized PiZero_App, or nullptr on failure
         */
        static std::unique_ptr<PiZero_App> create(const core::Grid_Layout& layout,
                                                  const std::filesystem::path& layout_path);

        /**
         * Destructor
         */
        ~PiZero_App() override;

        /**
         * @brief Initialize Pi Zero application
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
         * @brief Get the input interface
         * @return Reference to the Linux input handler
         */
        I_Input& get_input() override;

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
        explicit PiZero_App(const core::Grid_Layout& layout);

        /// @brief Callback function for key clicks
        static void on_key_clicked(int key_index, void* user_data);

        /// @brief Reference to the keyboard layout
        const core::Grid_Layout&          m_layout;

        /// @brief Path to the layout file
        std::filesystem::path             m_layout_path;

        /// @brief Flag indicating if the application has been initialized
        bool                              m_initialized = false;
        std::atomic<bool>                 m_should_quit = false;

        /// @brief Keymap for the calculator
        core::Keymap m_keymap;

        /// @brief Layer manager for the calculator
        core::Layer_Manager m_layers;

        /// @brief Calculator engine for mathematical operations
        math::Calc_Engine m_engine;

        /// @brief DRM display driver
        std::unique_ptr<Display_DRM>      m_display;

        /// @brief LVGL application view (LCD + key mapping info panel)
        std::unique_ptr<gui::App_View>    m_view;

        /// @brief Linux input handler
        std::unique_ptr<Linux_Input>      m_input;

};

} // namespace ovb::hal::pi_zero
