/**
 * @file    binary_operators.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Concrete binary operator implementations
 */
#pragma once

#include <overboard/math/operators/operator.hpp>

namespace ovb::math::operators {

/**
 * @brief Addition operator
 */
class Add : public Binary_Operator {

    public:

        /**
         * @brief Constructor
         */
        Add() : Binary_Operator(ast::Binary_Op::ADD) {}

        /**
         * @brief Return the addition symbol
         */
        std::string_view symbol() const override { return "+"; }

        /**
         * @brief Return the precedence of the addition operator
         */
        int precedence() const override { return 2; }

        /**
         * @brief Return whether the addition operator is right associative
         */
        bool right_associative() const override { return false; }

};

/**
 * @brief Subtraction operator
 */
class Subtract : public Binary_Operator {

    public:

        /**
         * @brief Constructor
         */
        Subtract() : Binary_Operator(ast::Binary_Op::SUBTRACT) {}

        /**
         * @brief Return the subtraction symbol
         */
        std::string_view symbol() const override { return "-"; }

        /**
         * @brief Return the precedence of the subtraction operator
         */
        int precedence() const override { return 2; }

        /**
         * @brief Return whether the subtraction operator is right associative
         */
        bool right_associative() const override { return false; }

}; // End of Subtract class

/**
 * @brief Multiplication operator
 */
class Multiply : public Binary_Operator {

    public:

        /**
         * @brief Constructor
         */
        Multiply() : Binary_Operator(ast::Binary_Op::MULTIPLY) {}

        /**
         * @brief Return the multiplication symbol
         */
        std::string_view symbol() const override { return "*"; }

        /**
         * @brief Return the precedence of the multiplication operator
         */
        int precedence() const override { return 3; }

        /**
         * @brief Return whether the multiplication operator is right associative
         */
        bool right_associative() const override { return false; }

}; // End of Multiply class

/**
 * @brief Division operator
 */
class Divide : public Binary_Operator {

    public:

        /**
         * @brief Constructor
         */
        Divide() : Binary_Operator(ast::Binary_Op::DIVIDE) {}

        /**
         * @brief Return the division symbol
         */
        std::string_view symbol() const override { return "/"; }

        /**
         * @brief Return the precedence of the division operator
         */
        int precedence() const override { return 3; }

        /**
         * @brief Return whether the division operator is right associative
         */
        bool right_associative() const override { return false; }
}; // End of Divide class

} // namespace ovb::math::operators
