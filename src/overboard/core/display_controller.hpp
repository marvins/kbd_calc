/**
 * @file display_controller.hpp
 * @brief Display controller for calculator UI
 */
#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/core/point.hpp>
#include <overboard/core/rect.hpp>
#include <overboard/hal/i_display.hpp>
#include <overboard/math/calc_engine.hpp>
#include <overboard/math/layout/engine.hpp>
#include <overboard/font/font_metrics.hpp>

namespace ovb::core {

/**
 * @brief Main display coordinator for calculator UI
 *
 * Manages rendering of both the virtual keyboard and LCD display.
 * Handles keyboard layers, key press visualization, history display,
 * and math layout preview. Integrates with Calc_Engine for state
 * and Layout_Engine for math expression typesetting.
 *
 * Supports configurable keyboard layouts via Grid_Layout for asymmetric
 * layouts like the Womier SK30 with split sections and multi-cell keys.
 */
class Display_Controller {

    public:
        /**
         * @brief Construct display controller with default layout
         * @param kbd_display Keyboard window display interface
         * @param lcd_display Calculator LCD display interface
         * @param layers Layer manager for keyboard layout
         * @param engine Calculation engine for state and expressions
         */
        Display_Controller( I_Display&               kbd_display,
                            I_Display&               lcd_display,
                            const Layer_Manager&     layers,
                            const math::Calc_Engine& engine );

        /**
         * @brief Construct display controller with custom layout
         * @param kbd_display Keyboard window display interface
         * @param lcd_display Calculator LCD display interface
         * @param layers Layer manager for keyboard layout
         * @param engine Calculation engine for state and expressions
         * @param layout Custom grid layout for key positioning
         */
        Display_Controller( I_Display&               kbd_display,
                            I_Display&               lcd_display,
                            const Layer_Manager&     layers,
                            const math::Calc_Engine& engine,
                            Grid_Layout              layout );

        ~Display_Controller() = default;

        /// @brief Render both keyboard and LCD displays
        void render();

        /// @brief Highlight key at given index as pressed
        void set_pressed_key(int key_index);

        /// @brief Clear pressed key highlight
        void clear_pressed_key();

    private:

        I_Display&            m_kbd_display;       ///< Keyboard display interface
        I_Display&            m_lcd_display;       ///< LCD display interface
        const Layer_Manager&  m_layers;            ///< Keymap layer manager
        const math::Calc_Engine& m_engine;         ///< Calculator state engine
        Grid_Layout           m_layout;            ///< Keyboard grid layout
        int                   m_pressed_key = -1;  ///< Currently pressed key index (-1 if none)
        layout::Layout_Engine m_layout_engine{font::Font_Metrics::make_default(), 2};  ///< Math layout engine (scale 2)

        static constexpr int KEY_PAD    = 4;          ///< Padding between keys
        static constexpr int MARGIN_LEFT = 20;        ///< Left margin for row labels
        static constexpr int MARGIN_TOP  = 16;        ///< Top margin for column labels

        /// @brief Render virtual keyboard with current layer
        void render_keyboard();

        /// @brief Render LCD calculator display
        void render_lcd();

        /**
         * @brief Draw single key button at grid position
         * @param code Key code
         * @param rect Key rectangle in pixels
         * @param pressed Whether key is currently pressed (for highlight)
         */
        void draw_key( Key_Code        code,
                       core::Rect<int> rect,
                       bool            pressed );

        /**
         * @brief Draw mode indicator panel (MATH/LINE)
         * @param pos Panel position
         */
        void draw_mode_panel(Point<int> pos);

        /**
         * @brief Render expression using math layout engine
         * @param pos Starting position
         * @param max_size Maximum dimensions available
         */
        void render_math_expression(Point<int> pos, Point<int> max_size);
};

} // namespace ovb::core
