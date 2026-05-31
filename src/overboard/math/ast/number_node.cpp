/**
 * @file    number_node.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Number_Node implementation
 */
#include <overboard/math/ast/number_node.hpp>

// C++ Standard Libraries
#include <cmath>

namespace ovb::math::ast {

/***************************/
/*       To String         */
/***************************/
std::string Number_Node::to_string() const {
    // Check if value is effectively an integer
    if (std::abs(m_value - std::floor(m_value)) < 1e-10 && std::abs(m_value) < 1e15)
        return std::to_string(static_cast<long long>(m_value));

    // For decimal values, format without trailing zeros
    std::string str = std::to_string(m_value);
    // Remove trailing zeros after decimal point
    size_t decimal_pos = str.find('.');
    if (decimal_pos != std::string::npos) {
        size_t last_non_zero = str.find_last_not_of('0');
        if (last_non_zero != std::string::npos && last_non_zero > decimal_pos) {
            str = str.substr(0, last_non_zero + 1);
        } else if (last_non_zero == decimal_pos) {
            // All zeros after decimal, remove decimal point too
            str = str.substr(0, decimal_pos);
        }
    }
    return str;
}

/***************************/
/*       To LaTeX          */
/***************************/
std::string Number_Node::to_latex() const {
    return to_string();
}

} // namespace ovb::math::ast
