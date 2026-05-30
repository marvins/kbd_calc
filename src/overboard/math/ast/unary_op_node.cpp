/**
 * @file    unary_op_node.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Unary_Op_Node implementation
 */
#include <overboard/math/ast/unary_op_node.hpp>

namespace ovb::math::ast {

/**********************************/
/*        Convert to String       */
/**********************************/
std::string Unary_Op_Node::to_string() const {
    switch (m_op) {
        case Unary_Op::NEGATE:  return "(-" + m_operand->to_string() + ")";
        case Unary_Op::BIT_NOT: return "~" + m_operand->to_string();
        case Unary_Op::PERCENT: return m_operand->to_string() + "%";
    }
    return m_operand->to_string();
}

/***************************/
/*       To LaTeX          */
/***************************/
std::string Unary_Op_Node::to_latex() const {
    switch (m_op) {
        case Unary_Op::NEGATE:  return "-" + m_operand->to_latex();
        case Unary_Op::PERCENT: return m_operand->to_latex() + "\\%";
        default:                return to_string();
    }
}

} // namespace ovb::math::ast
