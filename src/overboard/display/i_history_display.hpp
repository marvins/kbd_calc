/**
 * @file    i_history_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   History display interface
 *
 * Manages the scrollable history of previous calculations.
 * Each entry shows the expression and its result.
 */
#pragma once

// C++ Standard Libraries
#include <cstddef>
#include <vector>

// Project Libraries
#include <overboard/display/i_canvas.hpp>
#include <overboard/display/i_display_manager.hpp>

namespace ovb::display {

/**
 * @brief History entry for display
 */
struct History_Entry {
    std::string expression;  ///< Expression string (rendered form)
    std::string result;      ///< Result string
    bool        is_symbolic; ///< Whether result is symbolic
};

/**
 * @brief History display interface
 *
 * Renders a scrollable list of previous calculations.
 * Supports scrolling, entry selection, and configurable
 * maximum entries.
 */
class I_History_Display {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_History_Display() = default;

        /**
         * @brief Add entry to history
         * @param entry History entry to add
         */
        virtual void add_entry(const History_Entry& entry) = 0;

        /**
         * @brief Remove oldest entry if at capacity
         */
        virtual void remove_oldest() = 0;

        /**
         * @brief Clear all history entries
         */
        virtual void clear() = 0;

        /**
         * @brief Render history to canvas
         * @param canvas Target canvas
         * @param rect Display region for history
         */
        virtual void render(I_Canvas&              canvas,
                            const core::Rect<int>& rect) = 0;

        /**
         * @brief Set maximum history entries
         * @param max_entries Maximum entries to retain
         */
        virtual void set_max_entries(std::size_t max_entries) = 0;

        /**
         * @brief Get maximum history entries
         * @return Maximum entry count
         */
        virtual std::size_t max_entries() const = 0;

        /**
         * @brief Get current entry count
         * @return Number of entries in history
         */
        virtual std::size_t entry_count() const = 0;

        /**
         * @brief Scroll up (show older entries)
         * @param lines Number of lines to scroll
         */
        virtual void scroll_up(std::size_t lines = 1) = 0;

        /**
         * @brief Scroll down (show newer entries)
         * @param lines Number of lines to scroll
         */
        virtual void scroll_down(std::size_t lines = 1) = 0;

        /**
         * @brief Check if can scroll up
         * @return true if more entries above
         */
        virtual bool can_scroll_up() const = 0;

        /**
         * @brief Check if can scroll down
         * @return true if more entries below
         */
        virtual bool can_scroll_down() const = 0;
};

} // namespace ovb::display

