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
    if (m_value == std::floor(m_value) && std::abs(m_value) < 1e15)
        return std::to_string(static_cast<long long>(m_value));
    return std::to_string(m_value);
}

/***************************/
/*       To LaTeX          */
/***************************/
std::string Number_Node::to_latex() const {
    return to_string();
}

} // namespace ovb::math::ast
