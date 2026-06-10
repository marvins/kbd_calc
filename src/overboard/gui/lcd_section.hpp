/**
 * @file    lcd_section.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   LCD section of the merged display
 *
 * Manages the top portion of the display containing:
 * - Bezel container with styling
 * - History table for previous calculations
 * - Canvas for typeset mathematical expression preview
 * Platform-agnostic — works on both SDL simulator and embedded targets.
 */
#pragma once

// C++ Standard Libraries
#include <optional>
#include <utility>
#include <vector>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/font/font_metrics.hpp>
#include <overboard/gui/lvgl_theme.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/math/calc_engine.hpp>
#include <overboard/math/layout/engine.hpp>

namespace ovb::gui {

/**
 * @brief LCD section within the merged display
 *
 * Manages the top portion of the display containing:
 * - Bezel container with styling
 * - History table for previous calculations
 * - Canvas for typeset mathematical expression preview
 */
struct LCD_Section {

    /// @brief LVGL object for the bezel container
    lv_obj_t*     bezel          = nullptr;

    /// @brief LVGL object for the history table
    lv_obj_t*     table          = nullptr;

    /// @brief LVGL object for the preview canvas
    lv_obj_t*     preview_canvas = nullptr;

    /// @brief Vector of history cells (expression and result)
    std::vector<std::pair<lv_obj_t*, lv_obj_t*>> history_cells;

    /// @brief Canvas buffer for expression preview (RAII-managed)
    std::vector<uint32_t> canvas_buf;  ///< Canvas buffer for expression preview (RAII-managed)

    /// @brief Reference to calculation engine for expression state
    const math::Calc_Engine&   engine;

    /// @brief Reference to layer manager for display configuration
    const core::Layer_Manager& layers;

    /// @brief Layout engine for typesetting mathematical expressions
    /// Initialized in build() after LVGL fonts are ready
    std::optional<math::layout::Layout_Engine> layout_engine;

    /**
     * @brief Construct LCD section
     * @param e Reference to calculation engine for expression state
     * @param l Reference to layer manager for display configuration
     */
    LCD_Section(const math::Calc_Engine& e, const core::Layer_Manager& l)
        : engine(e), layers(l) {}

    /**
     * @brief Build the LCD section UI
     * @param parent Parent LVGL object
     * @param width Width of the section
     * @param height Height of the section
     */
    void build(lv_obj_t* parent, int width, int height);

    /**
     * @brief Refresh the LCD section UI
     */
    void refresh();
};

} // namespace ovb::gui
