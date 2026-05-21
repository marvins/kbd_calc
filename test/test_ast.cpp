// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/ast/ast.hpp>

using namespace ovb::ast;

// ─── Number_Node ─────────────────────────────────────────────────────────────

TEST(Ast_Number, Eval) {
    Number_Node n(42.0);
    EXPECT_DOUBLE_EQ(n.eval(), 42.0);
}

TEST(Ast_Number, To_String_Integer) {
    EXPECT_EQ(Number_Node(7.0).to_string(), "7");
}

TEST(Ast_Number, To_Latex_Integer) {
    EXPECT_EQ(Number_Node(3.0).to_latex(), "3");
}

// ─── Constant_Node ───────────────────────────────────────────────────────────

TEST(Ast_Constant, Pi_Eval) {
    Constant_Node c(Constant_Id::PI);
    EXPECT_NEAR(c.eval(), 3.14159265358979, 1e-10);
}

TEST(Ast_Constant, E_Eval) {
    Constant_Node c(Constant_Id::E);
    EXPECT_NEAR(c.eval(), 2.71828182845904, 1e-10);
}

TEST(Ast_Constant, Pi_Latex) {
    EXPECT_EQ(Constant_Node(Constant_Id::PI).to_latex(), "\\pi");
}

TEST(Ast_Constant, E_Latex) {
    EXPECT_EQ(Constant_Node(Constant_Id::E).to_latex(), "e");
}

// ─── Binary_Op_Node ──────────────────────────────────────────────────────────

TEST(Ast_Binary, Add) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_DOUBLE_EQ(n->eval(), 7.0);
}

TEST(Ast_Binary, Subtract) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::SUBTRACT,
        std::make_unique<Number_Node>(10.0),
        std::make_unique<Number_Node>(3.0));
    EXPECT_DOUBLE_EQ(n->eval(), 7.0);
}

TEST(Ast_Binary, Multiply) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(6.0),
        std::make_unique<Number_Node>(7.0));
    EXPECT_DOUBLE_EQ(n->eval(), 42.0);
}

TEST(Ast_Binary, Divide) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::DIVIDE,
        std::make_unique<Number_Node>(10.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_DOUBLE_EQ(n->eval(), 2.5);
}

TEST(Ast_Binary, Divide_By_Zero) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::DIVIDE,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(0.0));
    EXPECT_THROW(n->eval(), std::runtime_error);
}

TEST(Ast_Binary, Power) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::POWER,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 1024.0);
}

TEST(Ast_Binary, Modulo) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MODULO,
        std::make_unique<Number_Node>(10.0),
        std::make_unique<Number_Node>(3.0));
    EXPECT_DOUBLE_EQ(n->eval(), 1.0);
}

TEST(Ast_Binary, Bit_And) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::BIT_AND,
        std::make_unique<Number_Node>(12.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 8.0); // 1100 & 1010 = 1000
}

TEST(Ast_Binary, Bit_Or) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::BIT_OR,
        std::make_unique<Number_Node>(12.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 14.0); // 1100 | 1010 = 1110
}

TEST(Ast_Binary, Bit_Xor) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::BIT_XOR,
        std::make_unique<Number_Node>(12.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 6.0); // 1100 ^ 1010 = 0110
}

TEST(Ast_Binary, Shift_Left) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::SHIFT_LEFT,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_DOUBLE_EQ(n->eval(), 16.0);
}

TEST(Ast_Binary, Shift_Right) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::SHIFT_RIGHT,
        std::make_unique<Number_Node>(16.0),
        std::make_unique<Number_Node>(2.0));
    EXPECT_DOUBLE_EQ(n->eval(), 4.0);
}

// ─── Binary to_latex ─────────────────────────────────────────────────────────

TEST(Ast_Binary, Divide_Latex) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::DIVIDE,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(2.0));
    EXPECT_EQ(n->to_latex(), "\\frac{1}{2}");
}

TEST(Ast_Binary, Power_Latex) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::POWER,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(8.0));
    EXPECT_EQ(n->to_latex(), "2^{8}");
}

TEST(Ast_Binary, Multiply_Latex) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_EQ(n->to_latex(), "3 \\cdot 4");
}

// ─── Unary_Op_Node ───────────────────────────────────────────────────────────

TEST(Ast_Unary, Negate) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::NEGATE,
        std::make_unique<Number_Node>(5.0));
    EXPECT_DOUBLE_EQ(n->eval(), -5.0);
}

TEST(Ast_Unary, Percent) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::PERCENT,
        std::make_unique<Number_Node>(50.0));
    EXPECT_DOUBLE_EQ(n->eval(), 0.5);
}

TEST(Ast_Unary, Bit_Not) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::BIT_NOT,
        std::make_unique<Number_Node>(0.0));
    EXPECT_DOUBLE_EQ(n->eval(), -1.0); // ~0 = -1 (two's complement)
}

// ─── Function_Node ───────────────────────────────────────────────────────────

TEST(Ast_Function, Sin) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(0.0));
    EXPECT_DOUBLE_EQ(Function_Node("sin", std::move(args)).eval(), 0.0);
}

TEST(Ast_Function, Cos) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(0.0));
    EXPECT_DOUBLE_EQ(Function_Node("cos", std::move(args)).eval(), 1.0);
}

TEST(Ast_Function, Sqrt) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(9.0));
    EXPECT_DOUBLE_EQ(Function_Node("sqrt", std::move(args)).eval(), 3.0);
}

TEST(Ast_Function, Log) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(100.0));
    EXPECT_DOUBLE_EQ(Function_Node("log", std::move(args)).eval(), 2.0);
}

TEST(Ast_Function, Ln) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(1.0));
    EXPECT_DOUBLE_EQ(Function_Node("ln", std::move(args)).eval(), 0.0);
}

TEST(Ast_Function, Unknown_Throws) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(1.0));
    EXPECT_THROW(Function_Node("foo", std::move(args)).eval(), std::runtime_error);
}

TEST(Ast_Function, Sqrt_Latex) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(2.0));
    EXPECT_EQ(Function_Node("sqrt", std::move(args)).to_latex(), "\\sqrt{2}");
}

TEST(Ast_Function, Sin_Latex) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(0.0));
    EXPECT_EQ(Function_Node("sin", std::move(args)).to_latex(), "\\sin\\left(0\\right)");
}

// ─── Factorial_Node ──────────────────────────────────────────────────────────

TEST(Ast_Factorial, Basic) {
    EXPECT_DOUBLE_EQ(
        Factorial_Node(std::make_unique<Number_Node>(5.0)).eval(), 120.0);
}

TEST(Ast_Factorial, Zero) {
    EXPECT_DOUBLE_EQ(
        Factorial_Node(std::make_unique<Number_Node>(0.0)).eval(), 1.0);
}

TEST(Ast_Factorial, Negative_Throws) {
    EXPECT_THROW(
        Factorial_Node(std::make_unique<Number_Node>(-1.0)).eval(),
        std::runtime_error);
}

TEST(Ast_Factorial, To_String) {
    EXPECT_EQ(
        Factorial_Node(std::make_unique<Number_Node>(5.0)).to_string(), "5!");
}

// ─── simplify() ──────────────────────────────────────────────────────────────

TEST(Ast_Simplify, Number_Returns_Clone) {
    Number_Node n(3.0);
    auto s = n.simplify();
    EXPECT_EQ(s->kind, Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 3.0);
}

TEST(Ast_Simplify, Constant_Stays_Symbolic) {
    Constant_Node c(Constant_Id::PI);
    auto s = c.simplify();
    EXPECT_EQ(s->kind, Node_Kind::CONSTANT);
    EXPECT_EQ(s->to_string(), "pi");
}

TEST(Ast_Simplify, Binary_Constant_Folds_Two_Numbers) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 7.0);
}

TEST(Ast_Simplify, Binary_Stays_Symbolic_With_Constant) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Constant_Node>(Constant_Id::PI));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::BINARY_OP);
    EXPECT_EQ(s->to_string(), "(2*pi)");
}

TEST(Ast_Simplify, Function_Folds_Numeric_Sqrt) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Number_Node>(9.0));
    auto n = std::make_unique<Function_Node>("sqrt", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 3.0);
}

TEST(Ast_Simplify, Function_Stays_Symbolic_With_Constant_Arg) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Constant_Node>(Constant_Id::PI));
    auto n = std::make_unique<Function_Node>("sin", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::FUNCTION);
    EXPECT_EQ(s->to_string(), "sin(pi)");
}

// ─── approx() ────────────────────────────────────────────────────────────────

TEST(Ast_Approx, Forces_Numeric_Eval_Of_Constant) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Constant_Node>(Constant_Id::PI));
    auto n = std::make_unique<Function_Node>("approx", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::NUMBER);
    EXPECT_NEAR(s->eval(), 3.14159265358979, 1e-10);
}

TEST(Ast_Approx, Forces_Numeric_Eval_Of_Symbolic_Expression) {
    std::vector<Node_Ptr> inner_args;
    inner_args.push_back(std::make_unique<Constant_Node>(Constant_Id::PI));
    auto sin_pi = std::make_unique<Function_Node>("sin", std::move(inner_args));

    std::vector<Node_Ptr> args;
    args.push_back(std::move(sin_pi));
    auto n = std::make_unique<Function_Node>("approx", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::NUMBER);
    EXPECT_NEAR(s->eval(), 0.0, 1e-10); // sin(π) ≈ 0
}

TEST(Ast_Approx, Two_Plus_Two_Stays_Four) {
    std::vector<Node_Ptr> args;
    args.push_back(std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(2.0)));
    auto n = std::make_unique<Function_Node>("approx", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind, Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 4.0);
}

// ─── clone() ─────────────────────────────────────────────────────────────────

TEST(Ast_Clone, Number_Clone_Is_Independent) {
    auto orig = std::make_unique<Number_Node>(99.0);
    auto copy = orig->clone();
    EXPECT_DOUBLE_EQ(copy->eval(), 99.0);
    EXPECT_NE(orig.get(), copy.get());
}

TEST(Ast_Clone, Binary_Deep_Clone) {
    auto orig = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(2.0));
    auto copy = orig->clone();
    EXPECT_DOUBLE_EQ(copy->eval(), 3.0);
    EXPECT_NE(orig.get(), copy.get());
}

TEST(Ast_Clone, Constant_Clone_Stays_Symbolic) {
    auto orig = std::make_unique<Constant_Node>(Constant_Id::E);
    auto copy = orig->clone();
    EXPECT_EQ(copy->to_string(), "e");
    EXPECT_EQ(copy->kind, Node_Kind::CONSTANT);
}
