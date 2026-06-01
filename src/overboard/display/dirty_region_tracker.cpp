/**
 * @file    dirty_region_tracker.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Dirty region tracker implementation
 */
#include <overboard/display/dirty_region_tracker.hpp>

// C++ Standard Libraries
#include <algorithm>

namespace ovb::display {

/*******************************************/
/*         Mark Region as Dirty            */
/*******************************************/
void Dirty_Region_Tracker::mark_dirty(const core::Rect<int>& region) {
    if (region.empty()) {
        return;
    }
    m_regions_.push_back(region);
    coalesce_regions();
}

/*******************************************/
/*         Mark All as Dirty               */
/*******************************************/
void Dirty_Region_Tracker::mark_all_dirty(const core::Point<int>& canvas_size) {
    m_regions_.clear();
    m_regions_.push_back(core::Rect<int>{0, 0, canvas_size.x, canvas_size.y});
}

/*******************************************/
/*           Get Dirty Regions             */
/*******************************************/
std::vector<core::Rect<int>> Dirty_Region_Tracker::dirty_regions() const {
    return m_regions_;
}

/*******************************************/
/*         Check Has Dirty Regions         */
/*******************************************/
bool Dirty_Region_Tracker::has_dirty_regions() const {
    return !m_regions_.empty();
}

/*******************************************/
/*            Clear Regions                */
/*******************************************/
void Dirty_Region_Tracker::clear() {
    m_regions_.clear();
}

/*******************************************/
/*          Get Bounding Box               */
/*******************************************/
core::Rect<int> Dirty_Region_Tracker::bounding_box() const {
    if (m_regions_.empty()) {
        return core::Rect<int>{};
    }

    int min_x = m_regions_[0].x;
    int min_y = m_regions_[0].y;
    int max_x = m_regions_[0].x + m_regions_[0].w;
    int max_y = m_regions_[0].y + m_regions_[0].h;

    for (const auto& r : m_regions_) {
        min_x = std::min(min_x, r.x);
        min_y = std::min(min_y, r.y);
        max_x = std::max(max_x, r.x + r.w);
        max_y = std::max(max_y, r.y + r.h);
    }

    return core::Rect<int>{min_x, min_y, max_x - min_x, max_y - min_y};
}

/*******************************************/
/*          Coalesce Regions               */
/*******************************************/
void Dirty_Region_Tracker::coalesce_regions() {
    if (m_regions_.size() < 2) {
        return;
    }

    bool merged = true;
    while (merged) {
        merged = false;
        std::vector<core::Rect<int>> new_regions;

        for (std::size_t i = 0; i < m_regions_.size(); ++i) {
            if (m_regions_[i].empty()) {
                continue;
            }

            core::Rect<int> current = m_regions_[i];

            for (std::size_t j = i + 1; j < m_regions_.size(); ++j) {
                if (m_regions_[j].empty()) {
                    continue;
                }

                if (should_merge(current, m_regions_[j])) {
                    current = merge(current, m_regions_[j]);
                    m_regions_[j] = core::Rect<int>{};  // Mark as merged
                    merged = true;
                }
            }

            new_regions.push_back(current);
        }

        m_regions_ = std::move(new_regions);
    }
}

/*******************************************/
/*        Check Should Merge               */
/*******************************************/
bool Dirty_Region_Tracker::should_merge(const core::Rect<int>& a,
                                        const core::Rect<int>& b) const {
    // Check for overlap or adjacency (within 4 pixels)
    const int margin = 4;
    return (a.x <= b.x + b.w + margin &&
            a.x + a.w + margin >= b.x &&
            a.y <= b.y + b.h + margin &&
            a.y + a.h + margin >= b.y);
}

/*******************************************/
/*            Merge Rects                  */
/*******************************************/
core::Rect<int> Dirty_Region_Tracker::merge(const core::Rect<int>& a,
                                            const core::Rect<int>& b) const {
    int min_x = std::min(a.x, b.x);
    int min_y = std::min(a.y, b.y);
    int max_x = std::max(a.x + a.w, b.x + b.w);
    int max_y = std::max(a.y + a.h, b.y + b.h);

    return core::Rect<int>{min_x, min_y, max_x - min_x, max_y - min_y};
}

} // namespace ovb::display

