/**
 * @file    factorial_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for factorial operation (n!)
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <stdexcept>
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Factorial node
 *
 * Represents a factorial operation (n!).
 */
class Factorial_Node : public Node {

    public:

        /**
         * @brief Construct a new Factorial_Node object
         *
         * @param o Operand
         */
        explicit Factorial_Node(Node::ptr_t o)
            : Node(Node_Kind::FACTORIAL), m_operand(std::move(o)) {}

        /**
         * @brief Get the operand
         *
         * @return const Node::ptr_t& Operand
         */
        const Node::ptr_t& operand() const { return m_operand; }

        /**
         * @brief Evaluate the factorial
         *
         * @return double Result of the factorial
         */
        double eval() const override {
            int n = static_cast<int>(m_operand->eval());
            if (n < 0) throw std::runtime_error("Factorial of negative");
            double f = 1.0;
            for (int i = 2; i <= n; ++i) f *= i;
            return f;
        }

        /**
         * @brief Convert the factorial to LaTeX
         *
         * @return std::string LaTeX representation of the factorial
         */
        std::string to_latex() const override { return m_operand->to_latex() + "!"; }

        /**
         * @brief Convert the factorial to string
         *
         * @return std::string String representation of the factorial
         */
        std::string to_string() const override { return m_operand->to_string() + "!"; }

        /**
         * @brief Clone the factorial node
         *
         * @return Node::ptr_t Cloned factorial node
         */
        Node::ptr_t clone() const override {
            return std::make_unique<Factorial_Node>(m_operand->clone());
        }

        /**
         * @brief Simplify the factorial node
         *
         * @return Node::ptr_t Simplified factorial node
         */
        Node::ptr_t simplify() const override {
            return std::make_unique<Factorial_Node>(m_operand->simplify());
        }

        /**
         * @brief Get the number of children (always 1 for factorial)
         *
         * @return size_t Number of children
         */
        size_t child_count() const override { return 1; }

        /**
         * @brief Get child node at given index
         *
         * @param index Child index (0 for operand)
         * @return Node* Child node (nullptr if index out of bounds)
         */
        Node* child_at( size_t index ) override {
            if ( index == 0 ) return m_operand.get();
            return nullptr;
        }

        /**
         * @brief Get child node at given index (const version)
         *
         * @param index Child index (0 for operand)
         * @return const Node* Child node (nullptr if index out of bounds)
         */
        const Node* child_at( size_t index ) const override {
            if ( index == 0 ) return m_operand.get();
            return nullptr;
        }

        /**
         * @brief Release the operand (for tree restructuring)
         *
         * @return Node::ptr_t Released operand
         */
        Node::ptr_t release_operand() { return std::move(m_operand); }

        /**
         * @brief Set the operand (for tree restructuring)
         *
         * @param node New operand
         */
        void set_operand( Node::ptr_t node ) { m_operand = std::move(node); }

    private:

        /// @brief Operand
        Node::ptr_t m_operand;

}; // End of Factorial_Node class

} // namespace ovb::math::ast
