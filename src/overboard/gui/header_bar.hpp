/**
 * @file    header_bar.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   LVGL header bar — shows active panel name
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/gui/i_header_bar.hpp>

namespace ovb::gui {

/**
 * @brief LVGL-backed header bar
 *
 * Renders a thin bar at the top of the panel container displaying
 * the active panel name left-aligned.
 */
class Header_Bar : public I_Header_Bar {

    public:

        static constexpr int HEIGHT { 20 };

        /**
         * @brief Construct and build the header bar
         * @param parent LVGL parent object
         * @param width  Width in pixels
         */
        Header_Bar(lv_obj_t* parent, int width);

        /**
         * @brief Destructor
         */
        ~Header_Bar() override;

        /**
         * @brief Set the application name to display
         * @param name Application name
         */
        void set_app_name(const std::string& name) override;

        /**
         * @brief Refresh the header bar
         */
        void refresh() override;

        /**
         * @brief Get the underlying LVGL object
         * @return LVGL object pointer
         */
        lv_obj_t* get_obj() const override;

    private:

        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::gui
