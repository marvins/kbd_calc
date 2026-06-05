/**
 * @file    display_drm.hpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Linux DRM/KMS display driver
 *
 * Creates and owns the DRM display and LVGL display handle.
 * Platform HAL responsibility only — no widget management.
 * Consumers call screen() to get the LVGL root for GUI attachment.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string>

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::hal::pi_zero {

/**
 * @brief Linux DRM/KMS window driver
 *
 * Owns the DRM display and LVGL display handle. Exposes the LVGL
 * active screen so the GUI layer can attach widgets to it.
 */
class Display_DRM {

    public:

        /**
         * @brief Create the DRM display and initialise LVGL
         *
         * @param width  Total display width in pixels
         * @param height Total display height in pixels
         */
        Display_DRM(int width, int height);

        /**
         * @brief Destroy the Display_DRM object
         */
        virtual ~Display_DRM();

        /// @brief LVGL active screen — attach GUI widgets here
        lv_obj_t* screen() const;

        /// @brief Trigger a display refresh
        void refresh();

    private:

        /// @brief PImpl (Pointer to Implementation) structure
        struct Impl;

        /// @brief Pointer to implementation
        std::unique_ptr<Impl> m_impl;
};

} // namespace ovb::hal::pi_zero
