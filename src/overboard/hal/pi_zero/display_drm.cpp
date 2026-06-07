/**
 * @file    display_drm.cpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Linux DRM/KMS display implementation
 */
#include <overboard/hal/pi_zero/display_drm.hpp>

// C++ Standard Libraries
#include <stdexcept>

// Third-Party Libraries
#include <lvgl.h>

// Overboard Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pi_zero {

/**
 * @brief PImpl structure for Display_DRM
 */
struct Display_DRM::Impl {
    lv_display_t* lv_display = nullptr;
    lv_obj_t*     screen     = nullptr;
    int           width      = 0;
    int           height     = 0;
};

/*****************************/
/*        Constructor        */
/*****************************/
Display_DRM::Display_DRM(int width, int height)
    : m_impl(std::make_unique<Impl>()) {
    LOG_INFO("Display_DRM: Creating DRM display: ", width, " x ", height);

    m_impl->width  = width;
    m_impl->height = height;

    // Create the LVGL DRM display. The driver opens the device, selects the
    // connected connector's native mode, performs the KMS modeset, and installs
    // a monotonic tick source (lv_tick_set_cb).
    m_impl->lv_display = lv_linux_drm_create();
    if (!m_impl->lv_display) {
        throw std::runtime_error("Display_DRM: lv_linux_drm_create failed");
    }

    // Set rotation BEFORE lv_linux_drm_set_file so the driver detects it during
    // buffer setup and allocates a SW render buffer in FULL render mode. The
    // flush callback then copy-rotates the completed frame into the physical
    // DMA scanout buffer.
    lv_display_set_rotation(m_impl->lv_display, LV_DISPLAY_ROTATION_270);

    // -1 lets the driver auto-select the first connected connector.
    lv_result_t lv_res = lv_linux_drm_set_file(m_impl->lv_display, "/dev/dri/card0", -1);
    if (lv_res != LV_RESULT_OK) {
        lv_display_delete(m_impl->lv_display);
        throw std::runtime_error("Display_DRM: lv_linux_drm_set_file failed");
    }

    lv_display_set_default(m_impl->lv_display);

    m_impl->screen = lv_screen_active();

    LOG_INFO("Display_DRM: DRM display ready (",
             lv_display_get_horizontal_resolution(m_impl->lv_display), " x ",
             lv_display_get_vertical_resolution(m_impl->lv_display), ")");
}

/*****************************/
/*        Destructor         */
/*****************************/
Display_DRM::~Display_DRM() = default;

/*****************************/
/*          Screen           */
/*****************************/
lv_obj_t* Display_DRM::screen() const {
    return m_impl->screen;
}

/*****************************/
/*          Refresh          */
/*****************************/
void Display_DRM::refresh() {
    lv_timer_handler();
}

} // namespace ovb::hal::pi_zero
