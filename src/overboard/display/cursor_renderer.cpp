/**
 * @file    cursor_renderer.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Cursor renderer implementation
 */
#include <overboard/display/cursor_renderer.hpp>

namespace ovb::display {

/*******************************************/
/*           Constructor                   */
/*******************************************/
Cursor_Renderer::Cursor_Renderer()
    : style_(Cursor_Style::Vertical_Bar)
    , visible_(true) {
}

/*******************************************/
/*            Render Cursor                */
/*******************************************/
void Cursor_Renderer::render(I_Canvas&                       canvas,
                             const math::layout::Layout_Box& box,
                             uint32_t                        color) {
    if (!visible_) {
        return;
    }

    core::Rect<int> cursor_rect;

    switch (style_) {
        case Cursor_Style::Block:
            // Fill entire box area
            cursor_rect = core::Rect<int>{
                box.pos.x,
                box.pos.y,
                box.size.x,
                box.size.y
            };
            canvas.fill_rect(cursor_rect, color);
            break;

        case Cursor_Style::Underline:
            // Line at bottom of box (2px high)
            cursor_rect = core::Rect<int>{
                box.pos.x,
                box.pos.y + box.size.y - 2,
                box.size.x,
                2
            };
            canvas.fill_rect(cursor_rect, color);
            break;

        case Cursor_Style::Vertical_Bar:
            // Line at left edge of box (2px wide)
            cursor_rect = core::Rect<int>{
                box.pos.x,
                box.pos.y,
                2,
                box.size.y
            };
            canvas.fill_rect(cursor_rect, color);
            break;
    }

    canvas.mark_dirty(cursor_rect);
}

/*******************************************/
/*           Set Style                     */
/*******************************************/
void Cursor_Renderer::set_style(Cursor_Style style) {
    style_ = style;
}

/*******************************************/
/*           Get Style                     */
/*******************************************/
Cursor_Style Cursor_Renderer::style() const {
    return style_;
}

/*******************************************/
/*         Set Visible                     */
/*******************************************/
void Cursor_Renderer::set_visible(bool visible) {
    visible_ = visible;
}

/*******************************************/
/*         Get Visible                     */
/*******************************************/
bool Cursor_Renderer::visible() const {
    return visible_;
}

} // namespace ovb::display

