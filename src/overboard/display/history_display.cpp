/**
 * @file    history_display.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   History display implementation
 */
#include <overboard/display/history_display.hpp>

// C++ Standard Libraries
#include <algorithm>

namespace ovb::display {

/*******************************************/
/*           Constructor                   */
/*******************************************/
History_Display::History_Display(std::size_t max_entries)
    : entries_()
    , max_entries_(max_entries)
    , scroll_offset_(0) {
}

/*******************************************/
/*           Add Entry                     */
/*******************************************/
void History_Display::add_entry(const History_Entry& entry) {
    entries_.push_back(entry);
    enforce_limit();

    // Auto-scroll to show newest entry
    if (entries_.size() > 1) {
        scroll_offset_ = 0;
    }
}

/*******************************************/
/*          Remove Oldest                  */
/*******************************************/
void History_Display::remove_oldest() {
    if (!entries_.empty()) {
        entries_.pop_front();
    }
}

/*******************************************/
/*            Clear                        */
/*******************************************/
void History_Display::clear() {
    entries_.clear();
    scroll_offset_ = 0;
}

/*******************************************/
/*            Render                       */
/*******************************************/
void History_Display::render(I_Canvas&              canvas,
                             const core::Rect<int>& rect) {
    if (entries_.empty()) {
        return;
    }

    // Clear history area
    uint32_t bg_color = 0x222222;  // Dark gray background
    canvas.fill_rect(rect, bg_color);

    // Render visible entries
    int line_height = 20;
    std::size_t max_visible = static_cast<std::size_t>(rect.h / line_height);
    int font_size = 14;
    uint32_t text_color = 0xCCCCCC;  // Light gray text

    std::size_t start = scroll_offset_;
    std::size_t end = std::min(start + max_visible, entries_.size());

    for (std::size_t i = start; i < end; ++i) {
        const auto& entry = entries_[i];
        int y = rect.y + static_cast<int>(i - start) * line_height;

        // Format: "expression = result"
        std::string line = entry.expression + " = " + entry.result;

        core::Point<int> pos{rect.x + 5, y};
        canvas.draw_text(line, pos, font_size, text_color);
    }

    canvas.mark_dirty(rect);
}

/*******************************************/
/*         Set Max Entries                 */
/*******************************************/
void History_Display::set_max_entries(std::size_t max_entries) {
    max_entries_ = max_entries;
    enforce_limit();
}

/*******************************************/
/*         Get Max Entries                 */
/*******************************************/
std::size_t History_Display::max_entries() const {
    return max_entries_;
}

/*******************************************/
/*         Get Entry Count                 */
/*******************************************/
std::size_t History_Display::entry_count() const {
    return entries_.size();
}

/*******************************************/
/*          Scroll Up                      */
/*******************************************/
void History_Display::scroll_up(std::size_t lines) {
    if (scroll_offset_ + lines < entries_.size()) {
        scroll_offset_ += lines;
    } else {
        scroll_offset_ = entries_.size() - 1;
    }
}

/*******************************************/
/*          Scroll Down                    */
/*******************************************/
void History_Display::scroll_down(std::size_t lines) {
    if (lines <= scroll_offset_) {
        scroll_offset_ -= lines;
    } else {
        scroll_offset_ = 0;
    }
}

/*******************************************/
/*        Can Scroll Up                    */
/*******************************************/
bool History_Display::can_scroll_up() const {
    return scroll_offset_ < entries_.size() - 1;
}

/*******************************************/
/*        Can Scroll Down                  */
/*******************************************/
bool History_Display::can_scroll_down() const {
    return scroll_offset_ > 0;
}

/*******************************************/
/*         Enforce Limit                   */
/*******************************************/
void History_Display::enforce_limit() {
    while (entries_.size() > max_entries_) {
        entries_.pop_front();
    }
}

} // namespace ovb::display

