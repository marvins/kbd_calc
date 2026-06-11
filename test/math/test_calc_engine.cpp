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

/************************************************************/
/*            Helper Function for "Pressing Keys"           */
/************************************************************/
static void press(math::Calc_Engine& eng, std::initializer_list<core::Action_Code> keys) {
    for (core::Action_Code k : keys)
        eng.handle_key(k);
}

/************************************************************/
/*                    Decimal Tests                         */
/************************************************************/
TEST(Calc_Engine_Decimal, Single_Decimal_Allowed) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_3 });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3");
}

TEST(Calc_Engine_Decimal, Second_Decimal_Ignored) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3");
}

TEST(Calc_Engine_Decimal, Third_Decimal_Also_Ignored) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_3,
                 core::Action_Code::DECIMAL, core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.33");
}

TEST(Calc_Engine_Decimal, New_Token_After_Operator_Allows_Decimal) {
    math::Calc_Engine eng;
    // 3.3 + 1.1 — each token may have its own decimal
    press(eng, { core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_3,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_1, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_1 });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3+1.1");
}

TEST(Calc_Engine_Decimal, Second_Decimal_After_Operator_Ignored) {
    math::Calc_Engine eng;
    // 3.3 + 1.1. — second dot in second token should be blocked
    press(eng, { core::Action_Code::DIGIT_3, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_3,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_1, core::Action_Code::DECIMAL, core::Action_Code::DIGIT_1,
                 core::Action_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "3.3+1.1");
}

TEST(Calc_Engine_Decimal, Leading_Decimal_Prepends_Zero) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression.eval_string(), "0.");
}

/************************************************************/
/*                    Evaluate Tests                        */
/************************************************************/
TEST(Calc_Engine_Evaluate, Two_Plus_Three) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::DIGIT_2, core::Action_Code::ADD, core::Action_Code::DIGIT_3,
                 core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "5");
}

TEST(Calc_Engine_Evaluate, Pi_Stays_Symbolic) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PI, core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "pi");
}

TEST(Calc_Engine_Evaluate, Two_Times_Pi_Stays_Symbolic) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::DIGIT_2, core::Action_Code::MULTIPLY, core::Action_Code::PI,
                 core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "(2*pi)");
}

TEST(Calc_Engine_Evaluate, Sqrt_Of_Four_Folds) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::SQRT, core::Action_Code::DIGIT_4, core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "2");
}

TEST(Calc_Engine_Evaluate, Sqrt_Of_Pi_Stays_Symbolic) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::SQRT, core::Action_Code::PI, core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "sqrt(pi)");
}

TEST(Calc_Engine_Evaluate, Sin_Of_Zero_Is_Zero) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::SIN, core::Action_Code::DIGIT_0, core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "0");
}

TEST(Calc_Engine_Evaluate, Sqrt_Of_Four_Divided_By_Three) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::SQRT, core::Action_Code::DIGIT_4, core::Action_Code::DIVIDE,
                 core::Action_Code::DIGIT_3, core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::EQUALS });
    // sqrt(4/3) evaluates numerically to approximately 1.1547
    ASSERT_FALSE(eng.state().history.empty());
    double result = std::stod(eng.state().history.front().result);
    EXPECT_NEAR(result, 1.1547005, 1e-6);
}

/************************************************************/
/*                  Parenthesis Tests                       */
/************************************************************/
TEST(Calc_Engine_Parenthesis, Open_Creates_Group_With_Placeholder) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN });
    EXPECT_EQ(eng.state().expression.eval_string(), "(0)");
}

TEST(Calc_Engine_Parenthesis, Group_Wraps_Expression) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_2,
                 core::Action_Code::PAREN_CLOSE });
    EXPECT_EQ(eng.state().expression.eval_string(), "(2)");
}

TEST(Calc_Engine_Parenthesis, Nested_Groups_Create_Nested_Parens) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_1,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_2,
                 core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::PAREN_CLOSE });
    // Nested groups: inner (1+2), outer wrapping it: ((1+2))
    EXPECT_EQ(eng.state().expression.eval_string(), "((1+2))");
}

TEST(Calc_Engine_Parenthesis, Expression_With_Group_Evaluates) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_2,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_3,
                 core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::MULTIPLY,
                 core::Action_Code::DIGIT_4,
                 core::Action_Code::EQUALS });
    // Result stored in history after evaluation
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "20");
}

/************************************************************/
/*                    Group Tests                           */
/************************************************************/
TEST(Calc_Engine_Group, Open_Group_Wraps_Placeholder) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN });
    EXPECT_EQ(eng.state().expression.eval_string(), "(0)");
}

TEST(Calc_Engine_Group, Open_Group_Then_Digit_Fills_Inner) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_4 });
    EXPECT_EQ(eng.state().expression.eval_string(), "(4)");
}

TEST(Calc_Engine_Group, Group_Then_Operator_Creates_Inside) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_4,
                 core::Action_Code::SUBTRACT });
    // Should be (4 - □) with cursor on right placeholder
    EXPECT_EQ(eng.state().expression.eval_string(), "(4-0)");
}

TEST(Calc_Engine_Group, Group_Complete_Expression) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_4,
                 core::Action_Code::SUBTRACT,
                 core::Action_Code::DIGIT_3,
                 core::Action_Code::PAREN_CLOSE });
    EXPECT_EQ(eng.state().expression.eval_string(), "(4-3)");
}

TEST(Calc_Engine_Group, Nested_Groups) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_1,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_2,
                 core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::MULTIPLY,
                 core::Action_Code::DIGIT_3,
                 core::Action_Code::PAREN_CLOSE });
    // Inner group (1+2), multiplied by 3 outside: ((1+2))*3
    EXPECT_EQ(eng.state().expression.eval_string(), "((1+2))*3");
}

TEST(Calc_Engine_Group, Group_With_External_Operator) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_2,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_3,
                 core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::MULTIPLY,
                 core::Action_Code::DIGIT_4 });
    // Group wraps (2+3), then multiplied by 4 outside: (2+3)*4
    EXPECT_EQ(eng.state().expression.eval_string(), "(2+3)*4");
}

TEST(Calc_Engine_Group, Group_Evaluates_Correctly) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::PAREN_OPEN,
                 core::Action_Code::DIGIT_2,
                 core::Action_Code::ADD,
                 core::Action_Code::DIGIT_3,
                 core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::MULTIPLY,
                 core::Action_Code::DIGIT_4,
                 core::Action_Code::EQUALS });
    // (2+3)*4 = 20, stored in history
    ASSERT_FALSE(eng.state().history.empty());
    EXPECT_EQ(eng.state().history.front().result, "20");
}

/************************************************************/
/*                    Approx Tests                          */
/************************************************************/
TEST(Calc_Engine_Approx, Wraps_Expression_And_Evaluates) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::APPROX,
                 core::Action_Code::PI,
                 core::Action_Code::PAREN_CLOSE,
                 core::Action_Code::EQUALS });
    ASSERT_FALSE(eng.state().history.empty());
    double result = std::stod(eng.state().history.front().result);
    EXPECT_NEAR(result, 3.14159265358979, 1e-6);
}

TEST(Calc_Engine_Approx, Token_Is_Atomic_Backspace) {
    math::Calc_Engine eng;
    press(eng, { core::Action_Code::APPROX });
    EXPECT_EQ(eng.state().expression.eval_string(), "approx(0)");
    eng.handle_key(core::Action_Code::BACKSPACE);
    EXPECT_TRUE(eng.state().expression.empty());
}
