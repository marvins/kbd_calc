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
#include <functional>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/hal/display_config.hpp>

namespace ovb::gui {

/**
 * @brief Overlay key for popup hotkey display
 */
struct Overlay_Key {
    int key_index;                  ///< Physical key position
    std::string label;              ///< Display text (math notation)
    core::Action_Code action;       ///< What it does
};

/**
 * @brief Simple keyboard layout display
 *
 * Renders a grid showing keyboard layout and current layer key mappings.
 * When a click callback is registered the buttons become interactive;
 * each click fires the callback with the logical key index.
 */
class Keyboard_Display {

    public:

        static constexpr int KEY_PAD     = ovb::hal::KEY_PAD;      ///< Gap between keys
        static constexpr int HEADER_H    = ovb::hal::KBD_HEADER_H; ///< Keyboard header height
        static constexpr int MARGIN_LEFT = ovb::hal::KBD_MARGIN_L; ///< Left margin
        static constexpr int MARGIN_TOP  = ovb::hal::KBD_MARGIN_T; ///< Top margin (below header)

        /// @brief Callback invoked when a key button is clicked; argument is the key index
        using Click_Callback = std::function<void(int key_index)>;

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
         * @brief Register a callback invoked on every button click
         *
         * Must be called after construction. Retroactively enables
         * LV_OBJ_FLAG_CLICKABLE on all key widgets and attaches the handler.
         *
         * @param cb Callable receiving the logical key index
         */
        void set_click_callback(Click_Callback cb);

        /**
         * @brief Update display to match current layer
         */
        void update_layer();
        
        /**
         * @brief Enable overlay mode (popup hotkeys)
         * @param keys Overlay keys to display
         */
        void set_overlay_mode(const std::vector<Overlay_Key>& keys);
        
        /**
         * @brief Clear overlay mode and return to normal layer
         */
        void clear_overlay_mode();
        
        /**
         * @brief Check if overlay mode is active
         */
        bool is_overlay_active() const;

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

        /// @brief Click callback (null if not interactive)
        Click_Callback m_click_cb;
        
        /// @brief Overlay mode state
        bool m_overlay_active = false;
        
        /// @brief Overlay keys (popup hotkeys)
        std::vector<Overlay_Key> m_overlay_keys;

        /// @brief LVGL event handler forwarded to m_click_cb
        static void on_btn_clicked(lv_event_t* e);

        /// @brief Build all key display elements from current layout
        void build_keys(lv_obj_t* parent);
};

} // namespace ovb::gui
