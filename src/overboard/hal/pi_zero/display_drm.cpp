/**
 * @file    display_drm.cpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Linux DRM/KMS display implementation with software portrait rotation.
 *
 * Software rotation is implemented entirely in this file without modifying LVGL
 * vendor source. After lv_linux_drm_set_file configures the DRM device in DIRECT
 * mode, we intercept the display by:
 *   1. Mirroring the DRM driver's internal structs (layout-stable for this LVGL pin).
 *   2. Allocating a SW render buffer sized for portrait (600x1024).
 *   3. Switching the display to FULL render mode with that SW buffer.
 *   4. Installing a rotation flush callback that copy-rotates into the physical
 *      DMA scanout buffer, then calls the original drm_flush via the saved cb.
 */
#include <overboard/hal/pi_zero/display_drm.hpp>

// C++ Standard Libraries
#include <cstdint>
#include <cstring>
#include <stdexcept>

// Third-Party Libraries — DRM headers must precede lvgl to avoid type conflicts.
// xf86drm.h provides drmEventContext; xf86drmMode.h provides drmModeModeInfo etc.
// These are found via LIBDRM_INCLUDE_DIR (typically /usr/include/libdrm on Debian).
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <lvgl.h>
#include <lvgl_private.h>

// Overboard Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pi_zero {

/*****************************/
/*      DRM Mirror Types     */
/*****************************/

// Mirror of drm_buffer_t from lv_linux_drm.c (LV_USE_LINUX_DRM_GBM_BUFFERS not set).
struct Drm_Buffer {
    uint32_t      handle;
    uint32_t      pitch;
    uint32_t      offset;
    unsigned long size;
    uint8_t*      map;
    uint32_t      fb_handle;
};

// Mirror of drm_dev_t from lv_linux_drm.c (pinned to LVGL v9.5.0, no GBM).
// Must be kept in sync with lv_linux_drm.c if LVGL is updated.
static constexpr int DRM_BUFFER_CNT { 2 };
struct Drm_Dev {
    int               fd;
    uint32_t          conn_id, enc_id, crtc_id, plane_id, crtc_idx;
    uint32_t          width, height;
    uint32_t          mmWidth, mmHeight;
    uint32_t          fourcc;
    drmModeModeInfo   mode;
    uint32_t          blob_id;
    drmModeCrtc*      saved_crtc;
    drmModeAtomicReq* req;
    drmEventContext   drm_event_ctx;
    drmModePlane*     plane;
    drmModeCrtc*      crtc;
    drmModeConnector* conn;
    uint32_t          count_plane_props;
    uint32_t          count_crtc_props;
    uint32_t          count_conn_props;
    drmModePropertyPtr plane_props[128];
    drmModePropertyPtr crtc_props[128];
    drmModePropertyPtr conn_props[128];
    Drm_Buffer        drm_bufs[DRM_BUFFER_CNT];
    Drm_Buffer*       act_buf;
};

/*****************************/
/*    Rotation Context       */
/*****************************/
struct Rot_Ctx {
    lv_display_flush_cb_t orig_flush_cb = nullptr;
    uint8_t*              sw_buf        = nullptr;
    int32_t               phys_w        = 0;  // physical DMA buffer width  (1024)
    int32_t               phys_h        = 0;  // physical DMA buffer height (600)
};

/*****************************/
/*    Rotation Flush CB      */
/*****************************/
static void rotation_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
    if (!lv_display_flush_is_last(disp)) return;

    auto* ctx     = static_cast<Rot_Ctx*>(lv_display_get_user_data(disp));
    auto* drm_dev = static_cast<Drm_Dev*>(lv_display_get_driver_data(disp));

    // Copy-rotate 270°: logical portrait (phys_h x phys_w) → physical landscape (phys_w x phys_h)
    const int32_t log_w = ctx->phys_h;
    const int32_t log_h = ctx->phys_w;
    const int32_t bpp   = static_cast<int32_t>(LV_COLOR_DEPTH / 8);

    const uint8_t* src = px_map;
    uint8_t*       dst = drm_dev->drm_bufs[0].map;

    for (int32_t y = 0; y < log_h; ++y) {
        for (int32_t x = 0; x < log_w; ++x) {
            const int32_t src_off = (y * log_w + x)                     * bpp;
            const int32_t dst_off = ((log_w - 1 - x) * ctx->phys_w + y) * bpp;
            std::memcpy(dst + dst_off, src + src_off, static_cast<size_t>(bpp));
        }
    }

    // Point DRM driver at drm_bufs[0] then hand off to original drm_flush.
    drm_dev->act_buf = &drm_dev->drm_bufs[0];
    ctx->orig_flush_cb(disp, area, px_map);
}

/*****************************/
/*          PImpl            */
/*****************************/
struct Display_DRM::Impl {
    lv_display_t* lv_display = nullptr;
    lv_obj_t*     screen     = nullptr;
    int           width      = 0;
    int           height     = 0;
    Rot_Ctx       rot_ctx;
};

/*****************************/
/*        Constructor        */
/*****************************/
Display_DRM::Display_DRM(int width, int height)
    : m_impl(std::make_unique<Impl>()) {
    LOG_INFO("Display_DRM: Creating DRM display: ", width, " x ", height);

    m_impl->width  = width;
    m_impl->height = height;

    // Step 1: create LVGL DRM display and let the driver configure itself
    // normally in DIRECT mode with physical DMA buffers (1024x600 landscape).
    m_impl->lv_display = lv_linux_drm_create();
    if (!m_impl->lv_display) {
        throw std::runtime_error("Display_DRM: lv_linux_drm_create failed");
    }

    lv_result_t lv_res = lv_linux_drm_set_file(m_impl->lv_display, "/dev/dri/card0", -1);
    if (lv_res != LV_RESULT_OK) {
        lv_display_delete(m_impl->lv_display);
        throw std::runtime_error("Display_DRM: lv_linux_drm_set_file failed");
    }

    // Step 2: record physical dimensions from what the driver detected.
    // lv_display_get_*_resolution returns the logical resolution which at this
    // point is still physical (no rotation set yet).
    const int32_t phys_w = lv_display_get_horizontal_resolution(m_impl->lv_display); // 1024
    const int32_t phys_h = lv_display_get_vertical_resolution(m_impl->lv_display);   // 600

    // Step 3: allocate SW render buffer for portrait (phys_h x phys_w = 600x1024).
    const size_t sw_buf_size = static_cast<size_t>(phys_h) * static_cast<size_t>(phys_w)
                               * (LV_COLOR_DEPTH / 8);
    m_impl->rot_ctx.sw_buf  = static_cast<uint8_t*>(lv_malloc(sw_buf_size));
    m_impl->rot_ctx.phys_w  = phys_w;
    m_impl->rot_ctx.phys_h  = phys_h;

    if (!m_impl->rot_ctx.sw_buf) {
        lv_display_delete(m_impl->lv_display);
        throw std::runtime_error("Display_DRM: SW rotation buffer allocation failed");
    }

    // Step 4: save original flush_cb, install our rotation wrapper, and store
    // the rotation context in user_data (driver_data is used by the DRM driver).
    m_impl->rot_ctx.orig_flush_cb = m_impl->lv_display->flush_cb;
    lv_display_set_user_data(m_impl->lv_display, &m_impl->rot_ctx);
    lv_display_set_flush_cb(m_impl->lv_display, rotation_flush_cb);

    // Step 5: set rotation so LVGL renders in portrait (600x1024 logical).
    lv_display_set_rotation(m_impl->lv_display, LV_DISPLAY_ROTATION_270);

    // Step 6: switch to FULL render mode with the SW buffer.
    // LVGL now renders the full portrait frame into sw_buf on every refresh.
    lv_display_set_buffers(m_impl->lv_display, m_impl->rot_ctx.sw_buf, nullptr,
                           static_cast<uint32_t>(sw_buf_size),
                           LV_DISPLAY_RENDER_MODE_FULL);

    lv_display_set_default(m_impl->lv_display);
    m_impl->screen = lv_screen_active();

    LOG_INFO("Display_DRM: DRM display ready (",
             lv_display_get_horizontal_resolution(m_impl->lv_display), " x ",
             lv_display_get_vertical_resolution(m_impl->lv_display), ")");
}

/*****************************/
/*        Destructor         */
/*****************************/
Display_DRM::~Display_DRM() {
    if (m_impl->rot_ctx.sw_buf) {
        lv_free(m_impl->rot_ctx.sw_buf);
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
    lv_timer_handler();
}

} // namespace ovb::hal::pi_zero
