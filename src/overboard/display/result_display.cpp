/**
 * @file    result_display.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Result display implementation
 */
#include <overboard/display/result_display.hpp>

// C++ Standard Libraries
#include <algorithm>

namespace ovb::display {

/*******************************************/
/*           Constructor                   */
/*******************************************/
Result_Display::Result_Display()
    : align_(Result_Align::Right)
    , max_chars_(50)
    , last_result_() {
}

/*******************************************/
/*              Show Result                */
/*******************************************/
void Result_Display::show(I_Canvas&          canvas,
                          const std::string& result,
                          bool               /*is_symbolic*/) {
    last_result_ = result;

    auto canvas_size = canvas.size();
    std::string display_text = truncate(result);

    // Calculate position based on alignment
    // For now, use a simple fixed font size
    int font_size = 16;
    int text_width = static_cast<int>(static_cast<double>(display_text.length()) * font_size * 0.6);
    int text_height = font_size;

    core::Point<int> pos;
    switch (align_) {
        case Result_Align::Left:
            pos = core::Point<int>{10, canvas_size.y - text_height - 10};
            break;
        case Result_Align::Center:
            pos = core::Point<int>{(canvas_size.x - text_width) / 2, canvas_size.y - text_height - 10};
            break;
        case Result_Align::Right:
            pos = core::Point<int>{canvas_size.x - text_width - 10, canvas_size.y - text_height - 10};
            break;
    }

    // Draw result text (white color)
    uint32_t color = 0xFFFFFF;
    canvas.draw_text(display_text, pos, font_size, color);

    // Mark region as dirty
    core::Rect<int> region{pos.x, pos.y, text_width, text_height};
    canvas.mark_dirty(region);
}

/*******************************************/
/*            Clear Result                 */
/*******************************************/
void Result_Display::clear(I_Canvas& canvas) {
    last_result_.clear();

    auto canvas_size = canvas.size();

    // Clear result area (bottom portion of canvas)
    int result_height = 30;
    core::Rect<int> region{0, canvas_size.y - result_height, canvas_size.x, result_height};

    uint32_t bg_color = 0x000000;  // Black background
    canvas.fill_rect(region, bg_color);
    canvas.mark_dirty(region);
}

/*******************************************/
/*          Set Alignment                  */
/*******************************************/
void Result_Display::set_alignment(Result_Align align) {
    align_ = align;
}

/*******************************************/
/*          Get Alignment                  */
/*******************************************/
Result_Align Result_Display::alignment() const {
    return align_;
}

/*******************************************/
/*         Set Max Width                   */
/*******************************************/
void Result_Display::set_max_width(std::size_t max_chars) {
    max_chars_ = max_chars;
}

/*******************************************/
/*         Get Max Width                   */
/*******************************************/
std::size_t Result_Display::max_width() const {
    return max_chars_;
}

/*******************************************/
/*           Truncate                    */
/*******************************************/
std::string Result_Display::truncate(const std::string& result) const {
    if (result.length() <= max_chars_) {
        return result;
    }

    // Truncate with ellipsis
    std::string truncated = result.substr(0, max_chars_ - 3);
    truncated += "...";
    return truncated;
}

} // namespace ovb::display

