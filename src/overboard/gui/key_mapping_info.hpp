/**
 * @file    key_mapping_info.hpp
 * @author  Marvin Smith
 * @date    2026-06-03
 *
 * @brief   Key Mapping Info Panel
 *
 * Displays current layer key mappings in an informational panel.
 * Used in the main window when the keyboard is in a separate window.
 * Non-interactive — purely informational for macropad users.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/core/layer_manager.hpp>

namespace ovb::gui {

/**
 * @brief Key mapping information panel
 *
 * Renders current layer key mappings in a grid layout.
 * Non-interactive — purely informational display.
 * Designed for macropad users who have physical keys locked to one layer
 * but need to see the dynamic key mapping.
 */
class Key_Mapping_Info {

    public:

        static constexpr int KEY_PAD     = 2;   ///< Gap between keys
        static constexpr int HEADER_H    = 20;  ///< Header height
        static constexpr int MARGIN_LEFT   = 8;   ///< Left margin
        static constexpr int MARGIN_TOP    = 6;   ///< Top margin (below header)

        /**
         * @brief Create key mapping info panel
         * @param parent           LVGL parent object
         * @param layout           Grid layout defining key positions
         * @param layers           Layer manager for current layer key definitions
         * @param width            Display width in pixels
         * @param height           Display height in pixels
         * @param get_label_cb     Optional callback to get custom label from active panel
         */
        Key_Mapping_Info( lv_obj_t*                      parent,
                         const ovb::core::Grid_Layout& layout,
                         const core::Layer_Manager&    layers,
                         int                           width,
                         int                           height,
                         std::function<std::string(int)> get_label_cb = nullptr );

        /**
         * @brief Update display to match current layer
         */
        void update_layer();

        /**
         * @brief Get the container object for positioning
         * @return LVGL container object
         */
        lv_obj_t* container() const { return m_container; }

    private:

        /// @brief Grid layout defining key positions
        const core::Grid_Layout&   m_layout;

        /// @brief Layer manager for current layer key definitions
        const core::Layer_Manager& m_layers;

        /// @brief Display width in pixels
        int                        m_width;

        /// @brief Display height in pixels
        int                        m_height;

        /// @brief Container object for all elements
        lv_obj_t*              m_container = nullptr;

        /// @brief Header label showing current layer name
        lv_obj_t*              m_header_label = nullptr;

        /// @brief Vector of label objects, one per key
        std::vector<lv_obj_t*> m_key_labels;

        /// @brief Vector of rectangle objects, one per key
        std::vector<lv_obj_t*> m_key_rects;

        /// @brief Callback to get custom label from active panel
        std::function<std::string(int)> m_get_label_cb;

        /// @brief Build all key display elements from current layout
        void build_keys(lv_obj_t* parent);

        /// @brief Get display text for a key, checking custom label callback first
        std::string get_key_label(int key_index, core::Action_Code action_code) const;
};

} // namespace ovb::gui
