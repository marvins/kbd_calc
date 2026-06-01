/**
 * @file    history_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   History display implementation
 */
#pragma once

// C++ Standard Libraries
#include <deque>

// Project Libraries
#include <overboard/display/i_history_display.hpp>

namespace ovb::display {

/**
 * @brief Standard history display
 *
 * Manages a scrollable list of previous calculations
 * with configurable maximum entries.
 */
class History_Display : public I_History_Display {
    public:
        /**
         * @brief Construct with default max entries
         * @param max_entries Maximum history entries (default 50)
         */
        explicit History_Display(std::size_t max_entries = 50);

        /**
         * @brief Add entry to history
         */
        void add_entry(const History_Entry& entry) override;

        /**
         * @brief Remove oldest entry
         */
        void remove_oldest() override;

        /**
         * @brief Clear all history entries
         */
        void clear() override;

        /**
         * @brief Render history to canvas
         */
        void render(I_Canvas&              canvas,
                    const core::Rect<int>& rect) override;

        /**
         * @brief Set maximum history entries
         */
        void set_max_entries(std::size_t max_entries) override;

        /**
         * @brief Get maximum history entries
         */
        std::size_t max_entries() const override;

        /**
         * @brief Get current entry count
         */
        std::size_t entry_count() const override;

        /**
         * @brief Scroll up
         */
        void scroll_up(std::size_t lines) override;

        /**
         * @brief Scroll down
         */
        void scroll_down(std::size_t lines) override;

        /**
         * @brief Check if can scroll up
         */
        bool can_scroll_up() const override;

        /**
         * @brief Check if can scroll down
         */
        bool can_scroll_down() const override;

    private:
        std::deque<History_Entry> entries_;      ///< History entries
        std::size_t               max_entries_;  ///< Maximum entries
        std::size_t               scroll_offset_; ///< First visible entry

        /**
         * @brief Enforce maximum entry limit
         */
        void enforce_limit();
};

} // namespace ovb::display

