/**
 * @file    display_drm.cpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Linux DRM/KMS display implementation
 */
#include <overboard/hal/pi_zero/display_drm.hpp>

// C++ Standard Libraries
#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>

// Third-Party Libraries
#include <gbm.h>
#include <lvgl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

// Overboard Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pi_zero {

/**
 * @brief PImpl structure for Display_DRM
 */
struct Display_DRM::Impl {
    int         drm_fd = -1;
    drmModeRes* res = nullptr;
    uint32_t    conn_id = 0;
    uint32_t    crtc_id = 0;
    uint32_t    fb_id = 0;

    gbm_device* gbm_dev = nullptr;
    gbm_surface* gbm_surf = nullptr;

    lv_display_t* lv_display = nullptr;
    lv_obj_t*     screen = nullptr;

    int width = 0;
    int height = 0;
};

/*****************************/
/*        Constructor        */
/*****************************/
Display_DRM::Display_DRM(int width, int height)
    : m_impl(std::make_unique<Impl>()) {
    LOG_DEBUG("Display_DRM: Creating DRM display: ", width, " x ", height);

    m_impl->width = width;
    m_impl->height = height;

    // Open DRM device
    m_impl->drm_fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (m_impl->drm_fd < 0) {
        throw std::runtime_error("Display_DRM: Failed to open /dev/dri/card0");
    }

    // Get resources
    m_impl->res = drmModeGetResources(m_impl->drm_fd);
    if (!m_impl->res) {
        close(m_impl->drm_fd);
        throw std::runtime_error("Display_DRM: Failed to get DRM resources");
    }

    // Find a connected connector
    for (int i = 0; i < m_impl->res->count_connectors; i++) {
        drmModeConnector* conn = drmModeGetConnector(m_impl->drm_fd, m_impl->res->connectors[i]);
        if (conn && conn->connection == DRM_MODE_CONNECTED) {
            m_impl->conn_id = conn->connector_id;
            // Find a CRTC
            for (int j = 0; j < m_impl->res->count_crtcs; j++) {
                if (conn->encoder_id) {
                    drmModeEncoder* enc = drmModeGetEncoder(m_impl->drm_fd, conn->encoder_id);
                    if (enc && enc->crtc_id == m_impl->res->crtcs[j]) {
                        m_impl->crtc_id = enc->crtc_id;
                        drmModeFreeEncoder(enc);
                        break;
                    }
                    if (enc) drmModeFreeEncoder(enc);
                }
            }
            drmModeFreeConnector(conn);
            break;
        }
        if (conn) drmModeFreeConnector(conn);
    }

    if (m_impl->conn_id == 0) {
        drmModeFreeResources(m_impl->res);
        close(m_impl->drm_fd);
        throw std::runtime_error("Display_DRM: No connected display found");
    }

    // Create GBM device
    m_impl->gbm_dev = gbm_create_device(m_impl->drm_fd);
    if (!m_impl->gbm_dev) {
        drmModeFreeResources(m_impl->res);
        close(m_impl->drm_fd);
        throw std::runtime_error("Display_DRM: Failed to create GBM device");
    }

    // Create GBM surface
    m_impl->gbm_surf = gbm_surface_create(
        m_impl->gbm_dev,
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        GBM_FORMAT_XRGB8888,
        GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
    );

    if (!m_impl->gbm_surf) {
        gbm_device_destroy(m_impl->gbm_dev);
        drmModeFreeResources(m_impl->res);
        close(m_impl->drm_fd);
        throw std::runtime_error("Display_DRM: Failed to create GBM surface");
    }

    // Create LVGL display using Linux DRM driver
    m_impl->lv_display = lv_linux_drm_create();
    if (!m_impl->lv_display) {
        gbm_surface_destroy(m_impl->gbm_surf);
        gbm_device_destroy(m_impl->gbm_dev);
        drmModeFreeResources(m_impl->res);
        close(m_impl->drm_fd);
        throw std::runtime_error("Display_DRM: lv_linux_drm_create failed");
    }

    // Set display resolution
    lv_display_set_resolution(m_impl->lv_display, width, height);
    lv_display_set_default(m_impl->lv_display);

    // Get the active screen
    m_impl->screen = lv_screen_active();

    LOG_DEBUG("Display_DRM: DRM display created successfully");
}

/*****************************/
/*        Destructor         */
/*****************************/
Display_DRM::~Display_DRM() {
    if (m_impl->fb_id) {
        drmModeRmFB(m_impl->drm_fd, m_impl->fb_id);
    }
    if (m_impl->gbm_surf) {
        gbm_surface_destroy(m_impl->gbm_surf);
    }
    if (m_impl->gbm_dev) {
        gbm_device_destroy(m_impl->gbm_dev);
    }
    if (m_impl->res) {
        drmModeFreeResources(m_impl->res);
    }
    if (m_impl->drm_fd >= 0) {
        close(m_impl->drm_fd);
    }
}

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
    // LVGL handles the actual rendering via the DRM driver
    lv_timer_handler();
}

} // namespace ovb::hal::pi_zero
