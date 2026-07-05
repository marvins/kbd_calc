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
#include <cstdint>
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
#include <overboard/log/stdout_logger.hpp>
#include <overboard/version.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Status_Page::Impl {

    /**
     * @brief Constructor
     */
    Impl( const core::Layer_Manager&              l,
          hal::I_System_Info&                     si,
          std::shared_ptr<core::Settings_Manager> s,
          Dismiss_Cb                              cb )
         : layers(l),
           system_info(si),
           settings(s),
           on_dismiss(std::move(cb)) {}

    const core::Layer_Manager&              layers;
    hal::I_System_Info&                     system_info;
    std::shared_ptr<core::Settings_Manager> settings;
    Dismiss_Cb                              on_dismiss;
    lv_obj_t*                    container   = nullptr;
    lv_timer_t*                  clock_timer = nullptr;
    lv_timer_t*                  solar_timer = nullptr;
    lv_obj_t*                    about_popup = nullptr;
    lv_obj_t*                    stats_popup = nullptr;
    std::unique_ptr<Header_Bar>  header;
    std::unique_ptr<Footer_Bar>  footer;
    std::unique_ptr<widgets::Analog_Clock>  analog_clock;
    std::unique_ptr<widgets::Digital_Clock> digital_clock;
    std::unique_ptr<widgets::Solar_Info>    solar_info;
    bool                         dismissed  = false;

    /// @brief Pending location resolved from background thread
    std::mutex                          location_mutex;
    std::optional<core::Solar_Location> pending_location;
};

/*******************************/
/*          Constructor        */
/*******************************/
Status_Page::Status_Page( const core::Layer_Manager&              layers,
                          hal::I_System_Info&                     system_info,
                          std::shared_ptr<core::Settings_Manager> settings,
                          Dismiss_Cb                              on_dismiss )
    : m_impl(std::make_unique<Impl>(layers, system_info, settings, std::move(on_dismiss))) {}

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
    lv_obj_set_scrollable(m_impl->container, false);

    // Use explicit dimensions - parent may not be laid out yet
    const int width = hal::LCD_WIDTH;

    // Header bar
    m_impl->header = std::make_unique<Header_Bar>( m_impl->container,
                                                   width,
                                                   m_impl->system_info );
    m_impl->header->set_app_name("kbd_calc - Status");
    lv_obj_align(m_impl->header->get_obj(), LV_ALIGN_TOP_MID, 0, 0);

    // Content container — horizontal row: [clock column] | [solar info]
    lv_obj_t* content = lv_obj_create(m_impl->container);
    lv_obj_set_size(content, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(content, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(content, false);
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
    lv_obj_set_scrollable(clock_col, false);
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
        core::resolve_location_async(m_impl->settings->tree(), [this](core::Solar_Location loc) {
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

    // Read update rates from settings
    const uint32_t clock_update_ms = m_impl->settings->get<uint32_t>("status.clock_update_ms", 1000);
    const uint32_t solar_update_ms = m_impl->settings->get<uint32_t>("status.solar_update_ms", 60000);

    // Timer to update clocks
    m_impl->clock_timer = lv_timer_create([](lv_timer_t* timer) {
        auto* impl = static_cast<Status_Page::Impl*>(timer->user_data);
        if (!impl) return;

        auto now_update = std::chrono::system_clock::now();
        std::time_t time_update = std::chrono::system_clock::to_time_t(now_update);
        std::tm tm_update = *std::localtime(&time_update);

        if (impl->analog_clock) {
            impl->analog_clock->update(tm_update);
        }

        if (impl->digital_clock) {
            impl->digital_clock->update(tm_update);
        }

        if (impl->header) {
            impl->header->refresh();
        }
    }, clock_update_ms, m_impl.get());

    // Separate timer for solar widget (slower update rate)
    m_impl->solar_timer = lv_timer_create([](lv_timer_t* timer) {
        auto* impl = static_cast<Status_Page::Impl*>(timer->user_data);
        if (!impl) return;

        auto now_update = std::chrono::system_clock::now();
        std::time_t time_update = std::chrono::system_clock::to_time_t(now_update);
        std::tm tm_update = *std::localtime(&time_update);

        if (impl->solar_info) {
            // Apply location if background thread resolved one
            {
                std::lock_guard<std::mutex> lock(impl->location_mutex);
                if (impl->pending_location) {
                    impl->solar_info->set_location(*impl->pending_location);
                    impl->pending_location.reset();
                }
            }
            impl->solar_info->update(tm_update);
        }
    }, solar_update_ms, m_impl.get());

    // Footer bar
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);
    m_impl->footer->set_label(0, "About");
    m_impl->footer->set_label(1, "Stats");
    lv_obj_align(m_impl->footer->get_obj(), LV_ALIGN_BOTTOM_MID, 0, 0);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Status_Page::deactivate() {
    LOG_DEBUG("Status_Page: deactivating");
    hide_stats_popup();
    hide_about_popup();
    if (m_impl->clock_timer) {
        lv_timer_del(m_impl->clock_timer);
        m_impl->clock_timer = nullptr;
    }
    if (m_impl->solar_timer) {
        lv_timer_del(m_impl->solar_timer);
        m_impl->solar_timer = nullptr;
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
    // ESC closes any open popup, otherwise dismisses status page
    if (action == core::Action_Code::ESCAPE) {
        if (m_impl->stats_popup) {
            hide_stats_popup();
            return true;
        }
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

    // F2 shows Stats popup
    if (action == core::Action_Code::FUNC_2) {
        show_stats_popup();
    }

    return true;
}

/*******************************/
/*       Handle Input Key      */
/*******************************/
bool Status_Page::handle_input_key(core::Input_Key key) {
    // ESCAPE closes any open popup, otherwise dismisses status page
    if (key == core::Input_Key::ESCAPE) {
        if (m_impl->stats_popup) {
            hide_stats_popup();
            return true;
        }
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

    // F2 shows Stats popup
    if (key == core::Input_Key::F2) {
        show_stats_popup();
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

/*******************************/
/*       Show Stats Popup      */
/*******************************/
void Status_Page::show_stats_popup() {
    if (m_impl->stats_popup) {
        lv_obj_del(m_impl->stats_popup);
        m_impl->stats_popup = nullptr;
    }

    const hal::System_Info info = m_impl->system_info.get_info();

    // Build display rows
    struct Row { std::string label; std::string value; };
    std::vector<Row> rows;

    // CPU temperature
    if (info.cpu_temp_c.has_value()) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << *info.cpu_temp_c << " \xC2\xB0" "C";
        rows.push_back({"CPU Temp", oss.str()});
    } else {
        rows.push_back({"CPU Temp", "N/A"});
    }

    // Storage
    if (info.storage.has_value()) {
        const auto& st = *info.storage;
        auto to_gb = [](uint64_t bytes) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1)
                << (static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0)) << " GB";
            return oss.str();
        };
        rows.push_back({"Storage Used",  to_gb(st.used_bytes)});
        rows.push_back({"Storage Free",  to_gb(st.free_bytes)});
        rows.push_back({"Storage Total", to_gb(st.total_bytes)});
    } else {
        rows.push_back({"Storage", "N/A"});
    }

    // Battery
    if (info.battery_percent.has_value()) {
        rows.push_back({"Battery", std::to_string(*info.battery_percent) + "%"});
    } else if (m_impl->system_info.has_battery()) {
        rows.push_back({"Battery", "N/A"});
    }

    // Size popup to content
    constexpr int ROW_HEIGHT    { 22 };
    constexpr int POPUP_PADDING { 12 };
    constexpr int TITLE_HEIGHT  { 32 };
    constexpr int FOOTER_HEIGHT { 24 };
    constexpr int POPUP_WIDTH   { 300 };
    const int popup_height = TITLE_HEIGHT
                           + static_cast<int>(rows.size()) * ROW_HEIGHT
                           + FOOTER_HEIGHT
                           + POPUP_PADDING * 2;

    m_impl->stats_popup = lv_obj_create(lv_screen_active());
    lv_obj_set_size(m_impl->stats_popup, POPUP_WIDTH, popup_height);
    lv_obj_center(m_impl->stats_popup);
    lv_obj_set_style_bg_color(m_impl->stats_popup, lvgl_color(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->stats_popup, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->stats_popup, lvgl_color(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->stats_popup, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->stats_popup, POPUP_PADDING, LV_PART_MAIN);

    // Title
    lv_obj_t* title = lv_label_create(m_impl->stats_popup);
    lv_label_set_text(title, "System Stats");
    lv_obj_set_style_text_font(title, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lvgl_color(LVGL_COLOR_TEXT_PRIMARY), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    // Rows
    int y = TITLE_HEIGHT;
    for (const auto& row : rows) {
        lv_obj_t* lbl = lv_label_create(m_impl->stats_popup);
        const std::string text = row.label + ": " + row.value;
        lv_label_set_text(lbl, text.c_str());
        lv_obj_set_style_text_font(lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
        lv_obj_set_style_text_color(lbl, lvgl_color(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
        lv_obj_set_pos(lbl, 0, y);
        y += ROW_HEIGHT;
    }

    // Close instruction
    lv_obj_t* close_lbl = lv_label_create(m_impl->stats_popup);
    lv_label_set_text(close_lbl, "Press ESC to close");
    lv_obj_set_style_text_font(close_lbl, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(close_lbl, lvgl_color(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
    lv_obj_align(close_lbl, LV_ALIGN_BOTTOM_MID, 0, 0);
}

/*******************************/
/*       Hide Stats Popup      */
/*******************************/
void Status_Page::hide_stats_popup() {
    if (m_impl->stats_popup) {
        lv_obj_del(m_impl->stats_popup);
        m_impl->stats_popup = nullptr;
    }
}

} // namespace ovb::gui
