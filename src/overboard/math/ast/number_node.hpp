/**
 * @file    number_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for numeric literals
 */
#pragma once

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Number literal node
 *
 * Represents a literal numeric value in the AST.
 */
class Number_Node : public Node {

    public:

        /**
         * @brief Construct a new Number_Node object
         *
         * @param v Value of the number
         */
        explicit Number_Node(double v)
            : Node(Node_Kind::NUMBER), m_value(v) {}

        /**
         * @brief Get the value of the number
         *
         * @return double Value of the number
         */
        double value() const { return m_value; }

        /**
         * @brief Evaluate the number
         *
         * @return double Value of the number
         */
        double eval() const override { return m_value; }

        /**
         * @brief Convert the number to LaTeX
         *
         * @return std::string LaTeX representation of the number
         */
        std::string to_latex() const override;

        /**
         * @brief Convert the number to string
         *
         * @return std::string String representation of the number
         */
        std::string to_string() const override;

        /**
         * @brief Clone the number node
         *
         * @return Node::ptr_t Cloned number node
         */
        Node::ptr_t clone() const override { return std::make_unique<Number_Node>(m_value); }

        /**
         * @brief Simplify the number node
         *
         * @return Node::ptr_t Simplified number node
         */
        Node::ptr_t simplify() const override { return clone(); }

    private:

        /// @brief Value of the number
        double m_value;
};

} // namespace ovb::math::ast
