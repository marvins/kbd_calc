#pragma once

/**
 * @file rect.hpp
 * @brief 2D rectangle type with position and size
 *
 * Templated Rect class for integer pixel rectangles (Rect<int>)
 * or floating-point sub-pixel precision (Rect<float>).
 */

#include <overboard/core/point.hpp>

namespace ovb::core {

/**
 * @brief 2D rectangle with position and size
 *
 * Represents an axis-aligned rectangle defined by top-left corner (x, y)
 * and dimensions (width, height). Used for UI element positioning,
 * hit testing, and layout calculations.
 *
 * @tparam T Coordinate and dimension type (int for pixels, float for sub-pixel)
 *
 * @code
 *   // Create a 100×50 rectangle at position (10, 20)
 *   Recti r(10, 20, 100, 50);
 *
 *   // Check if point is inside
 *   if (r.contains(Point<int>(50, 30))) { ... }
 *
 *   // Get position and size
 *   auto pos = r.position();   // Point<int>{10, 20}
 *   auto sz = r.size();        // Size<int>{100, 50}
 * @endcode
 */
template<typename T>
struct Rect {
    T x = 0;  ///< X coordinate of top-left corner
    T y = 0;  ///< Y coordinate of top-left corner
    T w = 0;  ///< Width (must be non-negative)
    T h = 0;  ///< Height (must be non-negative)

    /// @brief Default constructor - creates null rectangle at origin with zero size
    Rect() = default;

    /**
     * @brief Construct from individual coordinates and dimensions
     * @param x_val X coordinate of top-left corner
     * @param y_val Y coordinate of top-left corner
     * @param w_val Width (should be >= 0)
     * @param h_val Height (should be >= 0)
     */
    Rect(T x_val, T y_val, T w_val, T h_val)
        : x(x_val), y(y_val), w(w_val), h(h_val) {}

    /**
     * @brief Get position as Point
     * @return Top-left corner position {x, y}
     */
    Point<T> position() const { return {x, y}; }

    /**
     * @brief Get size as Size (Point alias)
     * @return Dimensions {width, height}
     */
    Size<T> size() const { return {w, h}; }

    /**
     * @brief Get right edge X coordinate
     * @return x + width (coordinate of right edge)
     */
    T right() const { return x + w; }

    /**
     * @brief Get bottom edge Y coordinate
     * @return y + height (coordinate of bottom edge)
     */
    T bottom() const { return y + h; }

    /**
     * @brief Check if rectangle contains a point
     * @param p Point to test
     * @return true if point is inside or on the edge
     *
     * Uses half-open intervals: [x, x+w) and [y, y+h)
     */
    bool contains(const Point<T>& p) const {
        return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
    }

    /**
     * @brief Check if rectangle fully contains another rectangle
     * @param other Rectangle to test
     * @return true if other is entirely within this rectangle
     */
    bool contains(const Rect& other) const {
        return other.x >= x && other.right() <= right() &&
               other.y >= y && other.bottom() <= bottom();
    }

    /**
     * @brief Check if rectangles intersect
     * @param other Rectangle to test against
     * @return true if rectangles have any overlapping area
     *
     * Touching edges (e.g., right edge of this equals left edge of other)
     * does not count as intersection.
     */
    bool intersects(const Rect& other) const {
        return x < other.right() && right() > other.x &&
               y < other.bottom() && bottom() > other.y;
    }

    /**
     * @brief Check if rectangle has zero area
     * @return true if width or height is zero
     */
    bool empty() const {
        return w == 0 || h == 0;
    }
};

/**
 * @brief Integer rectangle for pixel coordinates
 *
 * Primary type for UI layout and display calculations.
 * Use for screen positions, widget bounds, and hit testing.
 */
using Recti = Rect<int>;

/**
 * @brief Float rectangle for sub-pixel precision
 *
 * Use when fractional positions are needed, e.g., for
 * animations, scaling, or layout calculations before rounding.
 */
using Rectf = Rect<float>;

/**
 * @brief Double rectangle for high-precision calculations
 *
 * Use when maximum precision is required, e.g., for
 * complex geometric calculations or coordinate transforms.
 */
using Rectd = Rect<double>;

} // namespace ovb::core
