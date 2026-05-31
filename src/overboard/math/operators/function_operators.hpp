/**
 * @file    function_operators.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Function factory classes
 */
#pragma once

// Project Libraries
#include <overboard/math/ast/binary_op_node.hpp>
#include <overboard/math/ast/constant_node.hpp>
#include <overboard/math/ast/node.hpp>
#include <overboard/math/ast/number_node.hpp>
#include <overboard/math/operators/operator.hpp>

namespace ovb::math::operators {

/**
 * @brief Function base class
 */
class Function : public I_Operator {

    public:

        /**
         * @brief Constructor
         */
        Function(std::string name) : m_name(std::move(name)) {}

        /**
         * @brief Return the number of operands required
         */
        int operand_count() const override { return 1; }

        /**
         * @brief Get operator symbol
         */
        std::string_view symbol() const override { return m_name; }

        /**
         * @brief Get operator precedence (functions have highest precedence)
         */
        int precedence() const override { return 5; }

        /**
         * @brief Check if right-associative (functions are left-associative)
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Create AST node from operands
         */
        ast::Node::ptr_t create_node(
            std::vector<ast::Node::ptr_t> operands) const override {
            if (operands.empty()) {
                return nullptr;
            }
            return std::make_unique<ast::Function_Node>(m_name, std::move(operands));
        }

    protected:

        /// @brief Function name
        std::string m_name;

}; // End of Function class

/**
 * @brief Sine function
 */
class Sin : public Function {
    public:
        Sin() : Function("sin") {}
};

/**
 * @brief Cosine function
 */
class Cos : public Function {
    public:
        Cos() : Function("cos") {}
};

/**
 * @brief Tangent function
 */
class Tan : public Function {
    public:
        Tan() : Function("tan") {}
};

/**
 * @brief Arcsine function
 */
class Asin : public Function {
    public:
        Asin() : Function("asin") {}
};

/**
 * @brief Arccosine function
 */
class Acos : public Function {
    public:
        Acos() : Function("acos") {}
};

/**
 * @brief Arctangent function
 */
class Atan : public Function {
    public:
        Atan() : Function("atan") {}
};

/**
 * @brief Logarithm (base 10) function
 */
class Log : public Function {
    public:
        Log() : Function("log") {}
};

/**
 * @brief Natural logarithm function
 */
class Ln : public Function {
    public:
        Ln() : Function("ln") {}
};

/**
 * @brief Exponential function
 */
class Exp : public Function {
    public:
        Exp() : Function("exp") {}
};

/**
 * @brief Square root function
 */
class Sqrt : public Function {
    public:
        Sqrt() : Function("sqrt") {}
};

/**
 * @brief Ceiling function
 */
class Ceil : public Function {
    public:
        Ceil() : Function("ceil") {}
};

/**
 * @brief Floor function
 */
class Floor : public Function {
    public:
        Floor() : Function("floor") {}
};

/**
 * @brief Absolute value function
 */
class Abs : public Function {
    public:
        Abs() : Function("abs") {}
};

/**
 * @brief Pi constant
 */
class Pi : public I_Operator {

    public:

        /**
         * @brief Return the number of operands (0 for constants)
         */
        int operand_count() const override { return 0; }

        /**
         * @brief Return the symbol for the pi constant
         */
        std::string_view symbol() const override { return "pi"; }

        /**
         * @brief Return the precedence of the pi constant
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the pi constant is right associative
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Create a constant node for pi
         */
        ast::Node::ptr_t create_node([[maybe_unused]] std::vector<ast::Node::ptr_t> operands) const override {
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::PI);
        }
};

/**
 * @brief Euler's number constant
 */
class E : public I_Operator {

    public:

        /**
         * @brief Return the number of operands (0 for constants)
         */
        int operand_count() const override { return 0; }

        /**
         * @brief Return the symbol for the e constant
         */
        std::string_view symbol() const override { return "e"; }

        /**
         * @brief Return the precedence of the e constant
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the e constant is right associative
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Create a constant node for e
         */
        ast::Node::ptr_t create_node([[maybe_unused]] std::vector<ast::Node::ptr_t> operands) const override {
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::E);
        }
};

/**
 * @brief Phi constant (golden ratio)
 */
class Phi : public I_Operator {

    public:

        /**
         * @brief Return the number of operands (0 for constants)
         */
        int operand_count() const override { return 0; }

        /**
         * @brief Return the symbol for the phi constant
         */
        std::string_view symbol() const override { return "phi"; }

        /**
         * @brief Return the precedence of the phi constant
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the phi constant is right associative
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Create a constant node for phi
         */
        ast::Node::ptr_t create_node([[maybe_unused]] std::vector<ast::Node::ptr_t> operands) const override {
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::PHI);
        }
};

/**
 * @brief Tau constant (2*pi)
 */
class Tau : public I_Operator {

    public:

        /**
         * @brief Return the number of operands (0 for constants)
         */
        int operand_count() const override { return 0; }

        /**
         * @brief Return the symbol for the tau constant
         */
        std::string_view symbol() const override { return "tau"; }

        /**
         * @brief Return the precedence of the tau constant
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the tau constant is right associative
         */
        bool right_associative() const override { return false; }
        ast::Node::ptr_t create_node([[maybe_unused]] std::vector<ast::Node::ptr_t> operands) const override {
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::TAU);
        }
};

/**
 * @brief Approximation function
 */
class Approx : public Function {
    public:
        Approx() : Function("approx") {}
};

/**
 * @brief Reciprocal operator (1/x)
 */
class Reciprocal : public I_Operator {

    public:

        /**
         * @brief Return the number of operands (1 for unary operators)
         */
        int operand_count() const override { return 1; }

        /**
         * @brief Return the symbol for the reciprocal operator
         */
        std::string_view symbol() const override { return "1/"; }

        /**
         * @brief Return the precedence of the reciprocal operator
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the reciprocal operator is right associative
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Create a binary operation node for reciprocal
         */
        ast::Node::ptr_t create_node(std::vector<ast::Node::ptr_t> operands) const override {
            if (operands.empty()) {
                return nullptr;
            }
            auto left = std::make_unique<ast::Number_Node>(1.0);
            auto right = std::move(operands[0]);
            return std::make_unique<ast::Binary_Op_Node>(ast::Binary_Op::DIVIDE, std::move(left), std::move(right));
        }
};

/**
 * @brief Power-2 operator (x^2)
 */
class Power_2 : public I_Operator {

    public:

        /**
         * @brief Return the number of operands (1 for unary operators)
         */
        int operand_count() const override { return 1; }

        /**
         * @brief Return the symbol for the power-2 operator
         */
        std::string_view symbol() const override { return "^2"; }

        /**
         * @brief Return the precedence of the power-2 operator
         */
        int precedence() const override { return 5; }

        /**
         * @brief Return whether the power-2 operator is right associative
         */
        bool right_associative() const override { return false; }

        /**
         * @brief Create a binary operation node for power-2
         */
        ast::Node::ptr_t create_node(std::vector<ast::Node::ptr_t> operands) const override {
            if (operands.empty()) {
                return nullptr;
            }
            auto left = std::move(operands[0]);
            auto right = std::make_unique<ast::Number_Node>(2.0);
            return std::make_unique<ast::Binary_Op_Node>(ast::Binary_Op::POWER, std::move(left), std::move(right));
        }
};

} // namespace ovb::math::operators
