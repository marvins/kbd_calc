
// C++ Standard Libraries
#include <cmath>
#include <stdexcept>

// Project Libraries
#include <overboard/core/ast/ast.hpp>

namespace ast {

// ─── Binary_Op_Node ──────────────────────────────────────────────────────────

double Binary_Op_Node::eval() const {
    double l = left->eval();
    double r = right->eval();
    switch (op) {
        case Binary_Op::ADD:         return l + r;
        case Binary_Op::SUBTRACT:    return l - r;
        case Binary_Op::MULTIPLY:    return l * r;
        case Binary_Op::DIVIDE:
            if (r == 0.0) throw std::runtime_error("Division by zero");
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

std::string Binary_Op_Node::to_string() const {
    std::string ops;
    switch (op) {
        case Binary_Op::ADD:         ops = "+";  break;
        case Binary_Op::SUBTRACT:    ops = "-";  break;
        case Binary_Op::MULTIPLY:    ops = "*";  break;
        case Binary_Op::DIVIDE:      ops = "/";  break;
        case Binary_Op::POWER:       ops = "^";  break;
        case Binary_Op::MODULO:      ops = "%";  break;
        case Binary_Op::BIT_AND:     ops = "&";  break;
        case Binary_Op::BIT_OR:      ops = "|";  break;
        case Binary_Op::BIT_XOR:     ops = "^";  break;
        case Binary_Op::SHIFT_LEFT:  ops = "<<"; break;
        case Binary_Op::SHIFT_RIGHT: ops = ">>"; break;
    }
    return "(" + left->to_string() + ops + right->to_string() + ")";
}

std::string Binary_Op_Node::to_latex() const {
    switch (op) {
        case Binary_Op::DIVIDE:
            return "\\frac{" + left->to_latex() + "}{" + right->to_latex() + "}";
        case Binary_Op::MULTIPLY:
            return left->to_latex() + " \\cdot " + right->to_latex();
        case Binary_Op::POWER:
            return left->to_latex() + "^{" + right->to_latex() + "}";
        case Binary_Op::ADD:
            return left->to_latex() + "+" + right->to_latex();
        case Binary_Op::SUBTRACT:
            return left->to_latex() + "-" + right->to_latex();
        default:
            return to_string();
    }
}

Node_Ptr Binary_Op_Node::clone() const {
    return std::make_unique<Binary_Op_Node>(op, left->clone(), right->clone());
}

Node_Ptr Binary_Op_Node::simplify() const {
    Node_Ptr l = left->simplify();
    Node_Ptr r = right->simplify();
    if (l->kind == Node_Kind::NUMBER && r->kind == Node_Kind::NUMBER) {
        return std::make_unique<Number_Node>(
            Binary_Op_Node(op, std::move(l), std::move(r)).eval());
    }
    return std::make_unique<Binary_Op_Node>(op, std::move(l), std::move(r));
}

// ─── Unary_Op_Node ───────────────────────────────────────────────────────────

std::string Unary_Op_Node::to_string() const {
    switch (op) {
        case Unary_Op::NEGATE:  return "(-" + operand->to_string() + ")";
        case Unary_Op::BIT_NOT: return "~" + operand->to_string();
        case Unary_Op::PERCENT: return operand->to_string() + "%";
    }
    return operand->to_string();
}

std::string Unary_Op_Node::to_latex() const {
    switch (op) {
        case Unary_Op::NEGATE:  return "-" + operand->to_latex();
        case Unary_Op::PERCENT: return operand->to_latex() + "\\%";
        default:                return to_string();
    }
}

// ─── Function_Node ───────────────────────────────────────────────────────────

double Function_Node::eval() const {
    if (args.empty()) throw std::runtime_error("Function missing args: " + name);
    double a = args[0]->eval();
    if (name == "sin")  return std::sin(a);
    if (name == "cos")  return std::cos(a);
    if (name == "tan")  return std::tan(a);
    if (name == "asin") return std::asin(a);
    if (name == "acos") return std::acos(a);
    if (name == "atan") return std::atan(a);
    if (name == "log")  return std::log10(a);
    if (name == "ln")   return std::log(a);
    if (name == "exp")  return std::exp(a);
    if (name == "sqrt") return std::sqrt(a);
    throw std::runtime_error("Unknown function: " + name);
}

std::string Function_Node::to_string() const {
    std::string s = name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i) s += ",";
        s += args[i]->to_string();
    }
    return s + ")";
}

std::string Function_Node::to_latex() const {
    if (name == "sqrt" && args.size() == 1)
        return "\\sqrt{" + args[0]->to_latex() + "}";
    if (name == "log" && args.size() == 1)
        return "\\log\\left(" + args[0]->to_latex() + "\\right)";
    if (name == "ln" && args.size() == 1)
        return "\\ln\\left(" + args[0]->to_latex() + "\\right)";
    std::string s = "\\" + name + "\\left(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i) s += ",";
        s += args[i]->to_latex();
    }
    return s + "\\right)";
}

Node_Ptr Function_Node::clone() const {
    std::vector<Node_Ptr> a;
    a.reserve(args.size());
    for (const auto& arg : args) a.push_back(arg->clone());
    return std::make_unique<Function_Node>(name, std::move(a));
}

Node_Ptr Function_Node::simplify() const {
    if (name == "approx" && args.size() == 1) {
        return std::make_unique<Number_Node>(args[0]->eval());
    }

    std::vector<Node_Ptr> simplified_args;
    simplified_args.reserve(args.size());
    bool all_numbers = true;
    for (const auto& arg : args) {
        simplified_args.push_back(arg->simplify());
        if (simplified_args.back()->kind != Node_Kind::NUMBER)
            all_numbers = false;
    }
    if (all_numbers) {
        return std::make_unique<Number_Node>(
            Function_Node(name, std::move(simplified_args)).eval());
    }
    return std::make_unique<Function_Node>(name, std::move(simplified_args));
}

// ─── Number_Node ─────────────────────────────────────────────────────────────

std::string Number_Node::to_string() const {
    if (value == std::floor(value) && std::abs(value) < 1e15)
        return std::to_string(static_cast<long long>(value));
    return std::to_string(value);
}

std::string Number_Node::to_latex() const { return to_string(); }

} // namespace ast
