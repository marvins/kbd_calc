/**
 * @file    binary_op_node.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Binary_Op_Node implementation
 */
#include <overboard/math/ast/binary_op_node.hpp>

// C++ Standard Libraries
#include <cmath>
#include <limits>

// Project Libraries
#include <overboard/math/ast/number_node.hpp>

namespace ovb::math::ast {

/***************************/
/*         Evaluate        */
/***************************/
double Binary_Op_Node::eval() const {
    double l = m_left->eval();
    double r = m_right->eval();
    switch (m_op) {
        case Binary_Op::ADD:         return l + r;
        case Binary_Op::SUBTRACT:    return l - r;
        case Binary_Op::MULTIPLY:    return l * r;
        case Binary_Op::DIVIDE:
            if (r == 0.0) return std::numeric_limits<double>::quiet_NaN();
            return l / r;
        case Binary_Op::POWER:       return std::pow(l, r);
        case Binary_Op::MODULO:      return std::fmod(l, r);
        case Binary_Op::BIT_AND:     return static_cast<double>(static_cast<long long>(l) & static_cast<long long>(r));
        case Binary_Op::BIT_OR:      return static_cast<double>(static_cast<long long>(l) | static_cast<long long>(r));
        case Binary_Op::BIT_XOR:     return static_cast<double>(static_cast<long long>(l) ^ static_cast<long long>(r));
        case Binary_Op::SHIFT_LEFT:  return static_cast<double>(static_cast<long long>(l) << static_cast<int>(r));
        case Binary_Op::SHIFT_RIGHT: return static_cast<double>(static_cast<long long>(l) >> static_cast<int>(r));
    }
    return 0.0;
}

/***************************/
/*       To String         */
/***************************/
std::string Binary_Op_Node::to_string() const {
    switch (m_op) {
        case Binary_Op::DIVIDE:     return "(" + m_left->to_string() + "/" + m_right->to_string() + ")";
        case Binary_Op::MULTIPLY:   return "(" + m_left->to_string() + "*" + m_right->to_string() + ")";
        case Binary_Op::POWER:      return "(" + m_left->to_string() + "^" + m_right->to_string() + ")";
        case Binary_Op::ADD:        return "(" + m_left->to_string() + "+" + m_right->to_string() + ")";
        case Binary_Op::SUBTRACT:   return "(" + m_left->to_string() + "-" + m_right->to_string() + ")";
        case Binary_Op::MODULO:     return "(" + m_left->to_string() + "%" + m_right->to_string() + ")";
        case Binary_Op::BIT_AND:     return "(" + m_left->to_string() + "&" + m_right->to_string() + ")";
        case Binary_Op::BIT_OR:     return "(" + m_left->to_string() + "|" + m_right->to_string() + ")";
        case Binary_Op::BIT_XOR:    return "(" + m_left->to_string() + "^" + m_right->to_string() + ")";
        case Binary_Op::SHIFT_LEFT: return "(" + m_left->to_string() + "<<" + m_right->to_string() + ")";
        case Binary_Op::SHIFT_RIGHT:return "(" + m_left->to_string() + ">>" + m_right->to_string() + ")";
    }
    return "";
}

/***************************/
/*       To LaTeX          */
/***************************/
std::string Binary_Op_Node::to_latex() const {
    switch (m_op) {
        case Binary_Op::DIVIDE:
            return "\\frac{" + m_left->to_latex() + "}{" + m_right->to_latex() + "}";
        case Binary_Op::MULTIPLY:
            return m_left->to_latex() + " \\cdot " + m_right->to_latex();
        case Binary_Op::POWER:
            return m_left->to_latex() + "^{" + m_right->to_latex() + "}";
        case Binary_Op::ADD:
            return m_left->to_latex() + "+" + m_right->to_latex();
        case Binary_Op::SUBTRACT:
            return m_left->to_latex() + "-" + m_right->to_latex();
        default:
            return to_string();
    }
}

/***************************/
/*         Clone           */
/***************************/
Node::ptr_t Binary_Op_Node::clone() const {
    return std::make_unique<Binary_Op_Node>(m_op, m_left->clone(), m_right->clone());
}

/***************************/
/*        Simplify         */
/***************************/
Node::ptr_t Binary_Op_Node::simplify() const {
    Node::ptr_t l = m_left->simplify();
    Node::ptr_t r = m_right->simplify();

    // Fold: if both children are plain numbers and the result is an exact integer,
    // evaluate now (e.g. 2+3=5, 10/2=5). Leave non-integer results symbolic
    // (e.g. 10/4=2.5, sqrt(2)/4) so the user sees exact form until approx() is used.
    //
    // Note: this is a float-based heuristic, not a true symbolic type system (cf. SymPy/GiNaC).
    // A value is considered "exact integer" if it is within 1e-10 of a whole number.
    if (l->kind() == Node_Kind::NUMBER && r->kind() == Node_Kind::NUMBER) {
        const double result = Binary_Op_Node(m_op, l->clone(), r->clone()).eval();
        if (std::abs(result - std::floor(result)) < 1e-10 && std::abs(result) < 1e15) {
            return std::make_unique<Number_Node>(result);
        }
    }

    // Otherwise keep symbolic
    return std::make_unique<Binary_Op_Node>(m_op, std::move(l), std::move(r));
}

} // namespace ovb::math::ast
