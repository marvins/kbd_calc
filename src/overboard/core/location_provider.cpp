/**
 * @file    location_provider.cpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   Geographic location resolution implementation
 */
#include <overboard/core/location_provider.hpp>

// C++ Standard Libraries
#include <array>
#include <cstdio>
#include <format>
#include <string>
#include <thread>

// Third-Party Libraries
#include <nlohmann/json.hpp>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::core {

namespace {

/****************************************/
/*      Parse ip-api.com JSON           */
/****************************************/
std::optional<Solar_Location> parse_ip_api_response(const std::string& body) {
    // Use non-throwing parse overload (no exceptions — Pico builds with -fno-exceptions)
    const auto j = nlohmann::json::parse(body, nullptr, false);
    if (j.is_discarded()) {
        LOG_ERROR("Failed to parse ip-api.com response as JSON");
        return std::nullopt;
    }

    if (j.value("status", std::string{}) != "success") {
        LOG_WARN("ip-api.com returned non-success status");
        return std::nullopt;
    }

    if (!j.contains("lat") || !j.contains("lon")) {
        LOG_WARN("ip-api.com response missing lat/lon");
        return std::nullopt;
    }

    Solar_Location loc{};
    loc.latitude  = j["lat"].get<double>();
    loc.longitude = j["lon"].get<double>();

    // ip-api.com free tier gives timezone as IANA name, not a UTC offset.
    // Derive the offset from the system's own localtime/gmtime instead.
    {
        const std::time_t now   = std::time(nullptr);
        const std::tm     local = *std::localtime(&now);
        const std::tm     utc   = *std::gmtime(&now);
        int diff = (local.tm_hour * 60 + local.tm_min)
                 - (utc.tm_hour   * 60 + utc.tm_min);
        if (diff >  720) diff -= 1440;
        if (diff < -720) diff += 1440;
        loc.timezone = diff / 60.0;
    }

    LOG_INFO("ip-api.com resolved: lat=", loc.latitude, ", lon=", loc.longitude, ", tz=", loc.timezone);
    return loc;
}

/****************************************/
/*      Fetch via curl subprocess       */
/****************************************/
std::optional<Solar_Location> fetch_ip_location(const std::string& url) {
    // Use popen to call curl — avoids libcurl dependency
    // Timeout kept short to allow fast application shutdown
    const std::string cmd = "curl -s --max-time 2 \"" + url + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        LOG_ERROR("Failed to launch curl for IP geolocation");
        return std::nullopt;
    }

    std::string result;
    std::array<char, 256> buf{};
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe)) {
        result += buf.data();
    }
    pclose(pipe);

    if (result.empty()) {
        LOG_WARN("ip-api.com returned empty response");
        return std::nullopt;
    }

    return parse_ip_api_response(result);
}

} // anonymous namespace

/****************************************/
/*      location_from_settings          */
/****************************************/
std::optional<Solar_Location> location_from_settings(const hal::Settings_Tree& tree) {
    const auto lat = tree.get<float>("location.latitude");
    const auto lon = tree.get<float>("location.longitude");
    const auto tz  = tree.get<float>("location.timezone");

    if (!lat || !lon || !tz) {
        return std::nullopt;
    }
    return Solar_Location { static_cast<double>(*lat),
                            static_cast<double>(*lon),
                            static_cast<double>(*tz) };
}

/****************************************/
/*         location_source              */
/****************************************/
std::string location_source(const hal::Settings_Tree& tree) {
    return tree.get<std::string>("location.source", std::string("settings"));
}

/****************************************/
/*      resolve_location_async          */
/****************************************/
void resolve_location_async(const hal::Settings_Tree& tree, Location_Cb cb) {
    const std::string source = location_source(tree);

    // Try settings first regardless of source — if fully populated, use it
    if (source == "settings") {
        if (const auto loc = location_from_settings(tree)) {
            LOG_INFO("Location resolved from settings: lat=", loc->latitude, ", lon=", loc->longitude);
            cb(*loc);
            return;
        }
        LOG_WARN("location.source=settings but lat/lon/timezone missing — falling back to IP");
    }

    // IP geolocation — launch background thread (not supported in WASM)
    // Capture url by value to avoid dangling reference to tree
    const std::string url = tree.get<std::string>("location.ip_url",
                                std::string("http://ip-api.com/json"));
#ifdef __EMSCRIPTEN__
    // WASM: single-threaded; resolve synchronously
    if (auto loc = fetch_ip_location(url)) {
        cb(*loc);
    } else {
        LOG_WARN("IP geolocation failed, using default location (Denver, CO)");
        cb(DEFAULT_LOCATION);
    }
#else
    std::thread([cb, url]() {
        if (auto loc = fetch_ip_location(url)) {
            cb(*loc);
        } else {
            LOG_WARN("IP geolocation failed, using default location (Denver, CO)");
            cb(DEFAULT_LOCATION);
        }
    }).detach();
#endif
}

} // namespace ovb::core
