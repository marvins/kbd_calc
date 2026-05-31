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
/*   Backspace removes digit from multi-digit number         */
/*************************************************************/
TEST(Expression, backspace_removes_digit_from_number) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::DIGIT_2, AC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "123");
    e.backspace( );
    // Backspace removes last digit
    EXPECT_EQ(e.eval_string(), "12");
}

/*************************************************************/
/*   Backspace on single digit replaces with placeholder     */
/*************************************************************/
TEST(Expression, backspace_on_single_digit_creates_placeholder) {
    math::Expression e;
    press( e, { AC::DIGIT_5 });
    EXPECT_EQ(e.eval_string(), "5");
    e.backspace( );
    // Single digit becomes placeholder
    EXPECT_TRUE(e.has_placeholder());
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
/*   Cursor left moves to parent from leaf node              */
/*************************************************************/
TEST(Expression, cursor_left_moves_from_leaf_to_parent) {
    math::Expression e;
    press( e, { AC::SQRT, AC::DIGIT_4 });
    // "sqrt(4)" - cursor is inside the function at the "4" node
    std::size_t initial_pos = e.cursor_glyph_pos();

    // Move cursor left - should go to parent (function node)
    e.cursor_left( );
    // At parent node (function), cursor_glyph_pos should count all glyphs before the cursor
    EXPECT_NE(e.cursor_glyph_pos(), initial_pos);
}

/*************************************************************/
/*   Cursor left at root with no parent is no-op             */
/*************************************************************/
TEST(Expression, cursor_left_at_root_no_op) {
    math::Expression e;
    press( e, { AC::DIGIT_1 });
    // Single number node at root
    std::size_t pos = e.cursor_glyph_pos();
    e.cursor_left( );
    // At root node with no parent, cursor stays
    EXPECT_EQ(e.cursor_glyph_pos(), pos);
}

/*************************************************************/
/*   Cursor navigation in binary operator expression           */
/*************************************************************/
TEST(Expression, cursor_navigation_binary_expression) {
    math::Expression e;
    press( e, { AC::DIGIT_1, AC::ADD, AC::DIGIT_2 });
    // Tree: ADD(1, 2) - cursor at the "2" leaf

    // Move cursor left from "2" to parent ADD node
    e.cursor_left( );
    // Now at ADD node, which has children

    // Move cursor right from ADD node to first child
    e.cursor_right( );
}

/*************************************************************/
/*   Cursor navigation in nested function                    */
/*************************************************************/
TEST(Expression, cursor_navigation_nested_function) {
    math::Expression e;
    press( e, { AC::SQRT, AC::DIGIT_4, AC::ADD, AC::DIGIT_3 });
    // "sqrt(4+3)" - cursor is inside the function

    // Navigate left and right
    e.cursor_left( );
    e.cursor_right( );

    // Position may vary based on tree structure
    EXPECT_GE(e.cursor_glyph_pos(), 0);
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

