#include <gtest/gtest.h>
#include "core/parser.hpp"
#include <cmath>

// Helper: parse expression string and return evaluated double
static double eval(const std::string& expr) {
    return Parser(expr).parse()->eval();
}

// Helper: parse expression string and return LaTeX string
static std::string to_latex(const std::string& expr) {
    return Parser(expr).parse()->to_latex();
}

// ─── Basic arithmetic ────────────────────────────────────────────────────────

TEST(Parser_Arithmetic, Addition) {
    EXPECT_DOUBLE_EQ(eval("3+4"), 7.0);
}

TEST(Parser_Arithmetic, Subtraction) {
    EXPECT_DOUBLE_EQ(eval("10-3"), 7.0);
}

TEST(Parser_Arithmetic, Multiplication) {
    EXPECT_DOUBLE_EQ(eval("6*7"), 42.0);
}

TEST(Parser_Arithmetic, Division) {
    EXPECT_DOUBLE_EQ(eval("10/4"), 2.5);
}

TEST(Parser_Arithmetic, Modulo) {
    EXPECT_DOUBLE_EQ(eval("10%3"), 1.0);
}

TEST(Parser_Arithmetic, Power) {
    EXPECT_DOUBLE_EQ(eval("2^10"), 1024.0);
}

TEST(Parser_Arithmetic, Power_Right_Assoc) {
    EXPECT_DOUBLE_EQ(eval("2^3^2"), std::pow(2.0, std::pow(3.0, 2.0)));
}

// ─── Unary ───────────────────────────────────────────────────────────────────

TEST(Parser_Unary, Negate) {
    EXPECT_DOUBLE_EQ(eval("-5"), -5.0);
}

TEST(Parser_Unary, Double_Negate) {
    EXPECT_DOUBLE_EQ(eval("--5"), 5.0);
}

TEST(Parser_Unary, Unary_Plus) {
    EXPECT_DOUBLE_EQ(eval("+5"), 5.0);
}

// ─── Precedence ──────────────────────────────────────────────────────────────

TEST(Parser_Precedence, Mul_Before_Add) {
    EXPECT_DOUBLE_EQ(eval("2+3*4"), 14.0);
}

TEST(Parser_Precedence, Parens_Override) {
    EXPECT_DOUBLE_EQ(eval("(2+3)*4"), 20.0);
}

TEST(Parser_Precedence, Nested_Parens) {
    EXPECT_DOUBLE_EQ(eval("((2+3))*4"), 20.0);
}

TEST(Parser_Precedence, Power_Before_Mul) {
    EXPECT_DOUBLE_EQ(eval("2*3^2"), 18.0);
}

// ─── Constants ───────────────────────────────────────────────────────────────

TEST(Parser_Constants, Pi) {
    EXPECT_NEAR(eval("pi"), M_PI, 1e-10);
}

TEST(Parser_Constants, E) {
    EXPECT_NEAR(eval("e"), M_E, 1e-10);
}

TEST(Parser_Constants, Pi_In_Expression) {
    EXPECT_NEAR(eval("2*pi"), 2.0 * M_PI, 1e-10);
}

TEST(Parser_Constants, Implicit_Multiply_2pi) {
    EXPECT_NEAR(eval("2pi"), 2.0 * M_PI, 1e-10);
}

// ─── Functions ───────────────────────────────────────────────────────────────

TEST(Parser_Functions, Sin_Zero) {
    EXPECT_DOUBLE_EQ(eval("sin(0)"), 0.0);
}

TEST(Parser_Functions, Cos_Zero) {
    EXPECT_DOUBLE_EQ(eval("cos(0)"), 1.0);
}

TEST(Parser_Functions, Sqrt_Nine) {
    EXPECT_DOUBLE_EQ(eval("sqrt(9)"), 3.0);
}

TEST(Parser_Functions, Log_Hundred) {
    EXPECT_DOUBLE_EQ(eval("log(100)"), 2.0);
}

TEST(Parser_Functions, Ln_One) {
    EXPECT_DOUBLE_EQ(eval("ln(1)"), 0.0);
}

TEST(Parser_Functions, Nested_Function) {
    EXPECT_NEAR(eval("sqrt(sin(0)+cos(0))"), 1.0, 1e-10);
}

TEST(Parser_Functions, Exp) {
    EXPECT_NEAR(eval("exp(1)"), M_E, 1e-10);
}

// ─── Factorial ───────────────────────────────────────────────────────────────

TEST(Parser_Factorial, Basic) {
    EXPECT_DOUBLE_EQ(eval("5!"), 120.0);
}

TEST(Parser_Factorial, In_Expression) {
    EXPECT_DOUBLE_EQ(eval("3!+1"), 7.0);
}

// ─── Bitwise / Programmer ────────────────────────────────────────────────────

TEST(Parser_Bitwise, And) {
    EXPECT_DOUBLE_EQ(eval("12&10"), 8.0);
}

TEST(Parser_Bitwise, Or) {
    EXPECT_DOUBLE_EQ(eval("12|10"), 14.0);
}

TEST(Parser_Bitwise, Shift_Left) {
    EXPECT_DOUBLE_EQ(eval("1<<4"), 16.0);
}

TEST(Parser_Bitwise, Shift_Right) {
    EXPECT_DOUBLE_EQ(eval("16>>2"), 4.0);
}

TEST(Parser_Bitwise, Hex_Literal) {
    EXPECT_DOUBLE_EQ(eval("0xFF"), 255.0);
}

// ─── Whitespace handling ─────────────────────────────────────────────────────

TEST(Parser_Whitespace, Spaces_Ignored) {
    EXPECT_DOUBLE_EQ(eval("3 + 4"), 7.0);
}

TEST(Parser_Whitespace, Multiple_Spaces) {
    EXPECT_DOUBLE_EQ(eval("  2  *  3  "), 6.0);
}

// ─── Error cases ─────────────────────────────────────────────────────────────

TEST(Parser_Errors, Empty_Throws) {
    EXPECT_THROW(Parser("").parse(), std::exception);
}

TEST(Parser_Errors, Trailing_Junk_Throws) {
    EXPECT_THROW(eval("3+4 5"), std::runtime_error);
}

TEST(Parser_Errors, Unknown_Function_Throws) {
    EXPECT_THROW(eval("foo(1)"), std::runtime_error);
}

TEST(Parser_Errors, Division_By_Zero_Throws) {
    EXPECT_THROW(eval("1/0"), std::runtime_error);
}

// ─── LaTeX output ────────────────────────────────────────────────────────────

TEST(Parser_Latex, Simple_Fraction) {
    EXPECT_EQ(to_latex("1/2"), "\\frac{1}{2}");
}

TEST(Parser_Latex, Power) {
    EXPECT_EQ(to_latex("2^8"), "2^{8}");
}

TEST(Parser_Latex, Sqrt) {
    EXPECT_EQ(to_latex("sqrt(2)"), "\\sqrt{2}");
}

TEST(Parser_Latex, Pi_Constant) {
    EXPECT_EQ(to_latex("pi"), "\\pi");
}

TEST(Parser_Latex, Multiply) {
    EXPECT_EQ(to_latex("3*4"), "3 \\cdot 4");
}
