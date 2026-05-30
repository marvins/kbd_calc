/**
 * @file    i_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Platform-agnostic display interface
 *
 * Implemented by both the SDL simulator display and the embedded
 * hardware display. All rendering is assumed to be LVGL-driven;
 * this interface manages the display lifecycle rather than
 * individual pixel operations.
 */
#pragma once

namespace ovb::hal {

/**
 * @brief Platform display interface
 *
 * Abstracts the unified display (LCD + keyboard) across targets.
 * Implementations are responsible for LVGL widget management and
 * driving the render loop.
 */
class I_Display {
    public:

        /**
         * @brief Virtual destructor
         */
        virtual ~I_Display() = default;

        /**
         * @brief Refresh the LCD section from current engine state
         */
        virtual void refresh() = 0;

        /**
         * @brief Re-render keyboard buttons for the current layer
         */
        virtual void update_layer() = 0;

        /**
         * @brief Drive the LVGL render loop (call once per frame)
         */
        virtual void render() = 0;
};

} // namespace ovb::hal
