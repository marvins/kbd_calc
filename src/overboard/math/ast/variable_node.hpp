/**
 * @file    variable_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for variables (x, y, n, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <stdexcept>
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Variable node
 *
 * Represents a variable name in the AST.
 * Cannot be evaluated without a value assignment.
 */
class Variable_Node : public Node {

    public:

        /**
         * @brief Construct a new Variable_Node object
         *
         * @param n Name of the variable
         */
        explicit Variable_Node(std::string n)
            : Node(Node_Kind::VARIABLE), m_name(std::move(n)) {}

        /**
         * @brief Get the name of the variable
         *
         * @return const std::string& Name of the variable
         */
        const std::string& name() const { return m_name; }

        /**
         * @brief Evaluate the variable
         *
         * @return double Value of the variable
         */
        double eval() const override {
            throw std::runtime_error("Cannot evaluate variable '" + m_name + "' without a value");
        }

        /**
         * @brief Convert the variable to LaTeX
         *
         * @return std::string LaTeX representation of the variable
         */
        std::string to_latex() const override { return m_name; }

        /**
         * @brief Convert the variable to string
         *
         * @return std::string String representation of the variable
         */
        std::string to_string() const override { return m_name; }

        /**
         * @brief Clone the variable node
         *
         * @return Node::ptr_t Cloned variable node
         */
        Node::ptr_t clone() const override { return std::make_unique<Variable_Node>(m_name); }

        /**
         * @brief Simplify the variable node
         *
         * @return Node::ptr_t Simplified variable node
         */
        Node::ptr_t simplify() const override { return clone(); }

    private:

        /// @brief Name of the variable
        std::string m_name;

}; // End of Variable_Node class

} // namespace ovb::math::ast
