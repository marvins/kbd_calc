/**
 * @file layer_manager.hpp
 * @author Marvin Smith
 * @date 2026-05-21
 * @brief Keyboard layer state manager
 *
 * Tracks the active layer index and provides access to key definitions
 * from the current layer. Part of the core keyboard logic module.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <vector>

// Project Libraries
#include <overboard/core/keymap.hpp>

namespace ovb::core {

/**
 * @brief Manages keyboard layer state
 *
 * Wraps a Keymap and tracks which layer is currently active.
 * Provides navigation (next/prev layer) and direct layer selection.
 * All key lookups are delegated to the underlying Keymap using
 * the active layer index.
 */
class Layer_Manager {

    public:

        using Layer_Change_Cb = std::function<void(int layer_index)>;

        /**
         * @brief Construct layer manager with keymap
         * @param keymap Reference to keymap defining all layers
         */
        explicit Layer_Manager(const Keymap& keymap);

        /**
         * @brief Register a callback to be invoked on any layer change
         * @param cb Callback receiving the new layer index
         */
        void on_layer_change(Layer_Change_Cb cb);

        /// @brief Get current active layer index
        int            active_layer() const;

        /// @brief Get total number of layers
        int            layer_count() const;

        /// @brief Switch to next layer (wraps around)
        void           next_layer();

        /// @brief Switch to previous layer (wraps around)
        void           prev_layer();

        /**
         * @brief Jump directly to a specific layer
         * @param index Layer index to activate
         */
        void           set_layer(int index);

        /**
         * @brief Get the currently active layer definition
         * @return Reference to the current layer
         */
        const Layer&   current_layer() const;

        /**
         * @brief Get action code at index from current layer
         * @param key_index Key position in the layout grid
         * @return Action code for the specified key
         */
        Action_Code action_at(int key_index) const;

    private:

        void notify_layer_change();

        /// @brief Reference to keymap with all layers
        const Keymap&             m_keymap;

        /// @brief Currently active layer index
        int                       m_active_layer;

        /// @brief Registered layer-change callbacks
        std::vector<Layer_Change_Cb> m_layer_change_callbacks;
};

} // namespace ovb::core
