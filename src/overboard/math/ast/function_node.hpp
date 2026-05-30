/**
 * @file    function_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for function calls (sin, cos, sqrt, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Function call node
 *
 * Represents a function call with one or more arguments.
 */
class Function_Node : public Node {

    public:

        /**
         * @brief Construct a new Function_Node object
         *
         * @param n Function name
         * @param a Function arguments
         */
        Function_Node( std::string n,
                       std::vector<Node::ptr_t> a )
            : Node(Node_Kind::FUNCTION), m_name(std::move(n)),
              m_args(std::move(a)) {}

        /**
         * @brief Get the function name
         *
         * @return const std::string& Function name
         */
        const std::string&           name() const { return m_name; }

        /**
         * @brief Get the function arguments
         *
         * @return const std::vector<Node::ptr_t>& Function arguments
         */
        const std::vector<Node::ptr_t>& args() const { return m_args; }

        /**
         * @brief Evaluate the function
         *
         * @return double Evaluated result
         */
        double eval() const override;

        /**
         * @brief Convert to LaTeX representation
         *
         * @return std::string LaTeX representation
         */
        std::string to_latex()  const override;

        /**
         * @brief Convert to string representation
         *
         * @return std::string String representation
         */
        std::string to_string() const override;

        /**
         * @brief Clone the node
         *
         * @return Node::ptr_t Cloned node
         */
        Node::ptr_t clone() const override;

        /**
         * @brief Simplify the node
         *
         * @return Node::ptr_t Simplified node
         */
        Node::ptr_t simplify() const override;

    private:

        /// @brief Function name
        std::string           m_name;

        /// @brief Function arguments
        std::vector<Node::ptr_t> m_args;
};

} // namespace ovb::math::ast
