/**
 * @file    calculator_app.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Calculator application panel
 *
 * Wraps LCD_Section and the Calc_Engine to form the main
 * calculator panel shown to the user. Implements I_Panel so it
 * integrates with Panel_Manager.
 */
#pragma once

// C++ Standard Libraries
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/core/settings_manager.hpp>
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/function_menu_popup.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_app.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::gui {

// Number of F-key slots per context (F1–F5)
inline constexpr int F_KEY_SLOT_COUNT  = 5;

// Total popup slots (F1–F10, two banks)
inline constexpr int F_KEY_POPUP_COUNT = 10;

/**
 * @brief One named context defining what F1–F5 do and their footer labels.
 *
 * Each context owns a list of menu items per slot. An empty item list means
 * the corresponding F-key has no popup (pressing it is a no-op).
 */
struct F_Key_Context {
    std::string                                        name;             ///< Display name (e.g. "Core Math")
    std::array<std::string, F_KEY_SLOT_COUNT>          labels;           ///< Footer labels for F1–F5
    std::array<std::vector<Function_Menu_Item>, F_KEY_SLOT_COUNT> slots; ///< Popup items per slot
};

/**
 * @brief Calculator application panel
 *
 * Owns an LCD_Section and routes Action_Code inputs to the Calc_Engine.
 * Layer-switch actions are handled by updating the shared Layer_Manager.
 */
class Calculator_App : public I_App {
    public:

        /// @brief Back callback
        using Back_Cb = std::function<void()>;

        /**
         * @brief Construct the calculator panel
         * @param engine   Shared calculation engine
         * @param layers   Shared layer manager for key lookups
         * @param on_back  Optional callback fired when user navigates back (F1)
         * @param settings Optional settings manager for runtime configuration
         */
        Calculator_App( math::Calc_Engine&                      engine,
                        core::Layer_Manager&                    layers,
                        hal::I_System_Info&                     system_info,
                        Back_Cb                                 on_back  = {},
                        std::shared_ptr<core::Settings_Manager> settings = nullptr );

        /**
         * @brief Set keyboard overlay callbacks for popup hotkey display
         */
        void set_overlay_callbacks(Overlay_Push_Cb push, Overlay_Pop_Cb pop) override;

        /**
         * @brief Destructor
         */
        ~Calculator_App() override;

        /**
         * @brief Activate the calculator panel
         * @param parent Parent LVGL object
         */
        void        activate(lv_obj_t* parent) override;

        /**
         * @brief Deactivate the calculator panel
         */
        void        deactivate()               override;

        /**
         * @brief Handle input action
         * @param action Action code to process
         * @return true if action was handled, false otherwise
         */
        bool        handle_input(core::Action_Code action) override;

        /**
         * @brief Handle context-dependent input keys
         * @param key Input key (e.g., RETURN means EVAL in calculator)
         * @return true if action was handled, false otherwise
         */
        bool        handle_input_key(core::Input_Key key) override;

        /**
         * @brief Handle text input (digits, operators from standard keyboard)
         * @param codepoint UTF-32 character
         * @return true if action was handled, false otherwise
         */
        bool        handle_text(char32_t codepoint) override;

        /**
         * @brief Refresh the calculator display
         */
        void        refresh()                  override;

        /**
         * @brief Get the panel name
         * @return Panel name as string
         */
        std::string name() const override { return "Calculator"; }

        /**
         * @brief Get LVGL menu icon symbol
         * @return LVGL symbol constant
         */
        const char* menu_icon() const override { return LV_SYMBOL_KEYBOARD; }

        /**
         * @brief Get menu display priority
         * @return Priority value (1 = second in menu after Status)
         */
        int menu_priority() const override { return 1; }

        /**
         * @brief Get menu mnemonic hotkey
         * @return 'c' for Calculator
         */
        char menu_hotkey() const override { return 'c'; }

        /**
         * @brief Get custom label for keys in calculator context
         * @param key_index The key index
         * @return popup name for F-keys (indices 4-8), empty for others
         */
        std::string get_custom_label(int key_index) const override;

    private:

        /// @brief Build overlay key descriptors mapping digit keys to popup items
        std::vector<I_Panel::Overlay_Key_Desc> build_popup_overlay(const Function_Menu_Popup& popup) const;

        /// @brief Advance the active context by @p delta steps (wraps)
        void cycle_context(int delta);

        /// @brief Apply the active context: rebuild popups and update footer labels
        void apply_context();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
