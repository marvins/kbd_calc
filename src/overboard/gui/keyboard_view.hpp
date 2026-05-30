/**
 * @file    keyboard_view.hpp
 * @author  Marvin Smith
 * @date    2026-05-20
 *
 * @brief   LVGL widget-based keyboard view
 *
 * Creates and manages a grid of lv_button widgets representing the
 * physical keyboard layout. Key labels, layer state, and press
 * highlighting are driven by the Layer_Manager and Calc_Engine state.
 * Platform-agnostic — works on both SDL simulator and embedded targets.
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
 * @brief LVGL widget keyboard view
 *
 * Owns one lv_button + lv_label per key, laid out using pixel
 * coordinates derived from Grid_Layout. Call update() after any
 * layer or press state change to synchronise widget labels/styles.
 * Platform-agnostic — works on both SDL simulator and embedded targets.
 */
class Keyboard_View {

    public:

        static constexpr int KEY_PAD     = ovb::hal::KEY_PAD;      ///< Gap between keys
        static constexpr int HEADER_H    = ovb::hal::KBD_HEADER_H; ///< Keyboard header height
        static constexpr int MARGIN_LEFT = ovb::hal::KBD_MARGIN_L; ///< Left margin
        static constexpr int MARGIN_TOP  = ovb::hal::KBD_MARGIN_T; ///< Top margin (below header)

        /**
         * @brief Create keyboard view on an existing LVGL screen
         * @param parent    LVGL parent object (typically lv_screen_active())
         * @param layout    Grid layout defining key positions and spans
         * @param layers    Layer manager for current layer key definitions
         * @param width     Display width in pixels
         * @param height    Display height in pixels
         */
        Keyboard_View( lv_obj_t*                     parent,
                       const ovb::core::Grid_Layout& layout,
                       const core::Layer_Manager&    layers,
                       int                           width,
                       int                           height );

        /// @brief Rebuild all button labels to match the current layer
        void update_layer();

        /**
         * @brief Set callback invoked when a key button is clicked
         * @param cb        Function to call with the key index
         * @param user_data Passed through to the callback
         */
        void set_click_callback( void (*cb)(int key_index, void* user_data),
                                 void* user_data );

    private:

        /// @brief Grid layout defining key positions and spans
        const core::Grid_Layout&   m_layout;

        /// @brief Layer manager for current layer key definitions
        const core::Layer_Manager& m_layers;

        /// @brief Display width in pixels
        int                        m_width;

        /// @brief Display height in pixels
        int                        m_height;

        /// @brief Container object for all keyboard buttons
        lv_obj_t*              m_container = nullptr;

        /// @brief Vector of button objects, one per key index (sparse)
        std::vector<lv_obj_t*> m_buttons;   ///< One entry per key index (sparse)

        /// @brief Vector of label objects, one per button
        std::vector<lv_obj_t*> m_labels;    ///< Matching label per button

        /// @brief Vector of key indices, one per built button
        std::vector<int>       m_key_indices; ///< key_index for each built button

        /// @brief Click callback function
        void (*m_click_cb)(int key_index, void* user_data) = nullptr;

        /// @brief User data for click callback
        void*  m_click_user_data = nullptr;

        /// @brief Build all button widgets from current layout
        void build_buttons(lv_obj_t* parent);

        /// @brief LVGL event callback (pressed, released, clicked)
        static void button_event_cb(lv_event_t* e);
};

} // namespace ovb::gui
