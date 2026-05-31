/**
 * @file    unary_operators.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Concrete unary operator implementations
 */
#pragma once

#include <overboard/math/ast/factorial_node.hpp>
#include <overboard/math/ast/node.hpp>
#include <overboard/math/operators/operator.hpp>

namespace ovb::math::operators {

/**
 * @brief Negation operator
 */
class Negate : public Unary_Operator {
    public:

        /**
         * @brief Constructor
         */
        Negate() : Unary_Operator(ast::Unary_Op::NEGATE) {}

        /**
         * @brief Return the negation symbol
         */
        std::string_view symbol() const override { return "-"; }

        /**
         * @brief Return the precedence of the negation operator
         */
        int precedence() const override { return 4; }

        /**
         * @brief Return whether the negation operator is right associative
         */
        bool right_associative() const override { return false; }
};

/**
 * @brief Factorial operator
 */
class Factorial : public I_Operator {
    public:

        /**
         * @brief Create a factorial node from arguments
         */
        ast::Node::ptr_t create_node(std::vector<ast::Node::ptr_t> args) const override {
            if (args.empty()) {
                return nullptr;
            }
            return std::make_unique<ast::Factorial_Node>(std::move(args[0]));
        }

        /**
         * @brief Return the factorial symbol
         */
        std::string_view symbol() const override { return "!"; }

        /**
         * @brief Return the precedence of the factorial operator
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the factorial operator is right associative
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Return the number of operands required
         */
        int operand_count() const override { return 1; }
};

} // namespace ovb::math::operators
