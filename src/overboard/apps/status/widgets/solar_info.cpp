/**
 * @file    solar_info.cpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   Solar information widget implementation
 */
#include <overboard/apps/status/widgets/solar_info.hpp>

// C++ Standard Libraries
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

// Project Libraries
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui::widgets {

// Arc visualization constants
inline constexpr int   ARC_SIZE        { 160 };
inline constexpr int   ARC_LINE_WIDTH  { 4 };
inline constexpr int   SUN_DOT_SIZE    { 14 };
inline constexpr float ARC_START_DEG   { 180.0f };  // Left (sunrise side)
inline constexpr float ARC_END_DEG     { 0.0f   };  // Right (sunset side)

// Color constants
inline constexpr uint32_t COLOR_SUNRISE  { 0xFF8C00 };  // Dark orange
inline constexpr uint32_t COLOR_NOON     { 0xFFD700 };  // Gold
inline constexpr uint32_t COLOR_SUNSET   { 0xFF4500 };  // Orange-red
inline constexpr uint32_t COLOR_NIGHT    { 0x4A4A8A };  // Dark blue
inline constexpr uint32_t COLOR_ARC_BG   { 0xDDDDDD };  // Light grey
inline constexpr uint32_t COLOR_ARC_FG   { 0xFF9800 };  // Orange arc
inline constexpr uint32_t COLOR_HORIZON  { 0x888888 };  // Horizon line

/*******************************/
/*            Impl             */
/*******************************/
struct Solar_Info::Impl {
    lv_obj_t* container    = nullptr;

    // Title
    lv_obj_t* title_label  = nullptr;  ///< Day of week title

    // Arc visualization
    lv_obj_t* arc_bg       = nullptr;  ///< Background arc (full semicircle)
    lv_obj_t* arc_fg       = nullptr;  ///< Foreground arc (elapsed day)
    lv_obj_t* sun_dot      = nullptr;  ///< Sun position indicator
    lv_obj_t* horizon_line = nullptr;  ///< Horizon line below arc

    // Time labels
    lv_obj_t* sunrise_icon  = nullptr;
    lv_obj_t* sunrise_label = nullptr;
    lv_obj_t* noon_icon     = nullptr;
    lv_obj_t* noon_label    = nullptr;
    lv_obj_t* sunset_icon   = nullptr;
    lv_obj_t* sunset_label  = nullptr;

    // Info labels
    lv_obj_t* elevation_label  = nullptr;
    lv_obj_t* daylength_label  = nullptr;

    // Sun dot position points
    static lv_point_precise_t horizon_pts[2];

    core::Solar_Location location {
        39.7392,   // Denver latitude
        -104.9903, // Denver longitude
        -6.0       // MDT (UTC-6)
    };

    core::Solar_Times last_solar {};
};

lv_point_precise_t Solar_Info::Impl::horizon_pts[2] = {};

/*******************************/
/*       Helper: Row Item      */
/*******************************/
static void make_row_item(lv_obj_t* parent,
                          lv_obj_t*& icon_out,
                          lv_obj_t*& label_out,
                          const char* icon_text,
                          uint32_t    icon_color) {
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_scrollable(row, false);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 4, LV_PART_MAIN);

    icon_out = lv_label_create(row);
    lv_label_set_text(icon_out, icon_text);
    lv_obj_set_style_text_font(icon_out, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(icon_out, lv_color_hex(icon_color), LV_PART_MAIN);

    label_out = lv_label_create(row);
    lv_label_set_text(label_out, "--:--");
    lv_obj_set_style_text_font(label_out, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_out, lv_color_hex(0x333333), LV_PART_MAIN);
}

/*******************************/
/*          Constructor        */
/*******************************/
Solar_Info::Solar_Info()
    : m_impl(std::make_unique<Impl>())
{}

/*******************************/
/*          Destructor         */
/*******************************/
Solar_Info::~Solar_Info() {
    destroy();
}

/*******************************/
/*            Create           */
/*******************************/
void Solar_Info::create(lv_obj_t* parent) {
    // Outer card container
    m_impl->container = lv_obj_create(parent);
    lv_obj_set_size(m_impl->container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(m_impl->container, lv_color_hex(LVGL_COLOR_BG_BEZEL), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(m_impl->container, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->container, lv_color_hex(LVGL_COLOR_BORDER_MEDIUM), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->container, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->container, 10, LV_PART_MAIN);
    lv_obj_set_scrollable(m_impl->container, false);

    // Vertical flex layout for card contents
    lv_obj_set_layout(m_impl->container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_impl->container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(m_impl->container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(m_impl->container, 6, LV_PART_MAIN);

    // --- Title label (day of week) ---
    lv_obj_t* title = lv_label_create(m_impl->container);
    lv_label_set_text(title, "---");  // Will be updated in update()
    lv_obj_set_style_text_font(title, LVGL_FONT_DEFAULT, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lv_color_hex(LVGL_COLOR_TEXT_SECONDARY), LV_PART_MAIN);
    m_impl->title_label = title;

    // --- Arc visualization container ---
    lv_obj_t* arc_container = lv_obj_create(m_impl->container);
    lv_obj_set_size(arc_container, ARC_SIZE, ARC_SIZE / 2 + 12);
    lv_obj_set_style_bg_opa(arc_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(arc_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(arc_container, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(arc_container, false);

    // Background arc (full day semicircle, grey)
    // Top semicircle: 180° (left/sunrise) → 360°/0° (right/sunset) going through 270° (top/noon)
    m_impl->arc_bg = lv_arc_create(arc_container);
    lv_arc_set_bg_angles(m_impl->arc_bg, 180, 360);
    lv_arc_set_value(m_impl->arc_bg, 0);
    lv_arc_set_range(m_impl->arc_bg, 0, 100);
    lv_obj_set_size(m_impl->arc_bg, ARC_SIZE, ARC_SIZE);
    lv_obj_align(m_impl->arc_bg, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_arc_color(m_impl->arc_bg, lv_color_hex(COLOR_ARC_BG), LV_PART_MAIN);
    lv_obj_set_style_arc_width(m_impl->arc_bg, ARC_LINE_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_color(m_impl->arc_bg, lv_color_hex(COLOR_ARC_FG), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(m_impl->arc_bg, ARC_LINE_WIDTH, LV_PART_INDICATOR);

    // Disable events on the arc background
    // TODO: Replace with lv_obj_add_event_cb when LVGL is updated
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    lv_obj_add_flag(m_impl->arc_bg, LV_OBJ_FLAG_EVENT_BUBBLE);
#pragma GCC diagnostic pop

    lv_obj_set_clickable(m_impl->arc_bg, false);
    lv_obj_set_style_bg_opa(m_impl->arc_bg, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_pad_all(m_impl->arc_bg, 0, LV_PART_KNOB);

    // Sun dot indicator
    m_impl->sun_dot = lv_obj_create(arc_container);
    lv_obj_set_size(m_impl->sun_dot, SUN_DOT_SIZE, SUN_DOT_SIZE);
    lv_obj_set_style_radius(m_impl->sun_dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(m_impl->sun_dot, lv_color_hex(COLOR_NOON), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(m_impl->sun_dot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(m_impl->sun_dot, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->sun_dot, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(m_impl->sun_dot, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(m_impl->sun_dot, lv_color_hex(COLOR_NOON), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(m_impl->sun_dot, LV_OPA_60, LV_PART_MAIN);
    lv_obj_align(m_impl->sun_dot, LV_ALIGN_BOTTOM_MID, 0, 0);  // default at horizon

    // Horizon line (full width at bottom of arc)
    m_impl->horizon_line = lv_line_create(arc_container);
    m_impl->Impl::horizon_pts[0] = { 0, static_cast<lv_value_precise_t>(ARC_SIZE / 2 + 6) };
    m_impl->Impl::horizon_pts[1] = { static_cast<lv_value_precise_t>(ARC_SIZE), static_cast<lv_value_precise_t>(ARC_SIZE / 2 + 6) };
    lv_line_set_points(m_impl->horizon_line, m_impl->Impl::horizon_pts, 2);
    lv_obj_set_style_line_color(m_impl->horizon_line, lv_color_hex(COLOR_HORIZON), LV_PART_MAIN);
    lv_obj_set_style_line_width(m_impl->horizon_line, 1, LV_PART_MAIN);

    // --- Time rows ---
    make_row_item(m_impl->container, m_impl->sunrise_icon, m_impl->sunrise_label, "SR", COLOR_SUNRISE);
    make_row_item(m_impl->container, m_impl->noon_icon,    m_impl->noon_label,    "SN", COLOR_NOON);
    make_row_item(m_impl->container, m_impl->sunset_icon,  m_impl->sunset_label,  "SS", COLOR_SUNSET);

    // --- Elevation row ---
    m_impl->elevation_label = lv_label_create(m_impl->container);
    lv_label_set_text(m_impl->elevation_label, "Elev: --.-deg");
    lv_obj_set_style_text_font(m_impl->elevation_label, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_impl->elevation_label, lv_color_hex(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);

    // --- Day length row ---
    m_impl->daylength_label = lv_label_create(m_impl->container);
    lv_label_set_text(m_impl->daylength_label, "Day: --h --m");
    lv_obj_set_style_text_font(m_impl->daylength_label, LVGL_FONT_SMALL, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_impl->daylength_label, lv_color_hex(LVGL_COLOR_TEXT_MUTED), LV_PART_MAIN);
}

/*******************************/
/*            Update           */
/*******************************/
void Solar_Info::update(const std::tm& tm) {
    if (!m_impl->container) return;

    // Update day of week title
    if (m_impl->title_label) {
        std::tm tm_copy = tm;  // std::put_time requires non-const pointer
        std::ostringstream oss;
        oss << std::put_time(&tm_copy, "%A");  // %A = full weekday name
        lv_label_set_text(m_impl->title_label, oss.str().c_str());
    }

    const core::Solar_Times solar = core::calculate_solar_times(tm, m_impl->location);
    m_impl->last_solar = solar;

    // Update time labels
    if (solar.is_polar_day) {
        lv_label_set_text(m_impl->sunrise_label, "Polar Day");
        lv_label_set_text(m_impl->noon_label,    "--:--");
        lv_label_set_text(m_impl->sunset_label,  "Polar Day");
    } else if (solar.is_polar_night) {
        lv_label_set_text(m_impl->sunrise_label, "Polar Night");
        lv_label_set_text(m_impl->noon_label,    "--:--");
        lv_label_set_text(m_impl->sunset_label,  "Polar Night");
    } else {
        lv_label_set_text(m_impl->sunrise_label, core::format_solar_time(solar.sunrise_hour).c_str());
        lv_label_set_text(m_impl->noon_label,    core::format_solar_time(solar.solar_noon_hour).c_str());
        lv_label_set_text(m_impl->sunset_label,  core::format_solar_time(solar.sunset_hour).c_str());
    }

    // Update elevation label
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Elev: " << solar.solar_elevation << " deg";
        lv_label_set_text(m_impl->elevation_label, oss.str().c_str());
    }

    // Update day length label
    {
        const int dl_h = static_cast<int>(solar.day_length_hour);
        const int dl_m = static_cast<int>((solar.day_length_hour - dl_h) * 60.0 + 0.5);
        std::ostringstream oss;
        oss << "Day: " << dl_h << "h " << std::setfill('0') << std::setw(2) << dl_m << "m";
        lv_label_set_text(m_impl->daylength_label, oss.str().c_str());
    }

    // Update arc and sun dot
    if (!solar.is_polar_day && !solar.is_polar_night) {
        const double current_hour = tm.tm_hour + tm.tm_min / 60.0 + tm.tm_sec / 3600.0;
        const double day_len      = solar.sunset_hour - solar.sunrise_hour;

        // Arc progress 0-100 over the day
        double progress = 0.0;
        if (day_len > 0.0) {
            progress = (current_hour - solar.sunrise_hour) / day_len;
            progress = std::max(0.0, std::min(1.0, progress));
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << "Solar: current=" << current_hour
            << ", sunrise=" << solar.sunrise_hour
            << ", noon=" << solar.solar_noon_hour
            << ", sunset=" << solar.sunset_hour
            << std::setprecision(3) << ", progress=" << progress;
        LOG_DEBUG(oss.str());

        lv_arc_set_value(m_impl->arc_bg, static_cast<int32_t>(progress * 100.0));

        // Sun dot position along the arc
        // LVGL arc angles: 0° = 3 o'clock (right), increases clockwise
        // Our semicircle: bg_angles(0, 180) = right to left through bottom
        // But we want: sunrise (left) → noon (top) → sunset (right)
        // So we need the TOP semicircle: 180° (left) → 270° (top) → 0°/360° (right)
        // Map progress [0..1] to LVGL angle [180..360] (or [180..0] wrapping through 270)
        // In standard math: left=180°, top=90°, right=0°
        // progress=0 (sunrise): angle = 180° (left)
        // progress=0.5 (noon): angle = 90° (top)
        // progress=1.0 (sunset): angle = 0° (right)
        const float math_angle_deg = static_cast<float>(180.0 - progress * 180.0);
        const float math_angle_rad = math_angle_deg * 3.14159265f / 180.0f;

        // Radius of arc center line
        const float arc_r = (ARC_SIZE / 2.0f) - ARC_LINE_WIDTH / 2.0f;
        const float cx    = ARC_SIZE / 2.0f;
        const float cy    = ARC_SIZE / 2.0f;  // center of full circle (top half visible)

        // Standard math: cos/sin with y-axis inverted for screen coordinates
        const float sun_x = cx + arc_r * std::cos(math_angle_rad);
        const float sun_y = cy - arc_r * std::sin(math_angle_rad);  // y is flipped in screen coords

        // Position sun dot relative to arc container
        const int dot_x = static_cast<int>(sun_x) - SUN_DOT_SIZE / 2;
        const int dot_y = static_cast<int>(sun_y) - SUN_DOT_SIZE / 2;
        lv_obj_set_pos(m_impl->sun_dot, dot_x, dot_y);

        // Color the sun dot based on elevation
        uint32_t dot_color;
        if (solar.solar_elevation < 0.0) {
            dot_color = COLOR_NIGHT;
        } else if (solar.solar_elevation < 6.0) {
            dot_color = COLOR_SUNRISE;
        } else if (solar.solar_elevation > 45.0) {
            dot_color = COLOR_NOON;
        } else {
            dot_color = COLOR_SUNSET;
        }
        lv_obj_set_style_bg_color(m_impl->sun_dot, lv_color_hex(dot_color), LV_PART_MAIN);
        lv_obj_set_style_shadow_color(m_impl->sun_dot, lv_color_hex(dot_color), LV_PART_MAIN);
    }
}

/*******************************/
/*           Destroy           */
/*******************************/
void Solar_Info::destroy() {
    if (m_impl->container) {
        if (lv_display_get_next(nullptr) != nullptr) {
            lv_obj_del(m_impl->container);
        }
        m_impl->container   = nullptr;
        m_impl->title_label = nullptr;
        m_impl->arc_bg      = nullptr;
        m_impl->sun_dot     = nullptr;
        m_impl->horizon_line = nullptr;
        m_impl->sunrise_label = nullptr;
        m_impl->noon_label    = nullptr;
        m_impl->sunset_label  = nullptr;
        m_impl->elevation_label = nullptr;
        m_impl->daylength_label = nullptr;
    }
}

/*******************************/
/*         Get Container       */
/*******************************/
lv_obj_t* Solar_Info::get_container() const {
    return m_impl->container;
}

/*******************************/
/*        Set Location         */
/*******************************/
void Solar_Info::set_location(const core::Solar_Location& loc) {
    m_impl->location = loc;
}

} // namespace ovb::gui::widgets
