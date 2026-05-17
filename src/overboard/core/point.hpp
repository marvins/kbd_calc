#pragma once

/**
 * @file point.hpp
 * @brief 2D point/pixel coordinate type
 *
 * Templated Point class for integer pixel coordinates (Point<int>)
 * or floating-point sub-pixel precision (Point<float>).
 */

namespace ovb::core {

/**
 * @brief 2D coordinate point
 *
 * @tparam T Coordinate type (int for pixels, float for sub-pixel)
 */
template <typename T>
struct Point {
    T x = 0;  ///< X coordinate
    T y = 0;  ///< Y coordinate

    /**
     * @brief Default constructor - initializes to (0, 0)
     */
    Point() = default;

    /**
     * @brief Construct from individual coordinates
     * @param x_val X coordinate value
     * @param y_val Y coordinate value
     */
    Point(T x_val, T y_val) : x(x_val), y(y_val) {}

    /**
     * @brief Add two points component-wise
     * @param other Point to add
     * @return Sum as new Point
     */
    Point operator+(const Point& other) const {
        return Point{x + other.x, y + other.y};
    }

    /**
     * @brief Subtract two points component-wise
     * @param other Point to subtract
     * @return Difference as new Point
     */
    Point operator-(const Point& other) const {
        return Point{x - other.x, y - other.y};
    }

    /**
     * @brief Scale by scalar multiplication
     * @param scalar Value to multiply by
     * @return Scaled Point
     */
    Point operator*(T scalar) const {
        return Point{x * scalar, y * scalar};
    }

    /**
     * @brief Scale by scalar division
     * @param scalar Value to divide by
     * @return Scaled Point
     */
    Point operator/(T scalar) const {
        return Point{x / scalar, y / scalar};
    }

    /**
     * @brief Add-assign another point
     * @param other Point to add
     * @return Reference to this
     */
    Point& operator+=(const Point& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    /**
     * @brief Subtract-assign another point
     * @param other Point to subtract
     * @return Reference to this
     */
    Point& operator-=(const Point& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * @brief Equality comparison
     * @param other Point to compare with
     * @return True if both coordinates match
     */
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    /**
     * @brief Inequality comparison
     * @param other Point to compare with
     * @return True if any coordinate differs
     */
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

/**
 * @brief 2D point with double precision
 */
using Point2d = Point<double>;

/**
 * @brief 2D point with float precision
 */
using Point2f = Point<float>;

/**
 * @brief 2D point with integer coordinates
 */
using Point2i = Point<int>;

/**
 * @brief 2D size with width and height
 *
 * Semantically distinct from Point - represents dimensions rather than position.
 * Uses same underlying type for arithmetic compatibility.
 */
template <typename T>
using Size = Point<T>;

/// Integer size (width, height)
using Size2i = Size<int>;

/// Float size (width, height)
using Size2f = Size<float>;

/// Double size (width, height)
using Size2d = Size<double>;

} // namespace ovb::core
