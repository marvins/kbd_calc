/**
 * @file    test_expression.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Unit tests for the Expression class.
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
/*   Consecutive digits form a single number token           */
/*************************************************************/
TEST(Expression, number_digits_build_single_token) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "123");
}

/*************************************************************/
/*   Decimal point can only appear once per number           */
/*************************************************************/
TEST(Expression, number_decimal_allowed_once) {
    math::Expression e;
    press( e, { KC::DIGIT_3, KC::DECIMAL, KC::DIGIT_1, KC::DECIMAL });
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
/*   Operator creates separate number tokens                   */
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
/*  Backspace atomic token  */
/****************************/

/*************************************************************/
/*   Function tokens are deleted as a whole unit             */
/*************************************************************/
TEST(Expression, backspace_deletes_function_atomically) {
    math::Expression e;
    press( e, { KC::SIN });
    e.backspace( );
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
}

/*************************************************************/
/*   Constant tokens are deleted as a whole unit             */
/*************************************************************/
TEST(Expression, backspace_deletes_constant_atomically) {
    math::Expression e;
    press( e, { KC::PI });
    e.backspace( );
    EXPECT_TRUE(e.empty());
}

/*************************************************************/
/*   Backspace removes last digit from number                */
/*************************************************************/
TEST(Expression, backspace_deletes_one_digit_from_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    e.backspace( );
    EXPECT_EQ(e.eval_string(), "12");
}

/*************************************************************/
/*   Backspace on single-digit number removes entire token   */
/*************************************************************/
TEST(Expression, backspace_removes_empty_number_token) {
    math::Expression e;
    press( e, { KC::DIGIT_1 });
    e.backspace( );
    EXPECT_TRUE(e.empty());
}

/*************************************************************/
/*   Backspace handles function args then function itself    */
/*************************************************************/
TEST(Expression, backspace_after_function_then_number) {
    math::Expression e;
    press( e, { KC::SIN, KC::DIGIT_3 });
    e.backspace( );                      // removes '3'
    EXPECT_EQ(e.eval_string(), "sin(");
    e.backspace( );                      // removes 'sin(' atomically
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
/*   Cursor left moves character-by-character in numbers     */
/*************************************************************/
TEST(Expression, cursor_left_moves_into_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
    e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 1);
}

/*************************************************************/
/*   Cursor left jumps over non-number tokens                */
/*************************************************************/
TEST(Expression, cursor_left_jumps_over_operator) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2 });
    // cursor is inside '2' token at pos 1
    e.cursor_left( );   // exit '2', between '+' and '2'
    e.cursor_left( );   // jump over '+'
    e.cursor_left( );   // enter '1', at char 1
    EXPECT_EQ(e.cursor_glyph_pos(), 1);
}

/*************************************************************/
/*   Cursor right at end of expression is no-op              */
/*************************************************************/
TEST(Expression, cursor_right_does_nothing_at_end) {
    math::Expression e;
    press( e, { KC::DIGIT_1 });
    int pos = e.cursor_glyph_pos();
    e.cursor_right( );
    EXPECT_EQ(e.cursor_glyph_pos(), pos);
}

/*************************************************************/
/*   Cursor left stops at beginning of expression            */
/*************************************************************/
TEST(Expression, cursor_left_does_nothing_at_start) {
    math::Expression e;
    press( e, { KC::DIGIT_1 });
    // After multiple lefts at start, cursor stays at position 1 (implementation detail)
    e.cursor_left( ); e.cursor_left( ); e.cursor_left( );
    EXPECT_EQ(e.cursor_glyph_pos(), 1);
}

/****************************/
/*  Backspace mid-cursor    */
/****************************/

/*************************************************************/
/*   Insert works with cursor in middle of number            */
/*************************************************************/
TEST(Expression, cursor_backspace_insert_in_middle_of_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_3 });  // "13"
    e.cursor_left( );                          // cursor between '1' and '3'
    e.insert( KC::DIGIT_2);                    // "123"
    EXPECT_EQ(e.eval_string(), "123");
}

/*************************************************************/
/*   Backspace deletes char before cursor in number          */
/*************************************************************/
TEST(Expression, cursor_backspace_backspace_in_middle_of_number) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });  // "123"
    e.cursor_left( );   // pos 2
    e.cursor_left( );   // pos 1
    e.backspace( );     // removes '1' → "23"
    EXPECT_EQ(e.eval_string(), "23");
}

/*************************************************************/
/*   Backspace between tokens removes left token             */
/*************************************************************/
TEST(Expression, cursor_backspace_between_tokens_removes_left_token) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2 });
    e.cursor_left( );   // inside '2' at char 1
    e.cursor_left( );   // exit '2', between '+' and '2'
    e.backspace( );     // removes '+' token
    EXPECT_EQ(e.eval_string(), "12");
}

/****************************/
/*  Auto-balanced parens    */
/****************************/

/*************************************************************/
/*   PAREN_OPEN inserts both '(' and ')' with cursor between   */
/*************************************************************/
TEST(Expression, paren_open_inserts_balanced_pair) {
    math::Expression e;
    e.insert(KC::PAREN_OPEN);
    EXPECT_EQ(e.eval_string(), "()");
}

/*************************************************************/
/*   Typing inside parens works correctly                     */
/*************************************************************/
TEST(Expression, type_inside_auto_parens) {
    math::Expression e;
    e.insert(KC::PAREN_OPEN);
    e.insert(KC::DIGIT_2);
    e.insert(KC::ADD);
    e.insert(KC::DIGIT_3);
    EXPECT_EQ(e.eval_string(), "(2+3)");
}

/*************************************************************/
/*   Backspace on '(' deletes both parens atomically          */
/*************************************************************/
TEST(Expression, backspace_deletes_balanced_parens_atomically) {
    math::Expression e;
    e.insert(KC::PAREN_OPEN);
    EXPECT_EQ(e.eval_string(), "()");
    e.backspace();
    EXPECT_TRUE(e.empty());
}

/*************************************************************/
/*   Backspace with content inside only deletes content       */
/*************************************************************/
TEST(Expression, backspace_inside_parens_deletes_content_first) {
    math::Expression e;
    e.insert(KC::PAREN_OPEN);
    e.insert(KC::DIGIT_5);
    e.backspace();  // deletes '5'
    EXPECT_EQ(e.eval_string(), "()");
    e.backspace();  // now deletes '()' atomically
    EXPECT_TRUE(e.empty());
}

/****************************/
/*     Render/eval string   */
/****************************/

/*************************************************************/
/*   Functions render with parentheses and args              */
/*************************************************************/
TEST(Expression, render_sin_of_pi) {
    math::Expression e;
    press( e, { KC::SIN, KC::PI, KC::PAREN_CLOSE });
    EXPECT_EQ(e.eval_string(), "sin(pi)");
}

/*************************************************************/
/*   sqrt() renders correctly with argument                  */
/*************************************************************/
TEST(Expression, render_sqrt_of_number) {
    math::Expression e;
    press( e, { KC::SQRT, KC::DIGIT_4, KC::PAREN_CLOSE });
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
/*   Clear removes all tokens and resets cursor              */
/*************************************************************/
TEST(Expression, clear_clears_all) {
    math::Expression e;
    press( e, { KC::DIGIT_1, KC::ADD, KC::SIN });
    e.clear( );
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
    EXPECT_EQ(e.cursor_glyph_pos(), 0);
}
