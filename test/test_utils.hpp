/**
 * @file test_utils.hpp
 * @brief Shared test utilities for calculator tests
 */
#pragma once

// C++ Standard Libraries
#include <iostream>
#include <string>

// Project Libraries
#include <overboard/math/layout/box.hpp>

namespace ovb::test {

// Print box structure for debugging
inline void print_box(const ovb::layout::Layout_Box& box, int indent = 0) {
    using ovb::layout::Box_Kind;

    std::string prefix(static_cast<std::size_t>(indent) * 2, ' ');
    switch (box.kind) {
        case Box_Kind::ATOM:
            std::cout << prefix << "ATOM: \"" << box.text << "\" scale=" << box.scale
                      << " w=" << box.size.x << " h=" << box.size.y << "\n";
            break;
        case Box_Kind::FRACTION:
            std::cout << prefix << "FRACTION w=" << box.size.x << " h=" << box.size.y << "\n";
            print_box(box.children[0], indent + 1);
            print_box(box.children[1], indent + 1);
            break;
        case Box_Kind::POWER:
            std::cout << prefix << "POWER w=" << box.size.x << " h=" << box.size.y << "\n";
            print_box(box.children[0], indent + 1);
            print_box(box.children[1], indent + 1);
            break;
        case Box_Kind::SEQUENCE:
            std::cout << prefix << "SEQUENCE w=" << box.size.x << " h=" << box.size.y
                      << " children=" << box.children.size() << "\n";
            for (const auto& child : box.children) {
                print_box(child, indent + 1);
            }
            break;
        case Box_Kind::SUPERSCRIPT:
            std::cout << prefix << "SUPERSCRIPT\n";
            break;
    }
}

// Dump box tree to string for test assertions
inline std::string box_to_string(const ovb::layout::Layout_Box& box) {
    using ovb::layout::Box_Kind;

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
    }
    return "UNKNOWN";
}

// Find first child of given kind (depth-first search)
inline const ovb::layout::Layout_Box* find_first(const ovb::layout::Layout_Box& root,
                                                ovb::layout::Box_Kind kind) {
    using ovb::layout::Box_Kind;

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
inline bool contains_kind(const ovb::layout::Layout_Box& root, ovb::layout::Box_Kind kind) {
    return find_first(root, kind) != nullptr;
}

} // namespace ovb::test
