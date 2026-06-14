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
#include <functional>
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_app.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::gui {

// Number of F-key popup slots (F1-F10)
inline constexpr int F_KEY_POPUP_COUNT = 10;

/**
 * @brief Popup menu indices for F-key assignments
 */
enum class Popup_Menu : int {
    Alg      = 0,  // F1: Algebraic operations (reciprocal, square, power, sqrt)
    Trig     = 1,  // F2: Trigonometric functions (sin, cos, tan, etc.)
    Calc     = 2,  // F3: Available for future use
    Memory   = 3,  // F4: Available for future use
    Advanced = 4,  // F5: Available for future use
    // F6-F10: Reserved for future menus
};

/**
 * @brief Calculator application panel
 *
 * Owns an LCD_Section and routes Action_Code inputs to the Calc_Engine.
 * Layer-switch actions are handled by updating the shared Layer_Manager.
 */
class Calculator_App : public I_App {
    public:

        using Back_Cb = std::function<void()>;

        /**
         * @brief Construct the calculator panel
         * @param engine   Shared calculation engine
         * @param layers   Shared layer manager for key lookups
         * @param on_back  Optional callback fired when user navigates back (F1)
         */
        Calculator_App(math::Calc_Engine& engine,
                       core::Layer_Manager& layers,
                       Back_Cb on_back = {});

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
         * @return "F1"-"F4" for keys 3-6, empty for others
         */
        std::string get_custom_label(int key_index) const override;

    private:

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
