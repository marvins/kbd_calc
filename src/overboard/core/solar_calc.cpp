/**
 * @file    solar_calc.cpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   NOAA-based solar position and sunrise/sunset calculator implementation
 */
#include <overboard/core/solar_calc.hpp>

// C++ Standard Libraries
#include <cmath>
#include <cstdio>
#include <limits>
#include <numbers>
#include <string>

namespace ovb::core {

namespace detail {

inline constexpr double DEG_TO_RAD { std::numbers::pi / 180.0 };
inline constexpr double RAD_TO_DEG { 180.0 / std::numbers::pi };

/****************************/
/*      Julian Day          */
/****************************/
static double julian_day(int year, int month, int day) {
    if (month <= 2) {
        year  -= 1;
        month += 12;
    }
    const double A = std::floor(year / 100.0);
    const double B = 2.0 - A + std::floor(A / 4.0);
    return std::floor(365.25 * (year + 4716.0))
         + std::floor(30.6001 * (month + 1.0))
         + day + B - 1524.5;
}

/****************************/
/*      Julian Century      */
/****************************/
static double julian_century(double jd) {
    return (jd - 2451545.0) / 36525.0;
}

/****************************/
/*   Sun Geo Mean Lon       */
/****************************/
static double sun_geo_mean_lon(double t) {
    double lon = 280.46646 + t * (36000.76983 + t * 0.0003032);
    while (lon > 360.0) lon -= 360.0;
    while (lon < 0.0)   lon += 360.0;
    return lon;
}

/****************************/
/*   Sun Geo Mean Anomaly   */
/****************************/
static double sun_geo_mean_anomaly(double t) {
    return 357.52911 + t * (35999.05029 - t * 0.0001537);
}

/****************************/
/*   Orbit Eccentricity     */
/****************************/
static double earth_orbit_eccentricity(double t) {
    return 0.016708634 - t * (0.000042037 + t * 0.0000001267);
}

/****************************/
/*   Sun Eq of Center       */
/****************************/
static double sun_eq_of_center(double t) {
    const double m = sun_geo_mean_anomaly(t) * DEG_TO_RAD;
    return std::sin(m)     * (1.914602 - t * (0.004817 + 0.000014 * t))
         + std::sin(2.0*m) * (0.019993 - t * 0.000101)
         + std::sin(3.0*m) *  0.000289;
}

/****************************/
/*   Sun True Longitude     */
/****************************/
static double sun_true_lon(double t) {
    return sun_geo_mean_lon(t) + sun_eq_of_center(t);
}

/****************************/
/*   Sun Apparent Lon       */
/****************************/
static double sun_apparent_lon(double t) {
    const double o     = sun_true_lon(t);
    const double omega = 125.04 - 1934.136 * t;
    return o - 0.00569 - 0.00478 * std::sin(omega * DEG_TO_RAD);
}

/****************************/
/*   Mean Obliquity         */
/****************************/
static double mean_obliquity(double t) {
    const double secs = 21.448 - t * (46.8150 + t * (0.00059 - t * 0.001813));
    return 23.0 + (26.0 + secs / 60.0) / 60.0;
}

/****************************/
/*   Obliquity Corrected    */
/****************************/
static double obliquity_corrected(double t) {
    const double e0    = mean_obliquity(t);
    const double omega = 125.04 - 1934.136 * t;
    return e0 + 0.00256 * std::cos(omega * DEG_TO_RAD);
}

/****************************/
/*   Sun Declination        */
/****************************/
static double sun_declination(double t) {
    const double e      = obliquity_corrected(t) * DEG_TO_RAD;
    const double lambda = sun_apparent_lon(t)    * DEG_TO_RAD;
    return std::asin(std::sin(e) * std::sin(lambda)) * RAD_TO_DEG;
}

/****************************/
/*   Equation of Time       */
/****************************/
static double equation_of_time(double t) {
    const double epsilon = obliquity_corrected(t)    * DEG_TO_RAD;
    const double l0      = sun_geo_mean_lon(t)       * DEG_TO_RAD;
    const double e       = earth_orbit_eccentricity(t);
    const double m       = sun_geo_mean_anomaly(t)   * DEG_TO_RAD;

    double y = std::tan(epsilon / 2.0);
    y *= y;

    return RAD_TO_DEG * 4.0 * (
          y          * std::sin(2.0 * l0)
        - 2.0 * e   * std::sin(m)
        + 4.0 * e   * y * std::sin(m) * std::cos(2.0 * l0)
        - 0.5 * y   * y * std::sin(4.0 * l0)
        - 1.25 * e  * e * std::sin(2.0 * m)
    );
}

/****************************/
/*   Hour Angle Sunrise     */
/****************************/
// Returns NaN for polar day/night conditions
static double hour_angle_sunrise(double lat_deg, double decl_deg) {
    const double lat_rad  = lat_deg  * DEG_TO_RAD;
    const double decl_rad = decl_deg * DEG_TO_RAD;
    // Solar zenith at horizon = 90.833° (accounts for refraction + solar disc radius)
    const double cos_ha = (std::cos(90.833 * DEG_TO_RAD) - std::sin(lat_rad) * std::sin(decl_rad))
                         / (std::cos(lat_rad) * std::cos(decl_rad));
    if (cos_ha < -1.0) return std::numeric_limits<double>::quiet_NaN();  // polar day
    if (cos_ha >  1.0) return std::numeric_limits<double>::quiet_NaN();  // polar night
    return std::acos(cos_ha) * RAD_TO_DEG;
}

} // namespace detail

/****************************/
/*   calculate_solar_times  */
/****************************/
Solar_Times calculate_solar_times(const std::tm& tm, const Solar_Location& loc) {
    using namespace detail;

    const int year  = tm.tm_year + 1900;
    const int month = tm.tm_mon  + 1;
    const int day   = tm.tm_mday;

    const double jd     = julian_day(year, month, day);
    const double t      = julian_century(jd);
    const double eqtime = equation_of_time(t);   // minutes
    const double decl   = sun_declination(t);     // degrees
    const double ha_sr  = hour_angle_sunrise(loc.latitude, decl);

    Solar_Times result{};
    result.is_polar_day   = false;
    result.is_polar_night = false;

    if (std::isnan(ha_sr)) {
        if ((decl > 0.0 && loc.latitude > 0.0) || (decl < 0.0 && loc.latitude < 0.0)) {
            result.is_polar_day = true;
        } else {
            result.is_polar_night = true;
        }
        result.sunrise_hour    = 0.0;
        result.sunset_hour     = 0.0;
        result.solar_noon_hour = 12.0;
        result.day_length_hour = result.is_polar_day ? 24.0 : 0.0;
    } else {
        const double noon_utc   = 720.0 - 4.0 * loc.longitude - eqtime;  // UTC minutes
        const double noon_local = noon_utc + loc.timezone * 60.0;         // local minutes

        result.solar_noon_hour = noon_local / 60.0;
        result.sunrise_hour    = (noon_local - ha_sr * 4.0) / 60.0;
        result.sunset_hour     = (noon_local + ha_sr * 4.0) / 60.0;
        result.day_length_hour = ha_sr * 8.0 / 60.0;
    }

    // Current solar elevation
    {
        const double current_minutes = tm.tm_hour * 60.0 + tm.tm_min + tm.tm_sec / 60.0;
        const double true_solar_time = current_minutes + eqtime + 4.0 * loc.longitude - 60.0 * loc.timezone;
        double hour_angle = true_solar_time / 4.0 - 180.0;
        if (hour_angle < -180.0) hour_angle += 360.0;

        const double lat_rad  = loc.latitude * DEG_TO_RAD;
        const double decl_rad = decl         * DEG_TO_RAD;
        const double ha_rad   = hour_angle   * DEG_TO_RAD;

        const double cos_zenith = std::sin(lat_rad) * std::sin(decl_rad)
                                + std::cos(lat_rad) * std::cos(decl_rad) * std::cos(ha_rad);
        result.solar_elevation = 90.0 - std::acos(std::max(-1.0, std::min(1.0, cos_zenith))) * RAD_TO_DEG;
    }

    return result;
}

/****************************/
/*    format_solar_time     */
/****************************/
std::string format_solar_time(double hours) {
    if (hours < 0.0 || hours >= 24.0) return "--:--";
    const int h = static_cast<int>(hours);
    const int m = static_cast<int>((hours - h) * 60.0 + 0.5);
    char buf[6];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
    return buf;
}

} // namespace ovb::core
