#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

namespace Ast {

// ─── Forward declarations ────────────────────────────────────────────────────

struct Node;
using Node_Ptr = std::unique_ptr<Node>;

// ─── Node kinds ──────────────────────────────────────────────────────────────

enum class Node_Kind {
    NUMBER,       // literal double
    CONSTANT,     // symbolic constant — pi, e
    BINARY_OP,    // left op right
    UNARY_OP,     // op operand
    FUNCTION,     // named function, one or more args
    FACTORIAL,    // n!
};

enum class Binary_Op {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    POWER,
    MODULO,
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,
};

enum class Unary_Op {
    NEGATE,
    BIT_NOT,
    PERCENT,
};

enum class Constant_Id {
    PI,
    E,
    PHI,
    TAU,
};

// ─── Base node ───────────────────────────────────────────────────────────────

struct Node {
    Node_Kind kind;
    explicit Node(Node_Kind k) : kind(k) {}
    virtual ~Node() = default;

    virtual double      eval()      const = 0;
    virtual std::string to_latex()  const = 0;
    virtual std::string to_string() const = 0;
};

// ─── Concrete nodes ──────────────────────────────────────────────────────────

struct Number_Node : Node {
    double value;

    explicit Number_Node(double v)
        : Node(Node_Kind::NUMBER), value(v) {}

    double      eval()      const override { return value; }
    std::string to_latex()  const override;
    std::string to_string() const override;
};

struct Constant_Node : Node {
    Constant_Id id;

    explicit Constant_Node(Constant_Id c)
        : Node(Node_Kind::CONSTANT), id(c) {}

    double eval() const override {
        switch (id) {
            case Constant_Id::PI:  return M_PI;
            case Constant_Id::E:   return M_E;
            case Constant_Id::PHI: return (1.0 + std::sqrt(5.0)) / 2.0;
            case Constant_Id::TAU: return 2.0 * M_PI;
        }
        return 0.0;
    }

    std::string to_latex() const override {
        switch (id) {
            case Constant_Id::PI:  return "\\pi";
            case Constant_Id::E:   return "e";
            case Constant_Id::PHI: return "\\varphi";
            case Constant_Id::TAU: return "\\tau";
        }
        return "";
    }

    std::string to_string() const override {
        switch (id) {
            case Constant_Id::PI:  return "pi";
            case Constant_Id::E:   return "e";
            case Constant_Id::PHI: return "phi";
            case Constant_Id::TAU: return "tau";
        }
        return "";
    }
};

struct Binary_Op_Node : Node {
    Binary_Op op;
    Node_Ptr  left;
    Node_Ptr  right;

    Binary_Op_Node(Binary_Op o, Node_Ptr l, Node_Ptr r)
        : Node(Node_Kind::BINARY_OP), op(o),
          left(std::move(l)), right(std::move(r)) {}

    double eval() const override;
    std::string to_latex()  const override;
    std::string to_string() const override;
};

struct Unary_Op_Node : Node {
    Unary_Op op;
    Node_Ptr operand;

    Unary_Op_Node(Unary_Op o, Node_Ptr operand)
        : Node(Node_Kind::UNARY_OP), op(o),
          operand(std::move(operand)) {}

    double eval() const override {
        double v = operand->eval();
        switch (op) {
            case Unary_Op::NEGATE:  return -v;
            case Unary_Op::BIT_NOT: return static_cast<double>(~static_cast<long long>(v));
            case Unary_Op::PERCENT: return v / 100.0;
        }
        return v;
    }

    std::string to_latex()  const override;
    std::string to_string() const override;
};

struct Function_Node : Node {
    std::string              name;
    std::vector<Node_Ptr>    args;

    Function_Node(std::string n, std::vector<Node_Ptr> a)
        : Node(Node_Kind::FUNCTION), name(std::move(n)),
          args(std::move(a)) {}

    double eval() const override;
    std::string to_latex()  const override;
    std::string to_string() const override;
};

struct Factorial_Node : Node {
    Node_Ptr operand;

    explicit Factorial_Node(Node_Ptr o)
        : Node(Node_Kind::FACTORIAL), operand(std::move(o)) {}

    double eval() const override {
        int n = static_cast<int>(operand->eval());
        if (n < 0) throw std::runtime_error("Factorial of negative");
        double f = 1.0;
        for (int i = 2; i <= n; ++i) f *= i;
        return f;
    }

    std::string to_latex()  const override { return operand->to_latex() + "!"; }
    std::string to_string() const override { return operand->to_string() + "!"; }
};

// ─── Inline implementations ──────────────────────────────────────────────────

inline double Binary_Op_Node::eval() const {
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

inline double Function_Node::eval() const {
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

inline std::string Binary_Op_Node::to_string() const {
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

inline std::string Binary_Op_Node::to_latex() const {
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

inline std::string Unary_Op_Node::to_string() const {
    switch (op) {
        case Unary_Op::NEGATE:  return "(-" + operand->to_string() + ")";
        case Unary_Op::BIT_NOT: return "~" + operand->to_string();
        case Unary_Op::PERCENT: return operand->to_string() + "%";
    }
    return operand->to_string();
}

inline std::string Unary_Op_Node::to_latex() const {
    switch (op) {
        case Unary_Op::NEGATE:  return "-" + operand->to_latex();
        case Unary_Op::PERCENT: return operand->to_latex() + "\\%";
        default:                return to_string();
    }
}

inline std::string Function_Node::to_string() const {
    std::string s = name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i) s += ",";
        s += args[i]->to_string();
    }
    return s + ")";
}

inline std::string Function_Node::to_latex() const {
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

inline std::string Number_Node::to_string() const {
    if (value == std::floor(value) && std::abs(value) < 1e15)
        return std::to_string(static_cast<long long>(value));
    return std::to_string(value);
}

inline std::string Number_Node::to_latex() const { return to_string(); }

} // namespace Ast
