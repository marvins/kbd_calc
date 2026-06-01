/**
 * @file    dirty_region_tracker.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Dirty region tracking for efficient display updates
 *
 * Tracks modified screen regions to minimize redraw work.
 * Implements rectangle coalescing to reduce update overhead.
 */
#pragma once

// C++ Standard Libraries
#include <vector>

// Project Libraries
#include <overboard/core/rect.hpp>

namespace ovb::display {

/**
 * @brief Tracks dirty screen regions for incremental updates
 *
 * Accumulates dirty rectangles and coalesces overlapping regions
 * to minimize the number of update areas. Used by I_Display_Manager
 * to optimize rendering.
 */
class Dirty_Region_Tracker {
    public:
        /**
         * @brief Mark a region as dirty
         * @param region Dirty rectangle
         */
        void mark_dirty(const core::Rect<int>& region);

        /**
         * @brief Mark entire canvas as dirty
         * @param canvas_size Full canvas dimensions
         */
        void mark_all_dirty(const core::Point<int>& canvas_size);

        /**
         * @brief Get list of dirty regions (coalesced)
         * @return Vector of non-overlapping dirty rectangles
         */
        std::vector<core::Rect<int>> dirty_regions() const;

        /**
         * @brief Check if any dirty regions exist
         * @return true if regions pending update
         */
        bool has_dirty_regions() const;

        /**
         * @brief Clear all dirty regions
         */
        void clear();

        /**
         * @brief Get bounding box of all dirty regions
         * @return Bounding rectangle, or empty if no regions
         */
        core::Rect<int> bounding_box() const;

    private:
        std::vector<core::Rect<int>> m_regions_;  ///< Accumulated dirty regions

        /**
         * @brief Coalesce overlapping rectangles
         *
         * Merges rectangles that overlap or touch to reduce count.
         */
        void coalesce_regions();

        /**
         * @brief Check if two rectangles should be merged
         * @param a First rectangle
         * @param b Second rectangle
         * @return true if overlapping or adjacent
         */
        bool should_merge(const core::Rect<int>& a,
                          const core::Rect<int>& b) const;

        /**
         * @brief Merge two rectangles into bounding box
         * @param a First rectangle
         * @param b Second rectangle
         * @return Bounding box containing both
         */
        core::Rect<int> merge(const core::Rect<int>& a,
                              const core::Rect<int>& b) const;
};

} // namespace ovb::display

