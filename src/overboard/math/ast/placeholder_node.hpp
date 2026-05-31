/**
 * @file    placeholder_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for expression placeholders
 *
 * Represents an empty slot in an incomplete expression,
 * used for typesetting fractions and other structures
 * before all operands are entered.
 */
#pragma once

// C++ Standard Libraries
#include <stdexcept>
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Placeholder node
 *
 * Represents an empty box for incomplete expressions.
 * Used for rendering partial expressions like "5/□".
 */
class Placeholder_Node : public Node {

    public:

        /**
         * @brief Construct a new Placeholder_Node object
         */
        explicit Placeholder_Node()
            : Node(Node_Kind::PLACEHOLDER) {}

        /**
         * @brief Evaluate the placeholder node
         *
         * @return double Throws an exception since placeholders cannot be evaluated
         */
        double eval() const override {
            throw std::runtime_error("Cannot evaluate placeholder");
        }

        /**
         * @brief Convert the placeholder to LaTeX format
         *
         * @return std::string LaTeX representation
         */
        std::string to_latex() const override { return "\\square"; }

        /**
         * @brief Convert the placeholder to string format
         *
         * @return std::string String representation
         */
        std::string to_string() const override { return ""; }

        /**
         * @brief Clone the placeholder node
         *
         * @return Node::ptr_t Cloned node
         */
        Node::ptr_t clone() const override { return std::make_unique<Placeholder_Node>(); }

        /**
         * @brief Simplify the placeholder node
         *
         * @return Node::ptr_t Simplified node
         */
        Node::ptr_t simplify() const override { return clone(); }

        /**
         * @brief Get the number of children (0 for leaf nodes)
         *
         * @return size_t Number of children
         */
        size_t child_count() const override { return 0; }

        /**
         * @brief Get child node at given index (always nullptr for leaf nodes)
         *
         * @param index Child index (ignored)
         * @return Node* Always nullptr
         */
        Node* child_at( [[maybe_unused]] size_t index ) override {
            return nullptr;
        }

        /**
         * @brief Get child node at given index (const version, always nullptr)
         *
         * @param index Child index (ignored)
         * @return const Node* Always nullptr
         */
        const Node* child_at( [[maybe_unused]] size_t index ) const override {
            return nullptr;
        }
};

} // namespace ovb::math::ast
