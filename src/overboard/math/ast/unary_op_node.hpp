/**
 * @file    unary_op_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for unary operations (-, ~, %)
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Unary operation node
 *
 * Represents operations with one operand like negation,
 * bitwise not, and percentage.
 */
class Unary_Op_Node : public Node {

    public:

        /**
         * @brief Construct a new Unary_Op_Node object
         *
         * @param o The unary operation to perform
         * @param operand_ The operand to perform the operation on
         */
        Unary_Op_Node( Unary_Op o, Node::ptr_t operand_ )
            : Node(Node_Kind::UNARY_OP), m_op(o),
              m_operand(std::move(operand_)) {}

        /**
         * @brief Get the operation
         *
         * @return Unary_Op The operation
         */
        Unary_Op op() const { return m_op; }

        /**
         * @brief Get the operand
         *
         * @return const Node::ptr_t& The operand
         */
        const Node::ptr_t& operand() const { return m_operand; }

        /**
         * @brief Evaluate the unary operation
         *
         * @return double The result of the operation
         */
        double eval() const override {
            double v = m_operand->eval();
            switch (m_op) {
                case Unary_Op::NEGATE:  return -v;
                case Unary_Op::BIT_NOT: return static_cast<double>(~static_cast<long long>(v));
                case Unary_Op::PERCENT: return v / 100.0;
            }
            return 0.0;
        }

        /**
         * @brief Convert the unary operation to LaTeX
         *
         * @return std::string The LaTeX representation
         */
        std::string to_latex()  const override;

        /**
         * @brief Convert the unary operation to a string
         *
         * @return std::string The string representation
         */
        std::string to_string() const override;

        /**
         * @brief Clone the unary operation
         *
         * @return Node::ptr_t The cloned node
         */
        Node::ptr_t clone() const override {
            return std::make_unique<Unary_Op_Node>(m_op, m_operand->clone());
        }

        /**
         * @brief Simplify the unary operation
         *
         * @return Node::ptr_t The simplified node
         */
        Node::ptr_t simplify() const override {
            return std::make_unique<Unary_Op_Node>(m_op, m_operand->simplify());
        }

        /**
         * @brief Get the number of children (always 1 for unary ops)
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

        /// @brief The unary operation to perform
        Unary_Op m_op;

        /// @brief The operand to perform the operation on
        Node::ptr_t m_operand;
};

} // namespace ovb::math::ast
