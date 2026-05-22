/**
 * @file lvgl_keyboard_view.hpp
 * @author Marvin Smith
 * @date 2026-05-20
 * @brief LVGL widget-based keyboard view
 *
 * Creates and manages a grid of lv_button widgets representing the
 * physical keyboard layout. Key labels, layer state, and press
 * highlighting are driven by the Layer_Manager and Calc_Engine state.
 */
#pragma once

// C++ Standard Libraries
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/layer_manager.hpp>

namespace ovb::hal::sdl {

/**
 * @brief LVGL widget keyboard view
 *
 * Owns one lv_button + lv_label per key, laid out using pixel
 * coordinates derived from Grid_Layout. Call update() after any
 * layer or press state change to synchronise widget labels/styles.
 */
class LVGL_Keyboard_View {
    public:
        /**
         * @brief Create keyboard view on an existing LVGL screen
         * @param parent    LVGL parent object (typically lv_screen_active())
         * @param layout    Grid layout defining key positions and spans
         * @param layers    Layer manager for current layer key definitions
         * @param width     Display width in pixels
         * @param height    Display height in pixels
         */
        LVGL_Keyboard_View( lv_obj_t*                     parent,
                            const ovb::core::Grid_Layout& layout,
                            const core::Layer_Manager&    layers,
                            int                           width,
                            int                           height );

        /// @brief Rebuild all button labels to match the current layer
        void update_layer();

        /**
         * @brief Highlight a key as pressed
         * @param key_index Key to highlight (-1 to clear all)
         */
        void set_pressed(int key_index);

        /// @brief Clear all press highlights
        void clear_pressed();

    private:
        const core::Grid_Layout&   m_layout;
        const core::Layer_Manager& m_layers;
        int                        m_width;
        int                        m_height;
        int                        m_pressed_key = -1;

        lv_obj_t*              m_container = nullptr;
        std::vector<lv_obj_t*> m_buttons;   ///< One entry per key index
        std::vector<lv_obj_t*> m_labels;    ///< Matching label per button

        static constexpr int KEY_PAD    = 4;
        static constexpr int HEADER_H   = 26;
        static constexpr int MARGIN_LEFT = 20;
        static constexpr int MARGIN_TOP  = 16;

        /// @brief Build all button widgets from current layout
        void build_buttons(lv_obj_t* parent);

        /// @brief Apply normal/pressed style to a single button
        void apply_style(int key_index, bool pressed);
};

} // namespace ovb::hal::sdl
