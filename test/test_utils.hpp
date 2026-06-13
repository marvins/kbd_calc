/**
 * @file test_utils.hpp
 * @brief Shared test utilities for calculator tests
 */
#pragma once

// C++ Standard Libraries
#include <format>
#include <iostream>
#include <string>

// Project Libraries
#include <overboard/math/layout/box.hpp>
#include <overboard/log/i_logger.hpp>

namespace ovb::test {

/**
 * @brief Print box structure for debugging
 *
 * @param box Box to print
 * @param logger Logger to use for output
 * @param indent Indentation level for nested boxes
 */
inline void print_box( const math::layout::Layout_Box& box,
                       log::I_Logger&            logger,
                       int                       indent = 0 ) {
    using ovb::math::layout::Box_Kind;

    std::string prefix(static_cast<std::size_t>(indent) * 2, ' ');
    switch (box.kind) {
        case Box_Kind::ATOM:
            logger.trace(std::format("{}ATOM: \"{}\" scale={} w={} h={}", prefix, box.text, box.scale, box.size.x, box.size.y));
            break;
        case Box_Kind::FRACTION:
            logger.trace(std::format("{}FRACTION w={} h={}", prefix, box.size.x, box.size.y));
            print_box(box.children[0], logger, indent + 1);
            print_box(box.children[1], logger, indent + 1);
            break;
        case Box_Kind::POWER:
            logger.trace(std::format("{}POWER w={} h={}", prefix, box.size.x, box.size.y));
            print_box(box.children[0], logger, indent + 1);
            print_box(box.children[1], logger, indent + 1);
            break;
        case Box_Kind::SEQUENCE:
            logger.trace(std::format("{}SEQUENCE w={} h={} children={}", prefix, box.size.x, box.size.y, box.children.size()));
            for (const auto& child : box.children) {
                print_box(child, logger, indent + 1);
            }
            break;
        case Box_Kind::SUPERSCRIPT:
            logger.trace(std::format("{}SUPERSCRIPT", prefix));
            break;
        case Box_Kind::SQRT:
            logger.trace(std::format("{}SQRT w={} h={}", prefix, box.size.x, box.size.y));
            print_box(box.children[0], logger, indent + 1);
            break;
    }
}

/**
 * @brief Dump box tree to string for test assertions
 *
 * @param box Box to convert to string
 * @return std::string String representation of the box tree
 */
inline std::string box_to_string(const ovb::math::layout::Layout_Box& box) {
    using ovb::math::layout::Box_Kind;

    switch (box.kind) {
        case Box_Kind::ATOM:
            return "ATOM(\"" + box.text + "\")";
        case Box_Kind::FRACTION:
            return "FRACTION[" + box_to_string(box.children[0]) + "/"
                              + box_to_string(box.children[1]) + "]";
        case Box_Kind::POWER:
            return "POWER[" + box_to_string(box.children[0]) + "^"
                            + box_to_string(box.children[1]) + "]";
        case Box_Kind::SEQUENCE:
            return "SEQUENCE{...}";
        case Box_Kind::SUPERSCRIPT:
            return "SUPERSCRIPT";
        case Box_Kind::SQRT:
            return "SQRT[" + box_to_string(box.children[0]) + "]";
    }
    return "UNKNOWN";
}

/**
 * @brief Find first child of given kind (depth-first search)
 *
 * @param root Root box to search from
 * @param kind Kind to search for
 * @return const ovb::math::layout::Layout_Box* Pointer to first matching box, or nullptr if not found
 */
inline const ovb::math::layout::Layout_Box* find_first(const ovb::math::layout::Layout_Box& root,
                                                ovb::math::layout::Box_Kind kind) {
    using ovb::math::layout::Box_Kind;

    if (root.kind == kind) {
        return &root;
    }
    for (const auto& child : root.children) {
        auto* found = find_first(child, kind);
        if (found) return found;
    }
    return nullptr;
}

// Check if box tree contains a specific box kind
inline bool contains_kind(const ovb::math::layout::Layout_Box& root, ovb::math::layout::Box_Kind kind) {
    return find_first(root, kind) != nullptr;
}

} // namespace ovb::test
