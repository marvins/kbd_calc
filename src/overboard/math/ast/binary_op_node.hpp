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

    private:

        /// @brief Binary operation
        Binary_Op m_op;

        /// @brief Left operand
        Node::ptr_t  m_left;

        /// @brief Right operand
        Node::ptr_t  m_right;

}; // End of Binary_Op_Node class

} // namespace ovb::math::ast
