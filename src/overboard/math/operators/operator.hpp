/**
 * @file    operator.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Base operator interface and traits
 */
#pragma once

// C++ Standard Libraries
#include <string>
#include <string_view>
#include <memory>

// Project Libraries
#include <overboard/math/ast/binary_op_node.hpp>
#include <overboard/math/ast/unary_op_node.hpp>
#include <overboard/math/ast/function_node.hpp>
#include <overboard/math/ast/node.hpp>

namespace ovb::math::operators {

/**
 * @brief Operator metadata traits
 *
 * Template specialization for each operator type.
 */
template<typename Op>
struct Operator_Traits;

/**
 * @brief Base operator interface
 */
class I_Operator {

    public:

        /**
         * @brief Virtual destructor
         */
        virtual ~I_Operator() = default;

        /**
         * @brief Get operator symbol
         */
        virtual std::string_view symbol() const = 0;

        /**
         * @brief Get operator precedence
         */
        virtual int precedence() const = 0;

        /**
         * @brief Check if right-associative
         */
        virtual bool right_associative() const = 0;

        /**
         * @brief Create AST node from operands
         */
        virtual ast::Node::ptr_t create_node(
            std::vector<ast::Node::ptr_t> operands) const = 0;

        /**
         * @brief Number of operands required
         */
        virtual int operand_count() const = 0;
};

/**
 * @brief Binary operator base class
 */
class Binary_Operator : public I_Operator {

    public:

        /**
         * @brief Constructor
         */
        Binary_Operator(ast::Binary_Op op) : m_op(op) {}

        /**
         * @brief Return the number of operands required
         */
        int operand_count() const override { return 2; }

        /**
         * @brief Create AST node from operands
         */
        ast::Node::ptr_t create_node(
            std::vector<ast::Node::ptr_t> operands) const override {
            if (operands.size() != 2) {
                return nullptr;
            }
            return std::make_unique<ast::Binary_Op_Node>(
                m_op,
                std::move(operands[0]),
                std::move(operands[1])
            );
        }

    protected:

        /// @brief Binary operator type
        ast::Binary_Op m_op;

}; // End of Binary_Operator class

/**
 * @brief Unary operator base class
 */
class Unary_Operator : public I_Operator {

    public:

        /**
         * @brief Constructor
         */
        Unary_Operator(ast::Unary_Op op) : m_op(op) {}

        /**
         * @brief Return the number of operands required
         */
        int operand_count() const override { return 1; }

        /**
         * @brief Create AST node from operands
         */
        ast::Node::ptr_t create_node(
            std::vector<ast::Node::ptr_t> operands) const override {
            if (operands.size() != 1) {
                return nullptr;
            }
            return std::make_unique<ast::Unary_Op_Node>(
                m_op,
                std::move(operands[0])
            );
        }

    protected:

        /// @brief Unary operator type
        ast::Unary_Op m_op;

}; // End of Unary_Operator class

} // namespace ovb::math::operators
