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
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/footer_bar.hpp>
#include <overboard/gui/header_bar.hpp>
#include <overboard/gui/i_panel.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::gui {

/**
 * @brief Calculator application panel
 *
 * Owns an LCD_Section and routes Action_Code inputs to the Calc_Engine.
 * Layer-switch actions are handled by updating the shared Layer_Manager.
 */
class Calculator_App : public I_Panel {
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
         * @brief Refresh the calculator display
         */
        void        refresh()                  override;

        /**
         * @brief Get the panel name
         * @return Panel name as string
         */
        std::string name()   const             override;

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
