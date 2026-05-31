/**
 * @file    keyboard_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Simple keyboard layout display (non-interactive)
 *
 * Renders the keyboard layout as a static visual display showing
 * current layer key mappings. Uses simple LVGL graphics primitives
 * (rectangles, text) instead of interactive button widgets.
 * Physical keyboard handles all input.
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
#include <overboard/hal/display_config.hpp>

namespace ovb::gui {

/**
 * @brief Simple keyboard layout display
 *
 * Renders a static grid showing keyboard layout and current layer
 * key mappings. No interactivity - physical keyboard handles input.
 */
class Keyboard_Display {

    public:

        static constexpr int KEY_PAD     = ovb::hal::KEY_PAD;      ///< Gap between keys
        static constexpr int HEADER_H    = ovb::hal::KBD_HEADER_H; ///< Keyboard header height
        static constexpr int MARGIN_LEFT = ovb::hal::KBD_MARGIN_L; ///< Left margin
        static constexpr int MARGIN_TOP  = ovb::hal::KBD_MARGIN_T; ///< Top margin (below header)

        /**
         * @brief Create keyboard display on an existing LVGL screen
         * @param parent    LVGL parent object (typically lv_screen_active())
         * @param layout    Grid layout defining key positions and spans
         * @param layers    Layer manager for current layer key definitions
         * @param width     Display width in pixels
         * @param height    Display height in pixels
         */
        Keyboard_Display( lv_obj_t*                     parent,
                         const ovb::core::Grid_Layout& layout,
                         const core::Layer_Manager&    layers,
                         int                           width,
                         int                           height );

        /**
         * @brief Update display to match current layer
         */
        void update_layer();

    private:

        /// @brief Grid layout defining key positions and spans
        const core::Grid_Layout&   m_layout;

        /// @brief Layer manager for current layer key definitions
        const core::Layer_Manager& m_layers;

        /// @brief Display width in pixels
        int                        m_width;

        /// @brief Display height in pixels
        int                        m_height;

        /// @brief Container object for all keyboard elements
        lv_obj_t*              m_container = nullptr;

        /// @brief Header label showing current layer name
        lv_obj_t*              m_header_label = nullptr;

        /// @brief Vector of label objects, one per key
        std::vector<lv_obj_t*> m_key_labels;

        /// @brief Vector of rectangle objects, one per key
        std::vector<lv_obj_t*> m_key_rects;

        /// @brief Build all key display elements from current layout
        void build_keys(lv_obj_t* parent);
};

} // namespace ovb::gui
