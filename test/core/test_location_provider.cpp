/**
 * @file    test_location_provider.cpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   Unit tests for location_provider and solar_calc
 */
#include <gtest/gtest.h>

// C++ Standard Libraries
#include <cmath>
#include <sstream>
#include <string>

// Third-Party Libraries
#include <tomlplusplus/toml.hpp>

// Project Libraries
#include <overboard/core/location_provider.hpp>
#include <overboard/core/solar_calc.hpp>
#include <overboard/hal/settings_tree.hpp>

using namespace ovb::core;
using namespace ovb::hal;

/*************************************/
/*   location_from_settings tests    */
/*************************************/
TEST(Location_Provider, Settings_Full_Block_Parsed) {
    std::string toml = R"(
[location]
source    = "settings"
latitude  = 39.7392
longitude = -104.9903
timezone  = -6.0
)";
    std::istringstream iss(toml);
    Settings_Tree tree(toml::parse(iss));

    const auto loc = location_from_settings(tree);
    ASSERT_TRUE(loc.has_value());
    EXPECT_NEAR(loc->latitude,  39.7392,   1e-4);
    EXPECT_NEAR(loc->longitude, -104.9903, 1e-4);
    EXPECT_NEAR(loc->timezone,  -6.0,      1e-6);
}

TEST(Location_Provider, Settings_Missing_Latitude_Returns_Nullopt) {
    std::string toml = R"(
[location]
longitude = -104.9903
timezone  = -6.0
)";
    std::istringstream iss(toml);
    Settings_Tree tree(toml::parse(iss));

    EXPECT_FALSE(location_from_settings(tree).has_value());
}

TEST(Location_Provider, Settings_Missing_Longitude_Returns_Nullopt) {
    std::string toml = R"(
[location]
latitude = 39.7392
timezone = -6.0
)";
    std::istringstream iss(toml);
    Settings_Tree tree(toml::parse(iss));

    EXPECT_FALSE(location_from_settings(tree).has_value());
}

TEST(Location_Provider, Settings_Missing_Timezone_Returns_Nullopt) {
    std::string toml = R"(
[location]
latitude  = 39.7392
longitude = -104.9903
)";
    std::istringstream iss(toml);
    Settings_Tree tree(toml::parse(iss));

    EXPECT_FALSE(location_from_settings(tree).has_value());
}

TEST(Location_Provider, Settings_Empty_Tree_Returns_Nullopt) {
    Settings_Tree tree;
    EXPECT_FALSE(location_from_settings(tree).has_value());
}

/*************************************/
/*   location_source tests           */
/*************************************/
TEST(Location_Provider, Source_Defaults_To_Settings) {
    Settings_Tree tree;
    EXPECT_EQ(location_source(tree), "settings");
}

TEST(Location_Provider, Source_Reads_IP) {
    std::string toml = R"(
[location]
source = "ip"
)";
    std::istringstream iss(toml);
    Settings_Tree tree(toml::parse(iss));
    EXPECT_EQ(location_source(tree), "ip");
}

TEST(Location_Provider, Source_Reads_Settings_Explicit) {
    std::string toml = R"(
[location]
source = "settings"
)";
    std::istringstream iss(toml);
    Settings_Tree tree(toml::parse(iss));
    EXPECT_EQ(location_source(tree), "settings");
}

/*************************************/
/*   solar_calc tests                */
/*************************************/
TEST(Solar_Calc, Denver_Summer_Solstice_Sunrise_Before_Noon) {
    // 2026-06-21, Denver CO
    std::tm tm{};
    tm.tm_year = 126;  // 2026
    tm.tm_mon  = 5;    // June
    tm.tm_mday = 21;
    tm.tm_hour = 12;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;

    const Solar_Location denver { 39.7392, -104.9903, -6.0 };
    const Solar_Times result = calculate_solar_times(tm, denver);

    EXPECT_FALSE(result.is_polar_day);
    EXPECT_FALSE(result.is_polar_night);
    EXPECT_LT(result.sunrise_hour,    result.solar_noon_hour);
    EXPECT_LT(result.solar_noon_hour, result.sunset_hour);
}

TEST(Solar_Calc, Denver_Summer_Solstice_Day_Length_Reasonable) {
    std::tm tm{};
    tm.tm_year = 126;
    tm.tm_mon  = 5;
    tm.tm_mday = 21;
    tm.tm_hour = 12;

    const Solar_Location denver { 39.7392, -104.9903, -6.0 };
    const Solar_Times result = calculate_solar_times(tm, denver);

    // Denver summer solstice: ~14.5 hours of daylight
    EXPECT_GT(result.day_length_hour, 14.0);
    EXPECT_LT(result.day_length_hour, 16.0);
}

TEST(Solar_Calc, Denver_Winter_Solstice_Day_Length_Shorter) {
    std::tm tm{};
    tm.tm_year = 126;
    tm.tm_mon  = 11;  // December
    tm.tm_mday = 21;
    tm.tm_hour = 12;

    const Solar_Location denver { 39.7392, -104.9903, -7.0 };  // MST
    const Solar_Times result = calculate_solar_times(tm, denver);

    // Denver winter solstice: ~9.5 hours of daylight
    EXPECT_GT(result.day_length_hour, 9.0);
    EXPECT_LT(result.day_length_hour, 11.0);
}

TEST(Solar_Calc, Noon_Elevation_Positive_In_Summer) {
    std::tm tm{};
    tm.tm_year = 126;
    tm.tm_mon  = 5;
    tm.tm_mday = 21;
    tm.tm_hour = 12;
    tm.tm_min  = 30;  // Approximate solar noon for Denver

    const Solar_Location denver { 39.7392, -104.9903, -6.0 };
    const Solar_Times result = calculate_solar_times(tm, denver);

    EXPECT_GT(result.solar_elevation, 0.0);
}

TEST(Solar_Calc, Midnight_Elevation_Negative) {
    std::tm tm{};
    tm.tm_year = 126;
    tm.tm_mon  = 5;
    tm.tm_mday = 21;
    tm.tm_hour = 0;
    tm.tm_min  = 0;

    const Solar_Location denver { 39.7392, -104.9903, -6.0 };
    const Solar_Times result = calculate_solar_times(tm, denver);

    EXPECT_LT(result.solar_elevation, 0.0);
}

TEST(Solar_Calc, Arctic_Summer_Polar_Day) {
    // North Pole in June — sun never sets
    std::tm tm{};
    tm.tm_year = 126;
    tm.tm_mon  = 5;
    tm.tm_mday = 21;
    tm.tm_hour = 12;

    const Solar_Location north_pole { 90.0, 0.0, 0.0 };
    const Solar_Times result = calculate_solar_times(tm, north_pole);

    EXPECT_TRUE(result.is_polar_day);
    EXPECT_FALSE(result.is_polar_night);
    EXPECT_NEAR(result.day_length_hour, 24.0, 1e-6);
}

TEST(Solar_Calc, Format_Solar_Time_Midnight) {
    EXPECT_EQ(format_solar_time(0.0),  "00:00");
}

TEST(Solar_Calc, Format_Solar_Time_Noon) {
    EXPECT_EQ(format_solar_time(12.0), "12:00");
}

TEST(Solar_Calc, Format_Solar_Time_Half_Past_Six) {
    EXPECT_EQ(format_solar_time(6.5),  "06:30");
}

TEST(Solar_Calc, Format_Solar_Time_Invalid_Negative) {
    EXPECT_EQ(format_solar_time(-1.0), "--:--");
}

TEST(Solar_Calc, Format_Solar_Time_Invalid_Over_24) {
    EXPECT_EQ(format_solar_time(25.0), "--:--");
}

/*************************************/
/*   Sample config file test         */
/*************************************/
TEST(Location_Provider, Sample_Config_File_Has_Location_Block) {
    const std::string config_path =
        "/Users/marvin/Desktop/Projects/kbd_calc/data/configs/settings.toml";

    if (!std::filesystem::exists(config_path)) {
        GTEST_SKIP() << "Sample config not found at " << config_path;
    }

    toml::table table = toml::parse_file(config_path);
    Settings_Tree tree(table);

    // Source field must be present and valid
    const std::string source = location_source(tree);
    EXPECT_TRUE(source == "settings" || source == "ip");

    // If source is "settings", coordinates must be parseable
    if (source == "settings") {
        const auto loc = location_from_settings(tree);
        ASSERT_TRUE(loc.has_value()) << "source=settings but lat/lon/timezone missing";
        EXPECT_GE(loc->latitude,  -90.0);
        EXPECT_LE(loc->latitude,   90.0);
        EXPECT_GE(loc->longitude, -180.0);
        EXPECT_LE(loc->longitude,  180.0);
        EXPECT_GE(loc->timezone,  -14.0);
        EXPECT_LE(loc->timezone,   14.0);
    }
}
