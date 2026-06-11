/**
 * @file    i_panel.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Abstract interface for application panels
 *
 * A panel owns a region of the display and handles input for one
 * application context (e.g. Calculator, Status Page, App Menu).
 * Panel_Manager owns and routes between panels.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/core/input_key.hpp>

namespace ovb::gui {

/**
 * @brief Application panel interface
 *
 * Each concrete panel builds its widgets onto the provided LVGL parent
 * in activate(), tears them down in deactivate(), and processes
 * Action_Code inputs via handle_input().
 */
class I_Panel {
    public:

        virtual ~I_Panel() = default;

        /// @brief Build widgets onto parent and make panel visible
        virtual void activate(lv_obj_t* parent) = 0;

        /// @brief Remove all widgets and release resources
        virtual void deactivate() = 0;

        /**
         * @brief Process a single action from the input system
         * @param action The action to handle
         * @return true if the action was consumed, false to pass up
         */
        virtual bool handle_input(core::Action_Code action) = 0;

        /**
         * @brief Receive a resolved text codepoint (UTF-32)
         *
         * Called when a key press did not map to any Action_Code and the
         * platform resolved it to a printable character.  The default
         * no-op means panels that do not need text input can ignore this.
         *
         * @param codepoint UTF-32 character value
         * @return true if consumed
         */
        virtual bool handle_text([[maybe_unused]] char32_t codepoint) { return false; }

        /**
         * @brief Handle context-dependent Input_Key
         *
         * Called for keys that have no direct Action_Code mapping (like RETURN, TAB).
         * Panels can decide how to interpret these keys based on their context.
         * The default implementation returns false (key not handled).
         *
         * @param key Input key to handle
         * @return true if consumed
         */
        virtual bool handle_input_key([[maybe_unused]] core::Input_Key key) { return false; }

        /// @brief Refresh display contents from current state
        virtual void refresh() = 0;

        /// @brief Short human-readable panel name
        virtual std::string name() const = 0;

        /**
         * @brief Get custom label for a key in this panel context
         * @param key_index The key index (0-33 for MF layout)
         * @return Custom label string, or empty to use default from keyboard.json
         */
        virtual std::string get_custom_label([[maybe_unused]] int key_index) const { return ""; }
};

} // namespace ovb::gui
