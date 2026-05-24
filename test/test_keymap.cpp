/**
 * @file    test_keymap.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 * @brief   Unit tests for keymap string conversion functions.
 */

// C++ Standard Libraries
#include <string>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/core/keymap.hpp>

using namespace ovb::core;

// ─── string_to_key_code ─────────────────────────────────────────────────────

TEST(String_To_Key_Code, Digit_Codes) {
    EXPECT_EQ(string_to_key_code("DIGIT_0"), Key_Code::DIGIT_0);
    EXPECT_EQ(string_to_key_code("DIGIT_1"), Key_Code::DIGIT_1);
    EXPECT_EQ(string_to_key_code("DIGIT_2"), Key_Code::DIGIT_2);
    EXPECT_EQ(string_to_key_code("DIGIT_3"), Key_Code::DIGIT_3);
    EXPECT_EQ(string_to_key_code("DIGIT_4"), Key_Code::DIGIT_4);
    EXPECT_EQ(string_to_key_code("DIGIT_5"), Key_Code::DIGIT_5);
    EXPECT_EQ(string_to_key_code("DIGIT_6"), Key_Code::DIGIT_6);
    EXPECT_EQ(string_to_key_code("DIGIT_7"), Key_Code::DIGIT_7);
    EXPECT_EQ(string_to_key_code("DIGIT_8"), Key_Code::DIGIT_8);
    EXPECT_EQ(string_to_key_code("DIGIT_9"), Key_Code::DIGIT_9);
}

TEST(String_To_Key_Code, Basic_Operators) {
    EXPECT_EQ( core::string_to_key_code("ADD"), Key_Code::ADD );
    EXPECT_EQ( core::string_to_key_code("SUBTRACT"), Key_Code::SUBTRACT );
    EXPECT_EQ( core::string_to_key_code("MULTIPLY"), Key_Code::MULTIPLY );
    EXPECT_EQ( core::string_to_key_code("DIVIDE"), Key_Code::DIVIDE );
    EXPECT_EQ( core::string_to_key_code("EQUALS"), Key_Code::EQUALS );
    EXPECT_EQ( core::string_to_key_code("DECIMAL"), Key_Code::DECIMAL );
    EXPECT_EQ( core::string_to_key_code("BACKSPACE"), Key_Code::BACKSPACE );
    EXPECT_EQ( core::string_to_key_code("CLEAR"), Key_Code::CLEAR );
    EXPECT_EQ( core::string_to_key_code("ALL_CLEAR"), Key_Code::ALL_CLEAR );
}

TEST(String_To_Key_Code, Grouping) {
    EXPECT_EQ(string_to_key_code("PAREN_OPEN"), Key_Code::PAREN_OPEN);
    EXPECT_EQ(string_to_key_code("PAREN_CLOSE"), Key_Code::PAREN_CLOSE);
    EXPECT_EQ(string_to_key_code("PERCENT"), Key_Code::PERCENT);
    EXPECT_EQ(string_to_key_code("NEGATE"), Key_Code::NEGATE);
}

TEST(String_To_Key_Code, Scientific) {
    EXPECT_EQ(string_to_key_code("SIN"), Key_Code::SIN);
    EXPECT_EQ(string_to_key_code("COS"), Key_Code::COS);
    EXPECT_EQ(string_to_key_code("TAN"), Key_Code::TAN);
    EXPECT_EQ(string_to_key_code("ASIN"), Key_Code::ASIN);
    EXPECT_EQ(string_to_key_code("ACOS"), Key_Code::ACOS);
    EXPECT_EQ(string_to_key_code("ATAN"), Key_Code::ATAN);
    EXPECT_EQ(string_to_key_code("LOG"), Key_Code::LOG);
    EXPECT_EQ(string_to_key_code("LN"), Key_Code::LN);
    EXPECT_EQ(string_to_key_code("EXP"), Key_Code::EXP);
    EXPECT_EQ(string_to_key_code("SQRT"), Key_Code::SQRT);
    EXPECT_EQ(string_to_key_code("PI"), Key_Code::PI);
    EXPECT_EQ(string_to_key_code("EULER"), Key_Code::EULER);
}

TEST(String_To_Key_Code, Programmer) {
    EXPECT_EQ(string_to_key_code("BIT_AND"), Key_Code::BIT_AND);
    EXPECT_EQ(string_to_key_code("BIT_OR"), Key_Code::BIT_OR);
    EXPECT_EQ(string_to_key_code("BIT_XOR"), Key_Code::BIT_XOR);
    EXPECT_EQ(string_to_key_code("BIT_NOT"), Key_Code::BIT_NOT);
    EXPECT_EQ(string_to_key_code("SHIFT_LEFT"), Key_Code::SHIFT_LEFT);
    EXPECT_EQ(string_to_key_code("SHIFT_RIGHT"), Key_Code::SHIFT_RIGHT);
    EXPECT_EQ(string_to_key_code("HEX_A"), Key_Code::HEX_A);
    EXPECT_EQ(string_to_key_code("HEX_B"), Key_Code::HEX_B);
    EXPECT_EQ(string_to_key_code("HEX_C"), Key_Code::HEX_C);
    EXPECT_EQ(string_to_key_code("HEX_D"), Key_Code::HEX_D);
    EXPECT_EQ(string_to_key_code("HEX_E"), Key_Code::HEX_E);
    EXPECT_EQ(string_to_key_code("HEX_F"), Key_Code::HEX_F);
}

TEST(String_To_Key_Code, Layer_Control) {
    EXPECT_EQ(string_to_key_code("LAYER_NEXT"), Key_Code::LAYER_NEXT);
    EXPECT_EQ(string_to_key_code("LAYER_PREV"), Key_Code::LAYER_PREV);
    EXPECT_EQ(string_to_key_code("LAYER_CONST"), Key_Code::LAYER_CONST);
    EXPECT_EQ(string_to_key_code("LAYER_ALG"), Key_Code::LAYER_ALG);
    EXPECT_EQ(string_to_key_code("LAYER_TRIG"), Key_Code::LAYER_TRIG);
    EXPECT_EQ(string_to_key_code("LAYER_VAR"), Key_Code::LAYER_VAR);
    EXPECT_EQ(string_to_key_code("LAYER_HOME"), Key_Code::LAYER_HOME);
}

TEST(String_To_Key_Code, Navigation) {
    EXPECT_EQ(string_to_key_code("CURSOR_LEFT"), Key_Code::CURSOR_LEFT);
    EXPECT_EQ(string_to_key_code("CURSOR_RIGHT"), Key_Code::CURSOR_RIGHT);
    EXPECT_EQ(string_to_key_code("CURSOR_UP"), Key_Code::CURSOR_UP);
    EXPECT_EQ(string_to_key_code("CURSOR_DOWN"), Key_Code::CURSOR_DOWN);
    EXPECT_EQ(string_to_key_code("PAGE_UP"), Key_Code::PAGE_UP);
    EXPECT_EQ(string_to_key_code("PAGE_DOWN"), Key_Code::PAGE_DOWN);
}

TEST(String_To_Key_Code, Invalid_String_Returns_NONE) {
    EXPECT_EQ(string_to_key_code("INVALID_CODE"), Key_Code::NONE);
    EXPECT_EQ(string_to_key_code(""), Key_Code::NONE);
    EXPECT_EQ(string_to_key_code("foobar"), Key_Code::NONE);
}

// ─── string_to_key_label ────────────────────────────────────────────────────

TEST(String_To_Key_Label, Digit_Labels) {
    EXPECT_EQ(string_to_key_label("D_0"), Key_Label::D_0);
    EXPECT_EQ(string_to_key_label("D_1"), Key_Label::D_1);
    EXPECT_EQ(string_to_key_label("D_2"), Key_Label::D_2);
    EXPECT_EQ(string_to_key_label("D_3"), Key_Label::D_3);
    EXPECT_EQ(string_to_key_label("D_4"), Key_Label::D_4);
    EXPECT_EQ(string_to_key_label("D_5"), Key_Label::D_5);
    EXPECT_EQ(string_to_key_label("D_6"), Key_Label::D_6);
    EXPECT_EQ(string_to_key_label("D_7"), Key_Label::D_7);
    EXPECT_EQ(string_to_key_label("D_8"), Key_Label::D_8);
    EXPECT_EQ(string_to_key_label("D_9"), Key_Label::D_9);
}

TEST(String_To_Key_Label, Hex_Labels) {
    EXPECT_EQ(string_to_key_label("HEX_A"), Key_Label::HEX_A);
    EXPECT_EQ(string_to_key_label("HEX_B"), Key_Label::HEX_B);
    EXPECT_EQ(string_to_key_label("HEX_C"), Key_Label::HEX_C);
    EXPECT_EQ(string_to_key_label("HEX_D"), Key_Label::HEX_D);
    EXPECT_EQ(string_to_key_label("HEX_E"), Key_Label::HEX_E);
    EXPECT_EQ(string_to_key_label("HEX_F"), Key_Label::HEX_F);
}

TEST(String_To_Key_Label, Arithmetic_Labels) {
    EXPECT_EQ(string_to_key_label("ADD"), Key_Label::ADD);
    EXPECT_EQ(string_to_key_label("SUBTRACT"), Key_Label::SUBTRACT);
    EXPECT_EQ(string_to_key_label("MULTIPLY"), Key_Label::MULTIPLY);
    EXPECT_EQ(string_to_key_label("DIVIDE"), Key_Label::DIVIDE);
    EXPECT_EQ(string_to_key_label("EQUALS"), Key_Label::EQUALS);
    EXPECT_EQ(string_to_key_label("DECIMAL"), Key_Label::DECIMAL);
    EXPECT_EQ(string_to_key_label("PERCENT"), Key_Label::PERCENT);
}

TEST(String_To_Key_Label, Editing_Labels) {
    EXPECT_EQ(string_to_key_label("BACKSPACE"), Key_Label::BACKSPACE);
    EXPECT_EQ(string_to_key_label("CLEAR"), Key_Label::CLEAR);
    EXPECT_EQ(string_to_key_label("ALL_CLEAR"), Key_Label::ALL_CLEAR);
    EXPECT_EQ(string_to_key_label("NEGATE"), Key_Label::NEGATE);
    EXPECT_EQ(string_to_key_label("PAREN_OPEN"), Key_Label::PAREN_OPEN);
    EXPECT_EQ(string_to_key_label("PAREN_CLOSE"), Key_Label::PAREN_CLOSE);
}

TEST(String_To_Key_Label, Scientific_Labels) {
    EXPECT_EQ(string_to_key_label("SIN"), Key_Label::SIN);
    EXPECT_EQ(string_to_key_label("COS"), Key_Label::COS);
    EXPECT_EQ(string_to_key_label("TAN"), Key_Label::TAN);
    EXPECT_EQ(string_to_key_label("ASIN"), Key_Label::ASIN);
    EXPECT_EQ(string_to_key_label("ACOS"), Key_Label::ACOS);
    EXPECT_EQ(string_to_key_label("ATAN"), Key_Label::ATAN);
    EXPECT_EQ(string_to_key_label("LOG"), Key_Label::LOG);
    EXPECT_EQ(string_to_key_label("LN"), Key_Label::LN);
    EXPECT_EQ(string_to_key_label("EXP"), Key_Label::EXP);
    EXPECT_EQ(string_to_key_label("SQRT"), Key_Label::SQRT);
    EXPECT_EQ(string_to_key_label("PI"), Key_Label::PI);
    EXPECT_EQ(string_to_key_label("EULER"), Key_Label::EULER);
    EXPECT_EQ(string_to_key_label("PHI"), Key_Label::PHI);
    EXPECT_EQ(string_to_key_label("TAU"), Key_Label::TAU);
}

TEST(String_To_Key_Label, Programmer_Labels) {
    EXPECT_EQ(string_to_key_label("BIT_AND"), Key_Label::BIT_AND);
    EXPECT_EQ(string_to_key_label("BIT_OR"), Key_Label::BIT_OR);
    EXPECT_EQ(string_to_key_label("BIT_XOR"), Key_Label::BIT_XOR);
    EXPECT_EQ(string_to_key_label("BIT_NOT"), Key_Label::BIT_NOT);
    EXPECT_EQ(string_to_key_label("SHIFT_LEFT"), Key_Label::SHIFT_LEFT);
    EXPECT_EQ(string_to_key_label("SHIFT_RIGHT"), Key_Label::SHIFT_RIGHT);
}

TEST(String_To_Key_Label, Layer_Labels) {
    EXPECT_EQ(string_to_key_label("LAYER_TRIG"), Key_Label::LAYER_TRIG);
    EXPECT_EQ(string_to_key_label("LAYER_ALGEBRA"), Key_Label::LAYER_ALGEBRA);
    EXPECT_EQ(string_to_key_label("LAYER_CONST"), Key_Label::LAYER_CONST);
    EXPECT_EQ(string_to_key_label("LAYER_VAR"), Key_Label::LAYER_VAR);
    EXPECT_EQ(string_to_key_label("LAYER_HOME"), Key_Label::LAYER_HOME);
}

TEST(String_To_Key_Label, Navigation_Labels) {
    EXPECT_EQ(string_to_key_label("CURSOR_LEFT"), Key_Label::CURSOR_LEFT);
    EXPECT_EQ(string_to_key_label("CURSOR_RIGHT"), Key_Label::CURSOR_RIGHT);
    EXPECT_EQ(string_to_key_label("CURSOR_UP"), Key_Label::CURSOR_UP);
    EXPECT_EQ(string_to_key_label("CURSOR_DOWN"), Key_Label::CURSOR_DOWN);
    EXPECT_EQ(string_to_key_label("PG_UP"), Key_Label::PG_UP);
    EXPECT_EQ(string_to_key_label("PG_DN"), Key_Label::PG_DN);
}

TEST(String_To_Key_Label, Invalid_String_Returns_NONE) {
    EXPECT_EQ(string_to_key_label("INVALID_LABEL"), Key_Label::NONE);
    EXPECT_EQ(string_to_key_label(""), Key_Label::NONE);
    EXPECT_EQ(string_to_key_label("foobar"), Key_Label::NONE);
}
