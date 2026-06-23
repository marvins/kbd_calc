/**
 * @file    status_page.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Status page panel implementation
 */
#include <overboard/apps/status/status_page.hpp>

// C++ Standard Libraries
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <optional>
#include <sstream>

// Project Libraries
#include <overboard/apps/status/widgets/analog_clock.hpp>
#include <overboard/apps/status/widgets/digital_clock.hpp>
#include <overboard/apps/status/widgets/solar_info.hpp>
#include <overboard/core/location_provider.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/settings_store_factory.hpp>
#include <overboard/log/stdout_logger.hpp>
#include <overboard/version.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Status_Page::Impl {
    Impl(const core::Layer_Manager& l, Dismiss_Cb cb)
        : layers(l), on_dismiss(std::move(cb)) {}

    const core::Layer_Manager&   layers;
    Dismiss_Cb                   on_dismiss;
    lv_obj_t*                    container  = nullptr;
    lv_timer_t*                  clock_timer = nullptr;
    lv_obj_t*                    about_popup = nullptr;
    std::unique_ptr<Header_Bar>  header;
    std::unique_ptr<Footer_Bar>  footer;
    std::unique_ptr<widgets::Analog_Clock> analog_clock;
    std::unique_ptr<widgets::Digital_Clock> digital_clock;
    std::unique_ptr<widgets::Solar_Info>   solar_info;
    bool                         dismissed  = false;

    /// @brief Pending location resolved from background thread
    std::mutex                          location_mutex;
    std::optional<core::Solar_Location> pending_location;
};

/*******************************/
/*          Constructor        */
/*******************************/
Status_Page::Status_Page(const core::Layer_Manager& layers, Dismiss_Cb on_dismiss)
    : m_impl(std::make_unique<Impl>(layers, std::move(on_dismiss))) {}

/*******************************/
/*          Destructor         */
/*******************************/
Status_Page::~Status_Page() = default;

/*******************************/
/*           Activate          */
/*******************************/
void Status_Page::activate(lv_obj_t* parent) {
    LOG_DEBUG("Status_Page: activating");
    m_impl->dismissed = false;

    m_impl->container = lv_obj_create(parent);
    lv_obj_set_size(m_impl->container, lv_pct(100), lv_pct(100));
    lv_obj_align(m_impl->container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->container, LV_OBJ_FLAG_SCROLLABLE);

    // Use explicit dimensions - parent may not be laid out yet
    const int width = hal::LCD_WIDTH;

    // Header bar
    m_impl->header = std::make_unique<Header_Bar>(m_impl->container, width);
    m_impl->header->set_app_name("kbd_calc - Status");
    lv_obj_align(m_impl->header->get_obj(), LV_ALIGN_TOP_MID, 0, 0);

    // Content container — horizontal row: [clock column] | [solar info]
    lv_obj_t* content = lv_obj_create(m_impl->container);
    lv_obj_set_size(content, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(content, 0, LV_PART_MAIN);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(content, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(content, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(content, 16, LV_PART_MAIN);
    lv_obj_align(content, LV_ALIGN_CENTER, 0, 0);

    // Left column: analog clock stacked above digital clock
    lv_obj_t* clock_col = lv_obj_create(content);
    lv_obj_set_size(clock_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(clock_col, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(clock_col, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(clock_col, 0, LV_PART_MAIN);
    lv_obj_clear_flag(clock_col, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(clock_col, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(clock_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(clock_col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(clock_col, 8, LV_PART_MAIN);

    m_impl->analog_clock = std::make_unique<widgets::Analog_Clock>();
    m_impl->analog_clock->create(clock_col);

    m_impl->digital_clock = std::make_unique<widgets::Digital_Clock>();
    m_impl->digital_clock->create(clock_col);

    // Right column: solar info card
    m_impl->solar_info = std::make_unique<widgets::Solar_Info>();
    m_impl->solar_info->create(content);

    // Resolve geographic location from settings or IP geolocation
    {
        auto store = hal::Settings_Store_Factory::create();
        hal::Settings_Tree settings;
        store->load(settings);

        core::resolve_location_async(settings, [this](core::Solar_Location loc) {
            std::lock_guard<std::mutex> lock(m_impl->location_mutex);
            m_impl->pending_location = loc;
        });
    }

    // Set initial time
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    m_impl->analog_clock->update(tm);
    m_impl->digital_clock->update(tm);
    m_impl->solar_info->update(tm);

    // Timer to update clock every second
    m_impl->clock_timer = lv_timer_create([](lv_timer_t* timer) {
        auto* impl = static_cast<Status_Page::Impl*>(timer->user_data);
        if (!impl) return;

        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        if (impl->analog_clock) {
            impl->analog_clock->update(tm);
        }

        if (impl->digital_clock) {
            impl->digital_clock->update(tm);
        }

        if (impl->solar_info) {
            // Apply location if background thread resolved one
            {
                std::lock_guard<std::mutex> lock(impl->location_mutex);
                if (impl->pending_location) {
                    impl->solar_info->set_location(*impl->pending_location);
                    impl->pending_location.reset();
                }
            }
            impl->solar_info->update(tm);
        }
    }, 1000, m_impl.get());

    // Footer bar — decorative only, navigation via ESCAPE
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);
    lv_obj_align(m_impl->footer->get_obj(), LV_ALIGN_BOTTOM_MID, 0, 0);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Status_Page::deactivate() {
    LOG_DEBUG("Status_Page: deactivating");
    hide_about_popup();
    if (m_impl->clock_timer) {
        lv_timer_del(m_impl->clock_timer);
        m_impl->clock_timer = nullptr;
    }
    m_impl->footer.reset();
    m_impl->header.reset();
    if (m_impl->container) {
        lv_obj_del(m_impl->container);
        m_impl->container = nullptr;
    }
}

/*******************************/
/*        Handle Input         */
/*******************************/
bool Status_Page::handle_input(core::Action_Code action) {
    // ESC closes About popup if open, otherwise dismisses status page
    if (action == core::Action_Code::ESCAPE) {
        if (m_impl->about_popup) {
            hide_about_popup();
            return true;
        }
        if (!m_impl->dismissed && m_impl->on_dismiss) {
            m_impl->dismissed = true;
            m_impl->on_dismiss();
        }
    }

    // F1 shows About popup
    if (action == core::Action_Code::FUNC_1) {
        show_about_popup();
    }

    return true;
}

/*******************************/
/*       Handle Input Key      */
/*******************************/
bool Status_Page::handle_input_key(core::Input_Key key) {
    // ESCAPE from standard keyboard closes About popup if open, otherwise dismisses status page
    if (key == core::Input_Key::ESCAPE) {
        if (m_impl->about_popup) {
            hide_about_popup();
            return true;
        }
        if (!m_impl->dismissed && m_impl->on_dismiss) {
            m_impl->dismissed = true;
            m_impl->on_dismiss();
            return true;
        }
    }

    // F1 shows About popup
    if (key == core::Input_Key::F1) {
        show_about_popup();
        return true;
    }

    return false;
}

/*******************************/
/*           Refresh           */
/*******************************/
void Status_Page::refresh() {
    // Static page — nothing to update
}

/*******************************/
/*       Show About Popup      */
/*******************************/
void Status_Page::show_about_popup() {
    if (m_impl->about_popup) {
        lv_obj_del(m_impl->about_popup);
        m_impl->about_popup = nullptr;
    }

    // Create popup container
    m_impl->about_popup = lv_obj_create(lv_screen_active());
    lv_obj_set_size(m_impl->about_popup, 280, 200);
    lv_obj_center(m_impl->about_popup);
    lv_obj_set_style_bg_color(m_impl->about_popup, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->about_popup, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->about_popup, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->about_popup, 8, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(m_impl->about_popup);
    lv_label_set_text(title, "About kbd_calc");
    lv_obj_set_style_text_font(title, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 12);

    // Version info
    const std::string version_str = "Version: " + std::string(ovb::PROJECT_VERSION_DISPLAY);
    lv_obj_t* version = lv_label_create(m_impl->about_popup);
    lv_label_set_text(version, version_str.c_str());
    lv_obj_set_style_text_font(version, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(version, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    lv_obj_align(version, LV_ALIGN_TOP_MID, 0, 40);

    // Build date
    const std::string build_str = "Built: " + std::string(ovb::BUILD_DATE);
    lv_obj_t* build = lv_label_create(m_impl->about_popup);
    lv_label_set_text(build, build_str.c_str());
    lv_obj_set_style_text_font(build, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(build, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    lv_obj_align(build, LV_ALIGN_TOP_MID, 0, 60);

    // Close instruction
    lv_obj_t* close = lv_label_create(m_impl->about_popup);
    lv_label_set_text(close, "Press ESC to close");
    lv_obj_set_style_text_font(close, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(close, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
    lv_obj_align(close, LV_ALIGN_BOTTOM_MID, 0, -12);
}

/*******************************/
/*       Hide About Popup      */
/*******************************/
void Status_Page::hide_about_popup() {
    if (m_impl->about_popup) {
        lv_obj_del(m_impl->about_popup);
        m_impl->about_popup = nullptr;
    }
}

} // namespace ovb::gui
