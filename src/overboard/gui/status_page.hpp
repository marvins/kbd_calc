/**
 * @file    status_page.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Status page panel — shown on boot
 *
 * Displays basic system information (build target, version, layer count)
 * and waits for any keypress to hand off to the Calculator_App panel.
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

namespace ovb::gui {

/**
 * @brief Boot status panel
 *
 * Shows system info on activation. Calls the provided on_dismiss
 * callback when the user presses any key, allowing Panel_Manager
 * to switch to the next panel.
 */
class Status_Page : public I_Panel {
    public:

        using Dismiss_Cb = std::function<void()>;

        /**
         * @brief Construct the status page
         * @param layers     Layer manager (used to show layer count)
         * @param on_dismiss Callback fired on first keypress
         */
        Status_Page(const core::Layer_Manager& layers, Dismiss_Cb on_dismiss);

        /**
         * @brief Destructor
         */
        ~Status_Page() override;

        /**
         * @brief Activate the status page
         * @param parent LVGL parent object
         */
        void activate(lv_obj_t* parent) override;

        /**
         * @brief Deactivate the status page
         */
        void deactivate() override;

        /**
         * @brief Handle input event
         * @param action Action code
         * @return true if consumed
         */
        bool handle_input(core::Action_Code action) override;

        /**
         * @brief Refresh the status page
         */
        void refresh() override;

        /**
         * @brief Get the name of the status page
         * @return Name string
         */
        std::string name() const override;

    private:

        /// @brief Implementation details
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
