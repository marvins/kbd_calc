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
#include <vector>
#include <utility>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/font/font_metrics.hpp>
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
    lv_obj_t*     bezel          = nullptr;
    lv_obj_t*     table          = nullptr;
    lv_obj_t*     preview_canvas = nullptr;
    std::vector<std::pair<lv_obj_t*, lv_obj_t*>> history_cells;
    std::vector<uint32_t> canvas_buf;  ///< Canvas buffer for expression preview (RAII-managed)

    const math::Calc_Engine&   engine;
    const core::Layer_Manager& layers;
    math::layout::Layout_Engine      layout_engine{font::Font_Metrics::make_default(), 2};

    /**
     * @brief Construct LCD section
     * @param e Reference to calculation engine for expression state
     * @param l Reference to layer manager for display configuration
     */
    LCD_Section(const math::Calc_Engine& e, const core::Layer_Manager& l)
        : engine(e), layers(l) {}

    void build(lv_obj_t* parent);
    void refresh();
};

} // namespace ovb::gui
