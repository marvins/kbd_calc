/**
 * @file    solar_calc.hpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   NOAA-based solar position and sunrise/sunset calculator
 *
 * Implements the NOAA Solar Calculator algorithm for computing sunrise,
 * solar noon, and sunset times for a given date and geographic location.
 * Based on the publicly available NOAA algorithm:
 * https://www.esrl.noaa.gov/gmd/grad/solcalc/
 */
#pragma once

// C++ Standard Libraries
#include <ctime>
#include <string>

namespace ovb::core {

/**
 * @brief Result of a solar calculation
 */
struct Solar_Times {
    double sunrise_hour;    ///< Sunrise as fractional hours (local)
    double solar_noon_hour; ///< Solar noon as fractional hours (local)
    double sunset_hour;     ///< Sunset as fractional hours (local)
    double day_length_hour; ///< Day length in hours
    double solar_elevation; ///< Current solar elevation in degrees
    bool   is_polar_day;    ///< Sun never sets
    bool   is_polar_night;  ///< Sun never rises
};

/**
 * @brief Geographic location for solar calculations
 */
struct Solar_Location {
    double latitude;   ///< Latitude in degrees (positive = north)
    double longitude;  ///< Longitude in degrees (positive = east)
    double timezone;   ///< UTC offset in hours (e.g. -7 for MDT)
};

/**
 * @brief Calculate solar times for a given date and location
 *
 * @param tm   Local time struct (year/month/day used for solar; hour/min/sec used for elevation)
 * @param loc  Geographic location
 * @return Solar_Times with all computed values
 */
Solar_Times calculate_solar_times(const std::tm& tm, const Solar_Location& loc);

/**
 * @brief Format fractional hours as HH:MM string
 *
 * Returns "--:--" for values outside [0, 24).
 */
std::string format_solar_time(double hours);

} // namespace ovb::core
