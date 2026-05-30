/**
 * @file    app.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   PicoCalc hardware application implementation
 *
 * Implements I_App for the ClockworkPi PicoCalc (RP2040/RP2350).
 * Owns the ILI9488 display driver, the I2C keyboard input driver,
 * and the LVGL GUI view. The main loop drives the LVGL tick and
 * processes key events.
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <memory>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/hal/i_app.hpp>
#include <overboard/hal/picocalc/display.hpp>
#include <overboard/hal/picocalc/input.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::hal::picocalc {

/**
 * @brief PicoCalc hardware application
 *
 * Manages the full embedded lifecycle:
 *  - Initialises the ILI9488 SPI display via LVGL
 *  - Initialises the I2C keyboard controller
 *  - Creates the GUI App_View on the LVGL screen
 *  - Runs the main loop (LVGL tick + key dispatch)
 */
class PicoCalc_App : public I_App {

    public:

        /// @brief Display dimensions (physical pixels)
        static constexpr int DISPLAY_WIDTH  = LCD_PHYS_WIDTH;
        static constexpr int DISPLAY_HEIGHT = LCD_PHYS_HEIGHT;

        /**
         * @brief Factory method — loads keymap/layers and initialises hardware
         *
         * @param layout       Key grid geometry
         * @param layout_path  Path to VIA layout JSON
         * @param keymap_path  Path to scancode mapping JSON
         * @param layers_path  Path to layer definitions JSON
         * @return Initialised PicoCalc_App, or nullptr on failure
         */
        static std::unique_ptr<PicoCalc_App> create(
            const core::Grid_Layout&     layout,
            const std::filesystem::path& layout_path,
            const std::filesystem::path& keymap_path,
            const std::filesystem::path& layers_path );

        ~PicoCalc_App() override;

        bool       init()        override;
        void       run()         override;
        bool       should_quit() const override;
        I_Display& get_display() override;

        /// @brief Handle a logical key press (public for callback use)
        void handle_key( int key_index );

    private:

        explicit PicoCalc_App( const core::Grid_Layout& layout );

        static void on_key_clicked( int key_index, void* user_data );

        const core::Grid_Layout&  m_layout;
        std::filesystem::path     m_layout_path;
        std::filesystem::path     m_keymap_path;
        std::filesystem::path     m_layers_path;

        bool m_initialized  = false;
        bool m_should_quit  = false;

        core::Keymap        m_keymap;
        core::Layer_Manager m_layers;
        math::Calc_Engine   m_engine;

        std::unique_ptr<PicoCalc_Display> m_display;
        std::unique_ptr<gui::App_View>    m_view;
        std::unique_ptr<PicoCalc_Input>   m_input;
};

} // namespace ovb::hal::picocalc
