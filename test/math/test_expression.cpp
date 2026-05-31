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

using AC = core::Action_Code;

/****************************/
/*        Helpers           */
/****************************/
static void press( math::Expression& e,
                   std::initializer_list<AC> keys ) {
    for (AC k : keys)
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
    press( e, { AC::DIGIT_1, AC::DIGIT_2, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "123");
}

/*************************************************************/
/*   Decimal point is handled by double value                */
/*************************************************************/
TEST(Expression, number_with_decimal) {
    math::Expression e;
    press( e, { AC::DIGIT_3, AC::DECIMAL, AC::DIGIT_1 });
    EXPECT_EQ(e.eval_string(), "3.1");
}

/*************************************************************/
/*   Leading decimal normalizes to 0.x format                */
/*************************************************************/
TEST(Expression, number_leading_decimal_starts_number) {
    math::Expression e;
    press( e, { AC::DECIMAL, AC::DIGIT_5 });
    EXPECT_EQ(e.eval_string(), "0.5");
}

/*************************************************************/
/*   Operator creates separate number nodes                   */
/*************************************************************/
TEST(Expression, number_two_numbers_separated_by_operator) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::ADD, AC::DIGIT_2 });
    EXPECT_EQ(e.eval_string(), "1+2");
}

/*************************************************************/
/*   Second number can have its own decimal point            */
/*************************************************************/
TEST(Expression, number_decimal_allowed_in_second_number) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::ADD, AC::DIGIT_2, AC::DECIMAL, AC::DIGIT_3 });
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
    press( e, { AC::SIN });
    e.backspace( );
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
}

/*************************************************************/
/*   Constant nodes are deleted as a whole unit             */
/*************************************************************/
TEST(Expression, backspace_deletes_constant_atomically) {
    math::Expression e;
    press( e, { AC::PI });
    e.backspace( );
    EXPECT_TRUE(e.empty());
}

/*************************************************************/
/*   Backspace on single-digit number removes entire node   */
/*************************************************************/
TEST(Expression, backspace_removes_single_digit_number) {
    math::Expression e;
    press( e, { AC::DIGIT_1 });
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

/*************************************************************/
/*   Backspace removes last digit from multi-digit number    */
/*************************************************************/
TEST(Expression, backspace_removes_last_digit) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::DIGIT_2, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "123");
    e.backspace( );
    EXPECT_EQ(e.eval_string(), "12");
}

/*************************************************************/
/*   Backspace removes operator and joins numbers              */
/*************************************************************/
TEST(Expression, backspace_removes_operator) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::ADD, AC::DIGIT_2 });
    EXPECT_EQ(e.eval_string(), "1+2");
    e.backspace( );
    // After backspace, the 2 is removed
    EXPECT_EQ(e.eval_string(), "1");
}

/*************************************************************/
/*   Backspace in middle of expression via cursor              */
/*************************************************************/
TEST(Expression, backspace_after_moving_cursor) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::ADD, AC::DIGIT_2 });
    // Move cursor left to position after "1+"
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
    // Backspace should remove the "+" operator
    e.backspace( );
    EXPECT_EQ(e.eval_string(), "1");
}

/****************************/
/*      Cursor movement     */
/****************************/

/*************************************************************/
/*   Cursor position equals glyph count at end               */
/*************************************************************/
TEST(Expression, cursor_glyph_pos_at_end) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::DIGIT_2 });
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
}

/*************************************************************/
/*   Cursor right at end of expression is no-op              */
/*************************************************************/
TEST(Expression, cursor_right_does_nothing_at_end) {
    math::Expression e;
    press( e, { AC::DIGIT_1 });
    std::size_t pos = e.cursor_glyph_pos();
    e.cursor_right( );
    EXPECT_EQ(e.cursor_glyph_pos(), pos);
}

/*************************************************************/
/*   Cursor left moves to previous position                  */
/*************************************************************/
TEST(Expression, cursor_left_moves_back) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::DIGIT_2 });
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 1);
}

/*************************************************************/
/*   Cursor left at start is no-op                           */
/*************************************************************/
TEST(Expression, cursor_left_does_nothing_at_start) {
    math::Expression e;
    press( e, { AC::DIGIT_1 });
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 0);
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 0);
}

/*************************************************************/
/*   Cursor navigation through operators                     */
/*************************************************************/
TEST(Expression, cursor_navigation_through_expression) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::ADD, AC::DIGIT_2 });
    EXPECT_EQ(e.cursor_glyph_pos(), 3);  // "1+2"

    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 2);

    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 1);

    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 0);

    e.cursor_right( );
    EXPECT_EQ(e.cursor_glyph_pos(), 1);

    e.cursor_right( );
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
}

/*************************************************************/
/*   Cursor navigation in function with argument             */
/*************************************************************/
TEST(Expression, cursor_navigation_in_function) {
    math::Expression e;
    press( e, { AC::SQRT, AC::DIGIT_4 });
    // "sqrt(4)" = 7 glyphs
    EXPECT_EQ(e.cursor_glyph_pos(), 7);

    // Move cursor left through the expression
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 6);

    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 5);
}

/****************************/
/*     Render/eval string   */
/****************************/

/*************************************************************/
/*   Functions render with parentheses and args              */
/*************************************************************/
TEST(Expression, render_sin_of_pi) {
    math::Expression e;
    press( e, { AC::SIN, AC::PI });
    EXPECT_EQ(e.eval_string(), "sin(pi)");
}

/*************************************************************/
/*   sqrt() renders correctly with argument                  */
/*************************************************************/
TEST(Expression, render_sqrt_of_number) {
    math::Expression e;
    press( e, { AC::SQRT, AC::DIGIT_4 });
    EXPECT_EQ(e.eval_string(), "sqrt(4)");
}

/*************************************************************/
/*   sin() renders correctly with argument                   */
/*************************************************************/
TEST(Expression, render_sin_of_zero) {
    math::Expression e;
    press( e, { AC::SIN, AC::DIGIT_0 });
    EXPECT_EQ(e.eval_string(), "sin(0)");
}

/*************************************************************/
/*   Power operator renders as ^                             */
/*************************************************************/
TEST(Expression, render_power_2) {
    math::Expression e;
    press( e, { AC::DIGIT_3, AC::POWER_2 });
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
    press( e, { AC::DIGIT_1, AC::ADD, AC::SIN });
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
    press( e, { AC::DIGIT_1, AC::DIGIT_2 });
    EXPECT_FALSE(e.has_placeholder());
}

/*************************************************************/
/*   Function with argument has no placeholder              */
/*************************************************************/
TEST(Expression, function_with_arg_no_placeholder) {
    math::Expression e;
    press( e, { AC::SIN, AC::DIGIT_4 });
    EXPECT_FALSE(e.has_placeholder());
}

/*************************************************************/
/*   Function without argument has placeholder              */
/*************************************************************/
TEST(Expression, function_without_arg_has_placeholder) {
    math::Expression e;
    press( e, { AC::SIN });
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
    press( e, { AC::DIGIT_4, AC::ADD, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4+3");
}

/*************************************************************/
/*   Basic subtraction works                                */
/*************************************************************/
TEST(Expression, basic_subtraction) {
    math::Expression e;
    press( e, { AC::DIGIT_4, AC::SUBTRACT, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4-3");
}

/*************************************************************/
/*   Basic multiplication works                             */
/*************************************************************/
TEST(Expression, basic_multiplication) {
    math::Expression e;
    press( e, { AC::DIGIT_4, AC::MULTIPLY, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "4*3");
}

/*************************************************************/
/*   Basic division works                                   */
/*************************************************************/
TEST(Expression, basic_division) {
    math::Expression e;
    press( e, { AC::DIGIT_4, AC::DIVIDE, AC::DIGIT_3 });
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
    press( e, { AC::DIGIT_5, AC::FACTORIAL });
    EXPECT_EQ(e.eval_string(), "5!");
}

/*************************************************************/
/*   Reciprocal operator works                              */
/*************************************************************/
TEST(Expression, reciprocal_operator) {
    math::Expression e;
    press( e, { AC::DIGIT_2, AC::RECIPROCAL });
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
    press( e, { AC::PI });
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
    press( e, { AC::SQRT, AC::DIGIT_4, AC::ADD, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "sqrt(4+3)");
}

/*************************************************************/
/*   Nested sqrt with division                             */
/*************************************************************/
TEST(Expression, nested_sqrt_with_division) {
    math::Expression e;
    press( e, { AC::SQRT, AC::DIGIT_4, AC::DIVIDE, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "sqrt(4/3)");
}

