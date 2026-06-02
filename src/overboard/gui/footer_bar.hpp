/**
 * @file    footer_bar.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   LVGL footer bar — 5 labelled function-key slots
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/i_footer_bar.hpp>

namespace ovb::gui {

/**
 * @brief LVGL-backed footer bar with 5 clickable F-key slots
 *
 * Renders at the bottom of the panel container. Each slot is a
 * button labelled Fx and showing a caller-supplied subtitle.
 * Clicking any slot fires the registered Slot_Cb with the 0-based index.
 */
class Footer_Bar : public I_Footer_Bar {

    public:

        /// @brief Height of the footer bar
        static constexpr int HEIGHT { 28 };

        /**
         * @brief Construct and build the footer bar
         * @param parent LVGL parent object
         * @param width  Width in pixels
         */
        Footer_Bar(lv_obj_t* parent, int width);

        /**
         * @brief Destructor
         */
        ~Footer_Bar() override;

        /**
         * @brief Set the label for a specific slot
         * @param slot  Slot index (0-4)
         * @param label Label text to display
         */
        void set_label(int slot, const std::string& label) override;

        /**
         * @brief Set the click callback for slot events
         * @param cb Callback function to invoke on slot click
         */
        void set_click_callback(Slot_Cb cb)                override;

        /**
         * @brief Refresh the footer bar display
         */
        void refresh()                                     override;

    private:

        static void slot_event_cb(lv_event_t* e);

        /// @brief Implementation details
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
