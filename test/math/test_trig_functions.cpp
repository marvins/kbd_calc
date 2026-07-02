/**
 * @file    test_trig_functions.cpp
 * @author  Marvin Smith
 * @date    2026-06-30
 * @brief   Unit tests for trigonometric functions
 */

// C++ Standard Libraries
#include <cmath>
#include <numbers>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/expression.hpp>
#include <overboard/math/parser.hpp>

using namespace ovb;
using AC = core::Action_Code;

/****************************/
/*        Helpers           */
/****************************/
static void press(math::Expression& e, std::initializer_list<AC> keys) {
    for (AC k : keys) {
        e.insert(k);
    }
}

static double eval(math::Expression& e) {
    // Use the AST directly instead of reparsing
    if (!e.ast_root()) return std::numeric_limits<double>::quiet_NaN();
    return e.ast_root()->eval();
}

constexpr double PI = std::numbers::pi;
constexpr double EPSILON = 1e-10;

/****************************/
/*     Basic Trig Tests     */
/****************************/

TEST(TrigFunctions, sin_of_zero) {
    math::Expression e;
    press(e, {AC::SIN, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, sin_of_pi_over_2) {
    math::Expression e;
    press(e, {AC::SIN, AC::PI, AC::DIVIDE, AC::DIGIT_2});
    EXPECT_NEAR(eval(e), 1.0, EPSILON);
}

TEST(TrigFunctions, cos_of_zero) {
    math::Expression e;
    press(e, {AC::COS, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 1.0, EPSILON);
}

TEST(TrigFunctions, cos_of_pi) {
    math::Expression e;
    press(e, {AC::COS, AC::PI});
    EXPECT_NEAR(eval(e), -1.0, EPSILON);
}

TEST(TrigFunctions, tan_of_zero) {
    math::Expression e;
    press(e, {AC::TAN, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, tan_of_pi_over_4) {
    math::Expression e;
    press(e, {AC::TAN, AC::PI, AC::DIVIDE, AC::DIGIT_4});
    EXPECT_NEAR(eval(e), 1.0, EPSILON);
}

/****************************/
/*     Cotangent Tests      */
/****************************/

TEST(TrigFunctions, cot_of_pi_over_4) {
    math::Expression e;
    press(e, {AC::COT, AC::PI, AC::DIVIDE, AC::DIGIT_4});
    EXPECT_NEAR(eval(e), 1.0, EPSILON);
}

TEST(TrigFunctions, cot_of_pi_over_2) {
    math::Expression e;
    press(e, {AC::COT, AC::PI, AC::DIVIDE, AC::DIGIT_2});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

/****************************/
/*   Inverse Trig Tests     */
/****************************/

TEST(TrigFunctions, asin_of_zero) {
    math::Expression e;
    press(e, {AC::ASIN, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, asin_of_one) {
    math::Expression e;
    press(e, {AC::ASIN, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), PI / 2.0, EPSILON);
}

TEST(TrigFunctions, acos_of_zero) {
    math::Expression e;
    press(e, {AC::ACOS, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), PI / 2.0, EPSILON);
}

TEST(TrigFunctions, acos_of_one) {
    math::Expression e;
    press(e, {AC::ACOS, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, atan_of_zero) {
    math::Expression e;
    press(e, {AC::ATAN, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, atan_of_one) {
    math::Expression e;
    press(e, {AC::ATAN, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), PI / 4.0, EPSILON);
}

TEST(TrigFunctions, acot_of_one) {
    math::Expression e;
    press(e, {AC::ACOT, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), PI / 4.0, EPSILON);
}

/****************************/
/*     Atan2 Tests          */
/****************************/

TEST(TrigFunctions, atan2_positive_quadrant) {
    // atan2(1, 1) should be pi/4 (45 degrees, first quadrant)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_1});
    EXPECT_NEAR(eval(e), PI / 4.0, EPSILON);
}

TEST(TrigFunctions, atan2_second_quadrant) {
    // atan2(1, -1) should be 3*pi/4 (135 degrees, second quadrant)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_0, AC::SUBTRACT, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 3.0 * PI / 4.0, EPSILON);
}

TEST(TrigFunctions, atan2_third_quadrant) {
    // atan2(-1, -1) should be -3*pi/4 (-135 degrees, third quadrant)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_0, AC::SUBTRACT, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_0, AC::SUBTRACT, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), -3.0 * PI / 4.0, EPSILON);
}

TEST(TrigFunctions, atan2_fourth_quadrant) {
    // atan2(-1, 1) should be -pi/4 (-45 degrees, fourth quadrant)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_0, AC::SUBTRACT, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_1});
    EXPECT_NEAR(eval(e), -PI / 4.0, EPSILON);
}

TEST(TrigFunctions, atan2_positive_x_axis) {
    // atan2(0, 1) should be 0 (positive x-axis)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_0});
    e.cursor_right();
    press(e, {AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, atan2_positive_y_axis) {
    // atan2(1, 0) should be pi/2 (positive y-axis)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_0});
    EXPECT_NEAR(eval(e), PI / 2.0, EPSILON);
}

TEST(TrigFunctions, atan2_negative_x_axis) {
    // atan2(0, -1) should be pi (negative x-axis)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_0});
    e.cursor_right();
    press(e, {AC::DIGIT_0, AC::SUBTRACT, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), PI, EPSILON);
}

TEST(TrigFunctions, atan2_negative_y_axis) {
    // atan2(-1, 0) should be -pi/2 (negative y-axis)
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_0, AC::SUBTRACT, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_0});
    EXPECT_NEAR(eval(e), -PI / 2.0, EPSILON);
}

/****************************/
/*  String Representation   */
/****************************/

TEST(TrigFunctions, sin_renders_correctly) {
    math::Expression e;
    press(e, {AC::SIN, AC::PI});
    EXPECT_EQ(e.render_string(), "sin(pi)");
}

TEST(TrigFunctions, cos_renders_correctly) {
    math::Expression e;
    press(e, {AC::COS, AC::PI});
    EXPECT_EQ(e.render_string(), "cos(pi)");
}

TEST(TrigFunctions, tan_renders_correctly) {
    math::Expression e;
    press(e, {AC::TAN, AC::DIGIT_0});
    EXPECT_EQ(e.render_string(), "tan(0)");
}

TEST(TrigFunctions, cot_renders_correctly) {
    math::Expression e;
    press(e, {AC::COT, AC::PI});
    EXPECT_EQ(e.render_string(), "cot(pi)");
}

TEST(TrigFunctions, sec_renders_correctly) {
    math::Expression e;
    press(e, {AC::SEC, AC::DIGIT_0});
    EXPECT_EQ(e.render_string(), "sec(0)");
}

TEST(TrigFunctions, csc_renders_correctly) {
    math::Expression e;
    press(e, {AC::CSC, AC::PI});
    EXPECT_EQ(e.render_string(), "csc(pi)");
}

TEST(TrigFunctions, asin_renders_correctly) {
    math::Expression e;
    press(e, {AC::ASIN, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "asin(1)");
}

TEST(TrigFunctions, acos_renders_correctly) {
    math::Expression e;
    press(e, {AC::ACOS, AC::DIGIT_0});
    EXPECT_EQ(e.render_string(), "acos(0)");
}

TEST(TrigFunctions, atan_renders_correctly) {
    math::Expression e;
    press(e, {AC::ATAN, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "atan(1)");
}

TEST(TrigFunctions, acot_renders_correctly) {
    math::Expression e;
    press(e, {AC::ACOT, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "acot(1)");
}

TEST(TrigFunctions, atan2_renders_correctly) {
    math::Expression e;
    press(e, {AC::ATAN2, AC::DIGIT_1});
    e.cursor_right();
    press(e, {AC::DIGIT_2});
    EXPECT_EQ(e.render_string(), "atan2(1,2)");
}

/****************************/
/*   Hyperbolic Functions   */
/****************************/

TEST(TrigFunctions, sinh_of_zero) {
    // sinh(0) = 0
    math::Expression e;
    press(e, {AC::SINH, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, sinh_of_one) {
    // sinh(1) ≈ 1.1752
    math::Expression e;
    press(e, {AC::SINH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), std::sinh(1.0), EPSILON);
}

TEST(TrigFunctions, cosh_of_zero) {
    // cosh(0) = 1
    math::Expression e;
    press(e, {AC::COSH, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 1.0, EPSILON);
}

TEST(TrigFunctions, cosh_of_one) {
    // cosh(1) ≈ 1.5431
    math::Expression e;
    press(e, {AC::COSH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), std::cosh(1.0), EPSILON);
}

TEST(TrigFunctions, tanh_of_zero) {
    // tanh(0) = 0
    math::Expression e;
    press(e, {AC::TANH, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, tanh_of_one) {
    // tanh(1) ≈ 0.7616
    math::Expression e;
    press(e, {AC::TANH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), std::tanh(1.0), EPSILON);
}

TEST(TrigFunctions, coth_of_one) {
    // coth(1) = 1/tanh(1) ≈ 1.3130
    math::Expression e;
    press(e, {AC::COTH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 1.0 / std::tanh(1.0), EPSILON);
}

TEST(TrigFunctions, sech_of_zero) {
    // sech(0) = 1/cosh(0) = 1
    math::Expression e;
    press(e, {AC::SECH, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 1.0, EPSILON);
}

TEST(TrigFunctions, sech_of_one) {
    // sech(1) = 1/cosh(1) ≈ 0.6481
    math::Expression e;
    press(e, {AC::SECH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 1.0 / std::cosh(1.0), EPSILON);
}

TEST(TrigFunctions, csch_of_one) {
    // csch(1) = 1/sinh(1) ≈ 0.8509
    math::Expression e;
    press(e, {AC::CSCH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 1.0 / std::sinh(1.0), EPSILON);
}

TEST(TrigFunctions, asinh_of_zero) {
    // asinh(0) = 0
    math::Expression e;
    press(e, {AC::ASINH, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, asinh_of_one) {
    // asinh(1) ≈ 0.8814
    math::Expression e;
    press(e, {AC::ASINH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), std::asinh(1.0), EPSILON);
}

TEST(TrigFunctions, acosh_of_one) {
    // acosh(1) = 0
    math::Expression e;
    press(e, {AC::ACOSH, AC::DIGIT_1});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, acosh_of_two) {
    // acosh(2) ≈ 1.3170
    math::Expression e;
    press(e, {AC::ACOSH, AC::DIGIT_2});
    EXPECT_NEAR(eval(e), std::acosh(2.0), EPSILON);
}

TEST(TrigFunctions, atanh_of_zero) {
    // atanh(0) = 0
    math::Expression e;
    press(e, {AC::ATANH, AC::DIGIT_0});
    EXPECT_NEAR(eval(e), 0.0, EPSILON);
}

TEST(TrigFunctions, atanh_of_half) {
    // atanh(0.5) ≈ 0.5493
    math::Expression e;
    press(e, {AC::ATANH, AC::DIGIT_0, AC::DECIMAL, AC::DIGIT_5});
    EXPECT_NEAR(eval(e), std::atanh(0.5), EPSILON);
}

TEST(TrigFunctions, hyperbolic_identity_cosh_squared_minus_sinh_squared) {
    // cosh²(x) - sinh²(x) = 1
    math::Expression e_cosh, e_sinh;
    press(e_cosh, {AC::COSH, AC::DIGIT_2});
    press(e_sinh, {AC::SINH, AC::DIGIT_2});
    double cosh_val = eval(e_cosh);
    double sinh_val = eval(e_sinh);
    EXPECT_NEAR(cosh_val * cosh_val - sinh_val * sinh_val, 1.0, EPSILON);
}

/****************************/
/*  Hyperbolic Rendering    */
/****************************/

TEST(TrigFunctions, sinh_renders_correctly) {
    math::Expression e;
    press(e, {AC::SINH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "sinh(1)");
}

TEST(TrigFunctions, cosh_renders_correctly) {
    math::Expression e;
    press(e, {AC::COSH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "cosh(1)");
}

TEST(TrigFunctions, tanh_renders_correctly) {
    math::Expression e;
    press(e, {AC::TANH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "tanh(1)");
}

TEST(TrigFunctions, asinh_renders_correctly) {
    math::Expression e;
    press(e, {AC::ASINH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "asinh(1)");
}

TEST(TrigFunctions, acosh_renders_correctly) {
    math::Expression e;
    press(e, {AC::ACOSH, AC::DIGIT_2});
    EXPECT_EQ(e.render_string(), "acosh(2)");
}

TEST(TrigFunctions, atanh_renders_correctly) {
    math::Expression e;
    press(e, {AC::ATANH, AC::DIGIT_0, AC::DECIMAL, AC::DIGIT_5});
    EXPECT_EQ(e.render_string(), "atanh(0.5)");
}

TEST(TrigFunctions, coth_renders_correctly) {
    math::Expression e;
    press(e, {AC::COTH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "coth(1)");
}

TEST(TrigFunctions, sech_renders_correctly) {
    math::Expression e;
    press(e, {AC::SECH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "sech(1)");
}

TEST(TrigFunctions, csch_renders_correctly) {
    math::Expression e;
    press(e, {AC::CSCH, AC::DIGIT_1});
    EXPECT_EQ(e.render_string(), "csch(1)");
}
