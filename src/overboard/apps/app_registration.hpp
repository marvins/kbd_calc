/**
 * @file    app_registration.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Central application registration
 *
 * Single location to register all user-facing applications.
 * Modify this file when adding new apps to the system.
 */
#pragma once

// C++ Standard Libraries
#include <functional>

// Project Libraries
#include <overboard/apps/calculator/calculator_app.hpp>
#include <overboard/apps/settings/settings_page.hpp>
#include <overboard/apps/status/status_page.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/app_registry.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::apps {

// Panel indices (must match registration order in register_all_apps())
// These are used by App_View for panel management
inline constexpr int PANEL_STATUS     { 0 };  // First app registered
inline constexpr int PANEL_CALCULATOR   { 1 };  // Second app registered
inline constexpr int PANEL_SETTINGS     { 2 };  // Third app registered
inline constexpr int PANEL_MENU         { 3 };  // Not an app, created separately

/**
 * @brief Register all applications with the registry
 *
 * Central registration point for all user-facing apps.
 * Priority determines menu order (lower = first).
 *
 * @param registry      App registry to register with
 * @param engine        Calculator engine (for Calculator_App)
 * @param layers        Layer manager (for Calculator_App, Status_Page)
 * @param panels        Panel manager for navigation callbacks
 */
inline void register_all_apps(
    gui::App_Registry& registry,
    math::Calc_Engine& engine,
    core::Layer_Manager& layers,
    gui::Panel_Manager& panels)
{
    // Auto-incrementing priority - just add apps here in desired menu order
    int priority = 0;

    // Status - shown first in menu
    // ESCAPE returns to menu (pop)
    registry.register_app(
        "Status", LV_SYMBOL_FILE, priority++, 's',
        [&layers, &panels]() mutable {
            return std::make_shared<gui::Status_Page>(layers, [&panels]() {
                panels.pop();
            });
        });

    // Calculator - second in menu
    // ESCAPE returns to menu (pop)
    registry.register_app(
        "Calculator", LV_SYMBOL_KEYBOARD, priority++, 'c',
        [&engine, &layers, &panels]() mutable {
            return std::make_shared<gui::Calculator_App>(engine, layers, [&panels]() {
                panels.pop();
            });
        });

    // Settings - third in menu
    // ESCAPE returns to menu (pop)
    registry.register_app(
        "Settings", LV_SYMBOL_SETTINGS, priority++, 'g',
        [&panels]() mutable {
            return std::make_shared<gui::Settings_Page>([&panels]() {
                panels.pop();
            });
        });
}

} // namespace ovb::apps
