/**
 * @file    test_expression.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Unit tests for the Expression class (tree-based AST).
 */

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/expression.hpp>

using namespace ovb;

using KC = core::Key_Code;

/****************************/
/*        Helpers           */
/****************************/
static void press( math::Expression& e,
                   std::initializer_list<KC> keys ) {
    for (KC k : keys)
        e.insert( k);
}

/****************************/
/*     Number building      */
/****************************/

/*************************************************************/
/*   Consecutive digits form a single number node           */
/*************************************************************/
TEST(Expression, number_digits_build_single_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "123");
}

/*************************************************************/
/*   Decimal point is handled by double value                */
/*************************************************************/
TEST(Expression, number_with_decimal) {
    math::Expression e;
    press( e, { KC::DIGIT_3, KC::DECIMAL, KC::DIGIT_1 });
    EXPECT_EQ(e.eval_string(), "3.1");
}

/*************************************************************/
/*   Leading decimal normalizes to 0.x format                */
/*************************************************************/
TEST(Expression, number_leading_decimal_starts_number) {
    math::Expression e;
    press( e, { KC::DECIMAL, KC::DIGIT_5 });
    EXPECT_EQ(e.eval_string(), "0.5");
}

/*************************************************************/
/*   Operator creates separate number nodes                   */
/*************************************************************/
TEST(Expression, number_two_numbers_separated_by_operator) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2 });
    EXPECT_EQ(e.eval_string(), "1+2");
}

/*************************************************************/
/*   Second number can have its own decimal point            */
/*************************************************************/
TEST(Expression, number_decimal_allowed_in_second_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2, KC::DECIMAL, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "1+2.3");
}

/****************************/
/*  Backspace node deletion */
/****************************/

/*************************************************************/
/*   Function nodes are deleted as a whole unit             */
/*************************************************************/
TEST(Expression, backspace_deletes_function_atomically) {
    math::Expression e;
    press( e, { KC::SIN });
    e.backspace( );
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
}

/*************************************************************/
/*   Constant nodes are deleted as a whole unit             */
/*************************************************************/
TEST(Expression, backspace_deletes_constant_atomically) {
    math::Expression e;
    press( e, { KC::PI });
    e.backspace( );
    EXPECT_TRUE(e.empty());
}

/*************************************************************/
/*   Backspace on single-digit number removes entire node   */
/*************************************************************/
TEST(Expression, backspace_removes_single_digit_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1 });
    e.backspace( );
    EXPECT_TRUE(e.empty());
}

/*************************************************************/
/*   Backspace at empty expression is no-op                  */
/*************************************************************/
TEST(Expression, backspace_does_nothing_at_start) {
    math::Expression e;
    e.backspace( );
    EXPECT_TRUE(e.empty());
}

/****************************/
/*      Cursor movement     */
/****************************/

/*************************************************************/
/*   Cursor position equals glyph count at end               */
/*************************************************************/
TEST(Expression, cursor_glyph_pos_at_end) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2 });
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
}

/*************************************************************/
/*   Cursor right at end of expression is no-op              */
/*************************************************************/
TEST(Expression, cursor_right_does_nothing_at_end) {
    math::Expression e;
    press( e, { KC::DIGIT_1 });
    std::size_t pos = e.cursor_glyph_pos();
    e.cursor_right( );
    EXPECT_EQ(e.cursor_glyph_pos(), pos);
}

/****************************/
/*     Render/eval string   */
/****************************/

/*************************************************************/
/*   Functions render with parentheses and args              */
/*************************************************************/
TEST(Expression, render_sin_of_pi) {
    math::Expression e;
    press( e, { KC::SIN, KC::PI });
    EXPECT_EQ(e.eval_string(), "sin(pi)");
}

/*************************************************************/
/*   sqrt() renders correctly with argument                  */
/*************************************************************/
TEST(Expression, render_sqrt_of_number) {
    math::Expression e;
    press( e, { KC::SQRT, KC::DIGIT_4 });
    EXPECT_EQ(e.eval_string(), "sqrt(4)");
}

/*************************************************************/
/*   Power operator renders as ^                             */
/*************************************************************/
TEST(Expression, render_power_2) {
    math::Expression e;
    press( e, { KC::DIGIT_3, KC::POWER_2 });
    EXPECT_EQ(e.eval_string(), "3^2");
}

/****************************/
/*          Clear           */
/****************************/

/*************************************************************/
/*    Clear removes all nodes and resets cursor              */
/*************************************************************/
TEST(Expression, clear_clears_all) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::ADD, KC::SIN });
    e.clear( );
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
    EXPECT_EQ(e.cursor_glyph_pos(), 0);
}

/****************************/
/*     Placeholder detection   */
/****************************/

/*************************************************************/
/*   Empty expression has placeholder                      */
/*************************************************************/
TEST(Expression, empty_expression_has_placeholder) {
    math::Expression e;
    EXPECT_TRUE(e.has_placeholder());
}

/*************************************************************/
/*   Number expression has no placeholder                   */
/*************************************************************/
TEST(Expression, number_expression_no_placeholder) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2 });
    EXPECT_FALSE(e.has_placeholder());
}

/*************************************************************/
/*   Function with argument has no placeholder              */
/*************************************************************/
TEST(Expression, function_with_arg_no_placeholder) {
    math::Expression e;
    press( e, { KC::SIN, KC::DIGIT_4 });
    EXPECT_FALSE(e.has_placeholder());
}

/*************************************************************/
/*   Function without argument has placeholder              */
/*************************************************************/
TEST(Expression, function_without_arg_has_placeholder) {
    math::Expression e;
    press( e, { KC::SIN });
    EXPECT_TRUE(e.has_placeholder());
}

/****************************/
/*     Binary operators    */
/****************************/

/*************************************************************/
/*   Basic addition works                                   */
/*************************************************************/
TEST(Expression, basic_addition) {
    math::Expression e;
    press( e, { KC::DIGIT_4, KC::ADD, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4+3");
}

/*************************************************************/
/*   Basic subtraction works                                */
/*************************************************************/
TEST(Expression, basic_subtraction) {
    math::Expression e;
    press( e, { KC::DIGIT_4, KC::SUBTRACT, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4-3");
}

/*************************************************************/
/*   Basic multiplication works                             */
/*************************************************************/
TEST(Expression, basic_multiplication) {
    math::Expression e;
    press( e, { KC::DIGIT_4, KC::MULTIPLY, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4*3");
}

/*************************************************************/
/*   Basic division works                                   */
/*************************************************************/
TEST(Expression, basic_division) {
    math::Expression e;
    press( e, { KC::DIGIT_4, KC::DIVIDE, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4/3");
}

/****************************/
/*     Special operators   */
/****************************/

/*************************************************************/
/*   Factorial operator works                               */
/*************************************************************/
TEST(Expression, factorial_operator) {
    math::Expression e;
    press( e, { KC::DIGIT_5, KC::FACTORIAL });
    EXPECT_EQ(e.eval_string(), "5!");
}

/*************************************************************/
/*   Reciprocal operator works                              */
/*************************************************************/
TEST(Expression, reciprocal_operator) {
    math::Expression e;
    press( e, { KC::DIGIT_2, KC::RECIPROCAL });
    EXPECT_EQ(e.eval_string(), "1/2");
}

/****************************/
/*     Constants           */
/****************************/

/*************************************************************/
/*   Pi constant works                                      */
/*************************************************************/
TEST(Expression, pi_constant) {
    math::Expression e;
    press( e, { KC::PI });
    EXPECT_EQ(e.eval_string(), "pi");
}

/****************************/
/*     Nested expressions  */
/****************************/

/*************************************************************/
/*   Nested function with operator                         */
/*************************************************************/
TEST(Expression, nested_function_with_operator) {
    math::Expression e;
    press( e, { KC::SQRT, KC::DIGIT_4, KC::ADD, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "sqrt(4+3)");
}

