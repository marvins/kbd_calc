#pragma once

/**
 * @file box.hpp
 * @brief Math expression layout box types
 *
 * Defines the box model for typesetting mathematical expressions.
 * Each expression node is converted to a Layout_Box tree, which is then
 * measured (bottom-up sizing) and positioned (top-down placement).
 */

// C++ Standard Libraries
#include <string>
#include <vector>

// Project Libraries
#include <overboard/core/point.hpp>

namespace ovb::layout {


/**
 * @brief Types of layout boxes
 *
 * The box model supports atoms (basic text), fractions, powers, sequences,
 * and superscripts. This is sufficient for standard calculator expressions.
 */
enum class Box_Kind {
    ATOM,       ///< Basic text element (number, symbol, operator)
    FRACTION,   ///< Fraction with numerator and denominator
    POWER,      ///< Base with superscript exponent
    SEQUENCE,   ///< Horizontal list of boxes
    SUPERSCRIPT,///< Standalone superscript (for future use)
    SQRT        ///< Square root with horizontal bar over argument
};

/**
 * @brief Layout box for math expression typesetting
 *
 * A Layout_Box represents a node in the layout tree. After construction,
 * the tree undergoes two passes:
 * 1. measure(): bottom-up sizing to compute w, h, baseline
 * 2. layout(): top-down positioning to set x, y
 *
 * @see Layout_Engine for the measurement and layout operations
 */
struct Layout_Box {
    Box_Kind kind;               ///< Box type
    core::Point<int> pos;             ///< Position (set by layout pass)
    core::Size<int> size;             ///< Size (set by measure pass)
    int baseline = 0;           ///< Distance from y to text baseline
    float scale = 2.0f;         ///< Font scale (1.0, 2.0, 3.0...)
    std::string text;           ///< Text content (for ATOM boxes)
    std::vector<Layout_Box> children;  ///< Child boxes (for composite types)


    /**
     * @brief Create an atom box containing text
     *
     * @param t Text content
     * @param scale Font scale (default 2)
     * @return Layout_Box Atom box with pre-computed size
     */
    static Layout_Box atom( std::string t, float scale = 2.0f );

    /**
     * @brief Create a fraction box
     *
     * The numerator is placed above a horizontal line,
     * the denominator below.
     *
     * @param num Numerator box
     * @param den Denominator box
     * @param scale Font scale (default 2)
     * @return Layout_Box Fraction box
     */
    static Layout_Box fraction( Layout_Box num, Layout_Box den, float scale = 2.0f );

    /**
     * @brief Create a power/exponent box
     *
     * The exponent is rendered at a smaller scale (scale-1)
     * and raised above the baseline.
     *
     * @param base Base expression
     * @param exp Exponent expression
     * @param scale Font scale (default 2)
     * @return Layout_Box Power box
     */
    static Layout_Box power( Layout_Box base, Layout_Box exp, float scale = 2.0f );

    /**
     * @brief Create a horizontal sequence of boxes
     *
     * Children are arranged left-to-right with baseline alignment.
     *
     * @param boxes Child boxes
     * @param scale Font scale (default 2)
     * @return Layout_Box Sequence box
     */
    static Layout_Box sequence( std::vector<Layout_Box> boxes, float scale = 2.0f );

    /**
     * @brief Create a square root box
     *
     * The argument is placed under a horizontal bar with a √ symbol.
     *
     * @param arg Argument box
     * @param scale Font scale (default 2)
     * @return Layout_Box Square root box
     */
    static Layout_Box sqrt( Layout_Box arg, float scale = 2.0f );

    /**
     * @brief Position accessor
     * @return Position as Point
     */
    core::Point<int> position() const { return pos; }

    /**
     * @brief Size accessor
     * @return Size as Size (Point<int>)
     */
    core::Size<int> dimensions() const { return size; }

    /**
     * @brief Width accessor
     * @return Width (size.x)
     */
    int width() const { return size.x; }

    /**
     * @brief Height accessor
     * @return Height (size.y)
     */
    int height() const { return size.y; }

    /**
     * @brief Top-left corner position
     * @return Point at (x, y)
     */
    core::Point<int> tl() const { return pos; }

    /**
     * @brief Top-right corner position
     * @return Point at (x + w, y)
     */
    core::Point<int> tr() const { return core::Point<int>(pos.x + size.x, pos.y); }

    /**
     * @brief Bottom-left corner position
     * @return Point at (x, y + h)
     */
    core::Point<int> bl() const { return core::Point<int>(pos.x, pos.y + size.y); }

    /**
     * @brief Bottom-right corner position
     * @return Point at (x + w, y + h)
     */
    core::Point<int> br() const { return core::Point<int>(pos.x + size.x, pos.y + size.y); }
};

} // namespace ovb::layout
