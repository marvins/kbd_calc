/**
 * @file    operator_traits.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Template traits for operator metadata
 */
#pragma once

// Project Libraries
#include <overboard/math/operators/binary_operators.hpp>
#include <overboard/math/operators/unary_operators.hpp>

namespace ovb::math::operators {

/**
 * @brief Template traits for operator classes
 */
template<typename Op>
struct Operator_Traits;

/**
 * @brief Traits for the Add operator
 */
template<>
struct Operator_Traits<Add> {

    /// @brief Symbol for the operator
    static constexpr const char* symbol = "+";

    /// @brief Precedence level for the operator
    static constexpr int precedence = 2;

    /// @brief Whether the operator is right-associative
    static constexpr bool right_associative = false;

    /// @brief Type of the operator
    using type = Add;

}; // End of Operator_Traits<Add> struct

/**
 * @brief Traits for the Subtract operator
 */
template<>
struct Operator_Traits<Subtract> {

    /// @brief Symbol for the operator
    static constexpr const char* symbol = "-";

    /// @brief Precedence level for the operator
    static constexpr int precedence = 2;

    /// @brief Whether the operator is right-associative
    static constexpr bool right_associative = false;

    /// @brief Type of the operator
    using type = Subtract;

}; // End of Operator_Traits<Subtract> struct

/**
 * @brief Traits for the Multiply operator
 */
template<>
struct Operator_Traits<Multiply> {

    /// @brief Symbol for the operator
    static constexpr const char* symbol = "*";

    /// @brief Precedence level for the operator
    static constexpr int precedence = 3;

    /// @brief Whether the operator is right-associative
    static constexpr bool right_associative = false;

    /// @brief Type of the operator
    using type = Multiply;

}; // End of Operator_Traits<Multiply> struct

/**
 * @brief Traits for the Divide operator
 */
template<>
struct Operator_Traits<Divide> {

    /// @brief Symbol for the operator
    static constexpr const char* symbol = "/";

    /// @brief Precedence level for the operator
    static constexpr int precedence = 3;

    /// @brief Whether the operator is right-associative
    static constexpr bool right_associative = false;

    /// @brief Type of the operator
    using type = Divide;

}; // End of Operator_Traits<Divide> struct

/**
 * @brief Traits for the Negate operator
 */
template<>
struct Operator_Traits<Negate> {

    /// @brief Symbol for the operator
    static constexpr const char* symbol = "-";

    /// @brief Precedence level for the operator
    static constexpr int precedence = 4;

    /// @brief Whether the operator is right-associative
    static constexpr bool right_associative = false;

    /// @brief Type of the operator
    using type = Negate;

}; // End of Operator_Traits<Negate> struct

} // namespace ovb::math::operators
