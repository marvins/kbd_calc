#include <gtest/gtest.h>
#include "core/expression.hpp"

using KC = Key_Code;

static void press(Expression& e, std::initializer_list<KC> keys) {
    for (KC k : keys)
        e.insert(k);
}

// ── Number building ───────────────────────────────────────────────────────────

TEST(Expression_Number, Digits_Build_Single_Token) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "123");
}

TEST(Expression_Number, Decimal_Allowed_Once) {
    Expression e;
    press(e, { KC::DIGIT_3, KC::DECIMAL, KC::DIGIT_1, KC::DECIMAL });
    EXPECT_EQ(e.eval_string(), "3.1");
}

TEST(Expression_Number, Leading_Decimal_Starts_Number) {
    Expression e;
    press(e, { KC::DECIMAL, KC::DIGIT_5 });
    EXPECT_EQ(e.eval_string(), ".5");
}

TEST(Expression_Number, Two_Numbers_Separated_By_Operator) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2 });
    EXPECT_EQ(e.eval_string(), "1+2");
}

TEST(Expression_Number, Decimal_Allowed_In_Second_Number) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2, KC::DECIMAL, KC::DIGIT_3 });
    EXPECT_EQ(e.eval_string(), "1+2.3");
}

// ── Backspace — atomic token deletion ────────────────────────────────────────

TEST(Expression_Backspace, Deletes_Function_Atomically) {
    Expression e;
    press(e, { KC::SIN });
    e.backspace();
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
}

TEST(Expression_Backspace, Deletes_Constant_Atomically) {
    Expression e;
    press(e, { KC::PI });
    e.backspace();
    EXPECT_TRUE(e.empty());
}

TEST(Expression_Backspace, Deletes_One_Digit_From_Number) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    e.backspace();
    EXPECT_EQ(e.eval_string(), "12");
}

TEST(Expression_Backspace, Removes_Empty_Number_Token) {
    Expression e;
    press(e, { KC::DIGIT_1 });
    e.backspace();
    EXPECT_TRUE(e.empty());
}

TEST(Expression_Backspace, Backspace_After_Function_Then_Number) {
    Expression e;
    press(e, { KC::SIN, KC::DIGIT_3 });
    e.backspace();                      // removes '3'
    EXPECT_EQ(e.eval_string(), "sin(");
    e.backspace();                      // removes 'sin(' atomically
    EXPECT_TRUE(e.empty());
}

TEST(Expression_Backspace, Does_Nothing_At_Start) {
    Expression e;
    e.backspace();
    EXPECT_TRUE(e.empty());
}

// ── Cursor movement ───────────────────────────────────────────────────────────

TEST(Expression_Cursor, Cursor_Glyph_Pos_At_End) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::DIGIT_2 });
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
}

TEST(Expression_Cursor, Cursor_Left_Moves_Into_Number) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });
    e.cursor_left();
    EXPECT_EQ(e.cursor_glyph_pos(), 2);
    e.cursor_left();
    EXPECT_EQ(e.cursor_glyph_pos(), 1);
}

TEST(Expression_Cursor, Cursor_Left_Jumps_Over_Operator) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2 });
    // cursor is inside '2' token at pos 1
    e.cursor_left();   // exit '2', between '+' and '2'
    e.cursor_left();   // jump over '+'
    e.cursor_left();   // enter '1', at char 1
    EXPECT_EQ(e.cursor_glyph_pos(), 1);
}

TEST(Expression_Cursor, Cursor_Right_Does_Nothing_At_End) {
    Expression e;
    press(e, { KC::DIGIT_1 });
    int pos = e.cursor_glyph_pos();
    e.cursor_right();
    EXPECT_EQ(e.cursor_glyph_pos(), pos);
}

TEST(Expression_Cursor, Cursor_Left_Does_Nothing_At_Start) {
    Expression e;
    press(e, { KC::DIGIT_1 });
    e.cursor_left(); e.cursor_left(); e.cursor_left(); // past start
    EXPECT_EQ(e.cursor_glyph_pos(), 0);
}

// ── Backspace with mid-expression cursor ─────────────────────────────────────

TEST(Expression_Cursor_Backspace, Insert_In_Middle_Of_Number) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::DIGIT_3 });  // "13"
    e.cursor_left();                          // cursor between '1' and '3'
    e.insert(KC::DIGIT_2);                    // "123"
    EXPECT_EQ(e.eval_string(), "123");
}

TEST(Expression_Cursor_Backspace, Backspace_In_Middle_Of_Number) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::DIGIT_2, KC::DIGIT_3 });  // "123"
    e.cursor_left();   // pos 2
    e.cursor_left();   // pos 1
    e.backspace();     // removes '1' → "23"
    EXPECT_EQ(e.eval_string(), "23");
}

TEST(Expression_Cursor_Backspace, Backspace_Between_Tokens_Removes_Left_Token) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::ADD, KC::DIGIT_2 });
    e.cursor_left();   // inside '2' at char 1
    e.cursor_left();   // exit '2', between '+' and '2'
    e.backspace();     // removes '+' token
    EXPECT_EQ(e.eval_string(), "12");
}

// ── render / eval string ─────────────────────────────────────────────────────

TEST(Expression_Render, Sin_Of_Pi) {
    Expression e;
    press(e, { KC::SIN, KC::PI, KC::PAREN_CLOSE });
    EXPECT_EQ(e.eval_string(), "sin(pi)");
}

TEST(Expression_Render, Sqrt_Of_Number) {
    Expression e;
    press(e, { KC::SQRT, KC::DIGIT_4, KC::PAREN_CLOSE });
    EXPECT_EQ(e.eval_string(), "sqrt(4)");
}

TEST(Expression_Render, Power_2) {
    Expression e;
    press(e, { KC::DIGIT_3, KC::POWER_2 });
    EXPECT_EQ(e.eval_string(), "3^2");
}

// ── clear ─────────────────────────────────────────────────────────────────────

TEST(Expression_Clear, Clears_All) {
    Expression e;
    press(e, { KC::DIGIT_1, KC::ADD, KC::SIN });
    e.clear();
    EXPECT_TRUE(e.empty());
    EXPECT_EQ(e.eval_string(), "");
    EXPECT_EQ(e.cursor_glyph_pos(), 0);
}
