/**
 * @file    binary_op_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for binary operations (+, -, *, /, ^, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Binary operation node
 *
 * Represents operations with two operands like addition,
 * subtraction, multiplication, division, power, etc.
 */
class Binary_Op_Node : public Node {

    public:

        /**
         * @brief Construct a new Binary_Op_Node object
         *
         * @param o Binary operation
         * @param l Left operand
         * @param r Right operand
         */
        Binary_Op_Node(Binary_Op o, Node::ptr_t l, Node::ptr_t r)
            : Node(Node_Kind::BINARY_OP), m_op(o),
              m_left(std::move(l)), m_right(std::move(r)) {}

        /**
         * @brief Get the binary operation
         *
         * @return Binary_Op Binary operation
         */
        Binary_Op op() const { return m_op; }

        /**
         * @brief Get the left operand
         *
         * @return const Node::ptr_t& Left operand
         */
        const Node::ptr_t& left() const { return m_left; }

        /**
         * @brief Get the right operand
         *
         * @return const Node::ptr_t& Right operand
         */
        const Node::ptr_t& right() const { return m_right; }

        /**
         * @brief Evaluate the binary operation
         *
         * @return double Result of the binary operation
         */
        double eval() const override;

        /**
         * @brief Convert the binary operation to LaTeX
         *
         * @return std::string LaTeX representation of the binary operation
         */
        std::string to_latex() const override;

        /**
         * @brief Convert the binary operation to string
         *
         * @return std::string String representation of the binary operation
         */
        std::string to_string() const override;

        /**
         * @brief Clone the binary operation node
         *
         * @return Node::ptr_t Cloned binary operation node
         */
        Node::ptr_t clone() const override;

        /**
         * @brief Simplify the binary operation node
         *
         * @return Node::ptr_t Simplified binary operation node
         */
        Node::ptr_t simplify() const override;

        /**
         * @brief Get the number of children (always 2 for binary ops)
         *
         * @return size_t Number of children
         */
        size_t child_count() const override { return 2; }

        /**
         * @brief Get child node at given index
         *
         * @param index Child index (0 for left, 1 for right)
         * @return Node* Child node (nullptr if index out of bounds)
         */
        Node* child_at( size_t index ) override {
            if ( index == 0 ) return m_left.get();
            if ( index == 1 ) return m_right.get();
            return nullptr;
        }

        /**
         * @brief Get child node at given index (const version)
         *
         * @param index Child index (0 for left, 1 for right)
         * @return const Node* Child node (nullptr if index out of bounds)
         */
        const Node* child_at( size_t index ) const override {
            if ( index == 0 ) return m_left.get();
            if ( index == 1 ) return m_right.get();
            return nullptr;
        }

        /**
         * @brief Release the left operand (for tree restructuring)
         *
         * @return Node::ptr_t Released left operand
         */
        Node::ptr_t release_left() { return std::move(m_left); }

        /**
         * @brief Release the right operand (for tree restructuring)
         *
         * @return Node::ptr_t Released right operand
         */
        Node::ptr_t release_right() { return std::move(m_right); }

        /**
         * @brief Set the left operand (for tree restructuring)
         *
         * @param node New left operand
         */
        void set_left( Node::ptr_t node ) { m_left = std::move(node); }

        /**
         * @brief Set the right operand (for tree restructuring)
         *
         * @param node New right operand
         */
        void set_right( Node::ptr_t node ) { m_right = std::move(node); }

    private:

        /// @brief Binary operation
        Binary_Op m_op;

        /// @brief Left operand
        Node::ptr_t  m_left;

        /// @brief Right operand
        Node::ptr_t  m_right;

}; // End of Binary_Op_Node class

} // namespace ovb::math::ast
