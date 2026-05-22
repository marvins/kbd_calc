/**
 * @file    test_calc_engine.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Unit tests for the Calc_Engine class.
 */

// C++ Standard Libraries
#include <cmath>
#include <string>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/calc_engine.hpp>

using namespace ovb;

static void press(math::Calc_Engine& eng, std::initializer_list<Key_Code> keys) {
    for (Key_Code k : keys)
        eng.handle_key(k);
}

// ─── Decimal guard ───────────────────────────────────────────────────────────

TEST(Calc_Engine_Decimal, Single_Decimal_Allowed) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3 });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3");
}

TEST(Calc_Engine_Decimal, Second_Decimal_Ignored) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3, Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3");
}

TEST(Calc_Engine_Decimal, Third_Decimal_Also_Ignored) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3,
                 Key_Code::DECIMAL, Key_Code::DIGIT_3, Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.33");
}

TEST(Calc_Engine_Decimal, New_Token_After_Operator_Allows_Decimal) {
    math::Calc_Engine eng;
    // 3.3 + 1.1 — each token may have its own decimal
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3,
                 Key_Code::ADD,
                 Key_Code::DIGIT_1, Key_Code::DECIMAL, Key_Code::DIGIT_1 });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3+1.1");
}

TEST(Calc_Engine_Decimal, Second_Decimal_After_Operator_Ignored) {
    math::Calc_Engine eng;
    // 3.3 + 1.1. — second dot in second token should be blocked
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3,
                 Key_Code::ADD,
                 Key_Code::DIGIT_1, Key_Code::DECIMAL, Key_Code::DIGIT_1,
                 Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3+1.1");
}

TEST(Calc_Engine_Decimal, Leading_Decimal_Prepends_Zero) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "0.");
}

// ─── evaluate() — symbolic simplify ──────────────────────────────────────────

TEST(Calc_Engine_Evaluate, Two_Plus_Three) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_2, Key_Code::ADD, Key_Code::DIGIT_3,
                 Key_Code::EQUALS });
    EXPECT_EQ(eng.state().display_value, "5");
}

TEST(Calc_Engine_Evaluate, Pi_Stays_Symbolic) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::PI, Key_Code::EQUALS });
    EXPECT_EQ(eng.state().display_value, "pi");
}

TEST(Calc_Engine_Evaluate, Two_Times_Pi_Stays_Symbolic) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_2, Key_Code::MULTIPLY, Key_Code::PI,
                 Key_Code::EQUALS });
    EXPECT_EQ(eng.state().display_value, "(2*pi)");
}

TEST(Calc_Engine_Evaluate, Sqrt_Of_Four_Folds) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::SQRT, Key_Code::DIGIT_4, Key_Code::PAREN_CLOSE,
                 Key_Code::EQUALS });
    EXPECT_EQ(eng.state().display_value, "2");
}

TEST(Calc_Engine_Evaluate, Sqrt_Of_Pi_Stays_Symbolic) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::SQRT, Key_Code::PI, Key_Code::PAREN_CLOSE,
                 Key_Code::EQUALS });
    EXPECT_EQ(eng.state().display_value, "sqrt(pi)");
}

// ─── approx() — force numeric ────────────────────────────────────────────────

TEST(Calc_Engine_Approx, Wraps_Expression_And_Evaluates) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::APPROX,
                 Key_Code::PI,
                 Key_Code::PAREN_CLOSE,
                 Key_Code::EQUALS });
    double result = std::stod(eng.state().display_value);
    EXPECT_NEAR(result, 3.14159265358979, 1e-6);
}

TEST(Calc_Engine_Approx, Token_Is_Atomic_Backspace) {
    math::Calc_Engine eng;
    press(eng, { Key_Code::APPROX });
    EXPECT_EQ(eng.state().expression.eval_string(), "approx(");
    eng.handle_key(Key_Code::BACKSPACE);
    EXPECT_TRUE(eng.state().expression.empty());
}
