/**
 * @file    sdl_keyboard_view.hpp
 * @author  Marvin Smith
 * @date    2026-05-20
 *
 * @brief   SDL widget-based keyboard view
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
 * @brief SDL widget keyboard view
 *
 * Owns one lv_button + lv_label per key, laid out using pixel
 * coordinates derived from Grid_Layout. Call update() after any
 * layer or press state change to synchronise widget labels/styles.
 */
class SDL_Keyboard_View {
    public:
        /**
         * @brief Create keyboard view on an existing LVGL screen
         * @param parent    LVGL parent object (typically lv_screen_active())
         * @param layout    Grid layout defining key positions and spans
         * @param layers    Layer manager for current layer key definitions
         * @param width     Display width in pixels
         * @param height    Display height in pixels
         */
        SDL_Keyboard_View( lv_obj_t*                     parent,
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
        const core::Grid_Layout&   m_layout;
        const core::Layer_Manager& m_layers;
        int                        m_width;
        int                        m_height;

        lv_obj_t*              m_container = nullptr;
        std::vector<lv_obj_t*> m_buttons;   ///< One entry per key index (sparse)
        std::vector<lv_obj_t*> m_labels;    ///< Matching label per button
        std::vector<int>       m_key_indices; ///< key_index for each built button

        void (*m_click_cb)(int key_index, void* user_data) = nullptr;
        void*  m_click_user_data = nullptr;

        static constexpr int KEY_PAD     = 4;
        static constexpr int HEADER_H    = 26;
        static constexpr int MARGIN_LEFT = 20;
        static constexpr int MARGIN_TOP  = 16;

        /// @brief Build all button widgets from current layout
        void build_buttons(lv_obj_t* parent);

        /// @brief LVGL event callback (pressed, released, clicked)
        static void button_event_cb(lv_event_t* e);
};

} // namespace ovb::hal::sdl
