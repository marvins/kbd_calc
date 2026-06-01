/**
 * @file    cursor_renderer.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Cursor renderer implementation
 */
#pragma once

// Project Libraries
#include <overboard/display/i_cursor_renderer.hpp>

namespace ovb::display {

/**
 * @brief Standard cursor renderer
 *
 * Implements block, underline, and vertical bar cursor styles.
 */
class Cursor_Renderer : public I_Cursor_Renderer {
    public:
        /**
         * @brief Default constructor
         */
        Cursor_Renderer();

        /**
         * @brief Render cursor at box position
         */
        void render(I_Canvas&                       canvas,
                    const math::layout::Layout_Box& box,
                    uint32_t                        color) override;

        /**
         * @brief Set cursor style
         */
        void set_style(Cursor_Style style) override;

        /**
         * @brief Get current cursor style
         */
        Cursor_Style style() const override;

        /**
         * @brief Set cursor blink state
         */
        void set_visible(bool visible) override;

        /**
         * @brief Check if cursor is currently visible
         */
        bool visible() const override;

    private:
        Cursor_Style style_;  ///< Current cursor style
        bool         visible_; ///< Current visibility state
};

} // namespace ovb::display

