#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

namespace ovb::ast {

// ─── Forward declarations ────────────────────────────────────────────────────

struct Node;
using Node_Ptr = std::unique_ptr<Node>;

// ─── Node kinds ──────────────────────────────────────────────────────────────

enum class Node_Kind {
    NUMBER,       // literal double
    CONSTANT,     // symbolic constant — pi, e
    VARIABLE,     // variable name — x, y, n
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

    // Returns a new, possibly simplified tree. Default: identity (deep clone).
    // Exact mode calls this before eval() to reduce rational / symbolic sub-expressions.
    virtual Node_Ptr    simplify()  const = 0;
    virtual Node_Ptr    clone()     const = 0;
};

// ─── Concrete nodes ──────────────────────────────────────────────────────────

struct Number_Node : Node {
    double value;

    explicit Number_Node(double v)
        : Node(Node_Kind::NUMBER), value(v) {}

    double      eval()      const override { return value; }
    std::string to_latex()  const override;
    std::string to_string() const override;
    Node_Ptr    clone()     const override { return std::make_unique<Number_Node>(value); }
    Node_Ptr    simplify()  const override { return clone(); }
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

    Node_Ptr clone()    const override { return std::make_unique<Constant_Node>(id); }
    Node_Ptr simplify() const override { return clone(); } // stays symbolic in exact mode

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

struct Variable_Node : Node {
    std::string name;

    explicit Variable_Node(std::string n)
        : Node(Node_Kind::VARIABLE), name(std::move(n)) {}

    double eval() const override {
        throw std::runtime_error("Cannot evaluate variable '" + name + "' without a value");
    }

    std::string to_latex() const override { return name; }
    std::string to_string() const override { return name; }
    Node_Ptr clone() const override { return std::make_unique<Variable_Node>(name); }
    Node_Ptr simplify() const override { return clone(); }
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
    Node_Ptr    clone()     const override;
    Node_Ptr    simplify()  const override;
};

struct Unary_Op_Node : Node {
    Unary_Op op;
    Node_Ptr operand;

    Unary_Op_Node(Unary_Op o, Node_Ptr operand_)
        : Node(Node_Kind::UNARY_OP), op(o),
          operand(std::move(operand_)) {}

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
    Node_Ptr    clone()     const override {
        return std::make_unique<Unary_Op_Node>(op, operand->clone());
    }
    Node_Ptr    simplify()  const override {
        return std::make_unique<Unary_Op_Node>(op, operand->simplify());
    }
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
    Node_Ptr    clone()     const override;
    Node_Ptr    simplify()  const override;
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
    Node_Ptr    clone()     const override {
        return std::make_unique<Factorial_Node>(operand->clone());
    }
    Node_Ptr    simplify()  const override {
        return std::make_unique<Factorial_Node>(operand->simplify());
    }
};

} // namespace ovb::ast
