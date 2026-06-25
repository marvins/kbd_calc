/**
 * @file    function_node.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Function_Node implementation
 */
#include <overboard/math/ast/function_node.hpp>

// C++ Standard Libraries
#include <algorithm>
#include <cmath>
#include <limits>

// Project Libraries
#include <overboard/math/ast/number_node.hpp>

namespace ovb::math::ast {

/***************************/
/*         Evaluate        */
/***************************/
double Function_Node::eval() const {
    if (m_args.empty()) return std::numeric_limits<double>::quiet_NaN();
    double a = m_args[0]->eval();
    if (m_name == "sin")  return std::sin(a);
    if (m_name == "cos")  return std::cos(a);
    if (m_name == "tan")  return std::tan(a);
    if (m_name == "asin") return std::asin(a);
    if (m_name == "acos") return std::acos(a);
    if (m_name == "atan") return std::atan(a);
    if (m_name == "log")  return std::log10(a);
    if (m_name == "ln")   return std::log(a);
    if (m_name == "exp")  return std::exp(a);
    if (m_name == "sqrt") return std::sqrt(a);
    if (m_name == "mod") {
        if (m_args.size() < 2) return std::numeric_limits<double>::quiet_NaN();
        double b = m_args[1]->eval();
        return std::fmod(a, b);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

/***************************/
/*       To String         */
/***************************/
std::string Function_Node::to_string() const {
    std::string s = m_name + "(";
    for (size_t i = 0; i < m_args.size(); ++i) {
        if (i) s += ",";
        s += m_args[i]->to_string();
    }
    return s + ")";
}

/***************************/
/*       To LaTeX          */
/***************************/
std::string Function_Node::to_latex() const {
    if (m_name == "sqrt" && m_args.size() == 1)
        return "\\sqrt{" + m_args[0]->to_latex() + "}";
    if (m_name == "log" && m_args.size() == 1)
        return "\\log\\left(" + m_args[0]->to_latex() + "\\right)";
    if (m_name == "ln" && m_args.size() == 1)
        return "\\ln\\left(" + m_args[0]->to_latex() + "\\right)";
    std::string s = "\\" + m_name + "\\left(";
    for (size_t i = 0; i < m_args.size(); ++i) {
        if (i) s += ",";
        s += m_args[i]->to_latex();
    }
    return s + "\\right)";
}

/***************************/
/*         Clone           */
/***************************/
Node::ptr_t Function_Node::clone() const {
    std::vector<Node::ptr_t> a;
    a.reserve(m_args.size());
    for (const auto& arg : m_args) a.push_back(arg->clone());
    return std::make_unique<Function_Node>(m_name, std::move(a));
}

/***************************/
/*        Simplify         */
/***************************/
Node::ptr_t Function_Node::simplify() const {
    // approx() always forces numeric evaluation regardless of argument type
    if (m_name == "approx" && m_args.size() == 1) {
        return std::make_unique<Number_Node>(m_args[0]->eval());
    }

    // Simplify all arguments first
    std::vector<Node::ptr_t> simplified_args;
    simplified_args.reserve(m_args.size());
    for (const auto& arg : m_args) {
        simplified_args.push_back(arg->simplify());
    }

    // Fold: if all simplified args are plain numbers, evaluate now
    const bool all_numeric = std::all_of(simplified_args.begin(), simplified_args.end(),
        [](const Node::ptr_t& n) { return n->kind() == Node_Kind::NUMBER; });

    if (all_numeric) {
        std::vector<Node::ptr_t> copies;
        copies.reserve(simplified_args.size());
        for (const auto& a : simplified_args) copies.push_back(a->clone());
        const double result = Function_Node(m_name, std::move(copies)).eval();
        if (std::abs(result - std::floor(result)) < 1e-10 && std::abs(result) < 1e15) {
            return std::make_unique<Number_Node>(result);
        }
    }

    // Otherwise stay symbolic (e.g. sqrt(pi), sin(pi/3))
    return std::make_unique<Function_Node>(m_name, std::move(simplified_args));
}

} // namespace ovb::math::ast
