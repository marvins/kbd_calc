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

        /**
         * @brief Get the number of children (number of function arguments)
         *
         * @return size_t Number of children
         */
        size_t child_count() const override { return m_args.size(); }

        /**
         * @brief Get child node at given index
         *
         * @param index Child index (argument index)
         * @return Node* Child node (nullptr if index out of bounds)
         */
        Node* child_at( size_t index ) override {
            if ( index < m_args.size() ) return m_args[index].get();
            return nullptr;
        }

        /**
         * @brief Get child node at given index (const version)
         *
         * @param index Child index (argument index)
         * @return const Node* Child node (nullptr if index out of bounds)
         */
        const Node* child_at( size_t index ) const override {
            if ( index < m_args.size() ) return m_args[index].get();
            return nullptr;
        }

        /**
         * @brief Release a child node (for tree restructuring)
         *
         * @param index Child index to release
         * @return Node::ptr_t Released child node
         */
        Node::ptr_t release_child( size_t index ) {
            if ( index < m_args.size() ) {
                Node::ptr_t node = std::move(m_args[index]);
                m_args.erase(m_args.begin() + static_cast<std::ptrdiff_t>(index));
                return node;
            }
            return nullptr;
        }

        /**
         * @brief Set a child node at a specific index
         *
         * @param index Child index to set
         * @param node New child node
         */
        void set_child( size_t index, Node::ptr_t node ) {
            if ( index < m_args.size() ) {
                m_args[index] = std::move(node);
            }
        }

    private:

        /// @brief Function name
        std::string           m_name;

        /// @brief Function arguments
        std::vector<Node::ptr_t> m_args;
};

} // namespace ovb::math::ast
