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

using namespace ovb;

// ─── string_to_key_code ─────────────────────────────────────────────────────

TEST(String_To_Key_Code, Digit_Codes) {
    EXPECT_EQ( core::string_to_key_code("DIGIT_0"), core::Key_Code::DIGIT_0 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_1"), core::Key_Code::DIGIT_1 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_2"), core::Key_Code::DIGIT_2 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_3"), core::Key_Code::DIGIT_3 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_4"), core::Key_Code::DIGIT_4 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_5"), core::Key_Code::DIGIT_5 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_6"), core::Key_Code::DIGIT_6 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_7"), core::Key_Code::DIGIT_7 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_8"), core::Key_Code::DIGIT_8 );
    EXPECT_EQ( core::string_to_key_code("DIGIT_9"), core::Key_Code::DIGIT_9 );
}

TEST(String_To_Key_Code, Basic_Operators) {
    EXPECT_EQ( core::string_to_key_code("ADD"),       core::Key_Code::ADD );
    EXPECT_EQ( core::string_to_key_code("SUBTRACT"),  core::Key_Code::SUBTRACT );
    EXPECT_EQ( core::string_to_key_code("MULTIPLY"),  core::Key_Code::MULTIPLY );
    EXPECT_EQ( core::string_to_key_code("DIVIDE"),    core::Key_Code::DIVIDE );
    EXPECT_EQ( core::string_to_key_code("EQUALS"),    core::Key_Code::EQUALS );
    EXPECT_EQ( core::string_to_key_code("DECIMAL"),   core::Key_Code::DECIMAL );
    EXPECT_EQ( core::string_to_key_code("BACKSPACE"), core::Key_Code::BACKSPACE );
    EXPECT_EQ( core::string_to_key_code("CLEAR"),     core::Key_Code::CLEAR );
    EXPECT_EQ( core::string_to_key_code("ALL_CLEAR"), core::Key_Code::ALL_CLEAR );
}

TEST(String_To_Key_Code, Grouping) {
    EXPECT_EQ( core::string_to_key_code("PAREN_OPEN"),  core::Key_Code::PAREN_OPEN );
    EXPECT_EQ( core::string_to_key_code("PAREN_CLOSE"), core::Key_Code::PAREN_CLOSE );
    EXPECT_EQ( core::string_to_key_code("PERCENT"),     core::Key_Code::PERCENT );
    EXPECT_EQ( core::string_to_key_code("NEGATE"),      core::Key_Code::NEGATE );
}

TEST(String_To_Key_Code, Scientific) {
    EXPECT_EQ( core::string_to_key_code("SIN"),  core::Key_Code::SIN );
    EXPECT_EQ( core::string_to_key_code("COS"),  core::Key_Code::COS );
    EXPECT_EQ( core::string_to_key_code("TAN"),  core::Key_Code::TAN );
    EXPECT_EQ( core::string_to_key_code("ASIN"), core::Key_Code::ASIN );
    EXPECT_EQ( core::string_to_key_code("ACOS"), core::Key_Code::ACOS );
    EXPECT_EQ( core::string_to_key_code("ATAN"), core::Key_Code::ATAN );
    EXPECT_EQ( core::string_to_key_code("LOG"),  core::Key_Code::LOG );
    EXPECT_EQ( core::string_to_key_code("LN"),   core::Key_Code::LN );
    EXPECT_EQ( core::string_to_key_code("EXP"),  core::Key_Code::EXP );
    EXPECT_EQ( core::string_to_key_code("SQRT"), core::Key_Code::SQRT );
    EXPECT_EQ( core::string_to_key_code("PI"),   core::Key_Code::PI );
    EXPECT_EQ( core::string_to_key_code("EULER"),core::Key_Code::EULER );
}

TEST(String_To_Key_Code, Programmer) {
    EXPECT_EQ( core::string_to_key_code("BIT_AND"),  core::Key_Code::BIT_AND );
    EXPECT_EQ( core::string_to_key_code("BIT_OR"),   core::Key_Code::BIT_OR );
    EXPECT_EQ( core::string_to_key_code("BIT_XOR"),  core::Key_Code::BIT_XOR );
    EXPECT_EQ( core::string_to_key_code("BIT_NOT"),  core::Key_Code::BIT_NOT );
    EXPECT_EQ( core::string_to_key_code("SHIFT_LEFT"), core::Key_Code::SHIFT_LEFT );
    EXPECT_EQ( core::string_to_key_code("SHIFT_RIGHT"),core::Key_Code::SHIFT_RIGHT );
    EXPECT_EQ( core::string_to_key_code("HEX_A"),    core::Key_Code::HEX_A );
    EXPECT_EQ( core::string_to_key_code("HEX_B"),    core::Key_Code::HEX_B );
    EXPECT_EQ( core::string_to_key_code("HEX_C"),    core::Key_Code::HEX_C );
    EXPECT_EQ( core::string_to_key_code("HEX_D"),    core::Key_Code::HEX_D );
    EXPECT_EQ( core::string_to_key_code("HEX_E"),    core::Key_Code::HEX_E );
    EXPECT_EQ( core::string_to_key_code("HEX_F"),    core::Key_Code::HEX_F );
}

TEST(String_To_Key_Code, Layer_Control) {
    EXPECT_EQ( core::string_to_key_code("LAYER_NEXT"),  core::Key_Code::LAYER_NEXT );
    EXPECT_EQ( core::string_to_key_code("LAYER_PREV"),  core::Key_Code::LAYER_PREV );
    EXPECT_EQ( core::string_to_key_code("LAYER_CONST"), core::Key_Code::LAYER_CONST );
    EXPECT_EQ( core::string_to_key_code("LAYER_ALG"),   core::Key_Code::LAYER_ALG );
    EXPECT_EQ( core::string_to_key_code("LAYER_TRIG"),  core::Key_Code::LAYER_TRIG );
    EXPECT_EQ( core::string_to_key_code("LAYER_VAR"),   core::Key_Code::LAYER_VAR );
    EXPECT_EQ( core::string_to_key_code("LAYER_HOME"),  core::Key_Code::LAYER_HOME );
}

TEST(String_To_Key_Code, Navigation) {
    EXPECT_EQ( core::string_to_key_code("CURSOR_LEFT"),  core::Key_Code::CURSOR_LEFT );
    EXPECT_EQ( core::string_to_key_code("CURSOR_RIGHT"), core::Key_Code::CURSOR_RIGHT );
    EXPECT_EQ( core::string_to_key_code("CURSOR_UP"),    core::Key_Code::CURSOR_UP );
    EXPECT_EQ( core::string_to_key_code("CURSOR_DOWN"),  core::Key_Code::CURSOR_DOWN );
    EXPECT_EQ( core::string_to_key_code("PAGE_UP"),      core::Key_Code::PAGE_UP );
    EXPECT_EQ( core::string_to_key_code("PAGE_DOWN"),    core::Key_Code::PAGE_DOWN );
}

TEST(String_To_Key_Code, Invalid_String_Returns_NONE) {
    EXPECT_EQ( core::string_to_key_code("INVALID_CODE"), core::Key_Code::NONE );
    EXPECT_EQ( core::string_to_key_code(""),             core::Key_Code::NONE );
    EXPECT_EQ( core::string_to_key_code("foobar"),       core::Key_Code::NONE );
}

// ─── key_code_to_display ────────────────────────────────────────────────────

TEST(Key_Code_To_Display, Digit_Codes) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_0), "0" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_1), "1" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_2), "2" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_3), "3" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_4), "4" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_5), "5" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_6), "6" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_7), "7" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_8), "8" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIGIT_9), "9" );
}

TEST(Key_Code_To_Display, Basic_Operators) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::ADD),       "+" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::SUBTRACT),  "-" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::MULTIPLY),  "x" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DIVIDE),    "\xC3\xB7" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::EQUALS),    "=" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::DECIMAL),   "." );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::PERCENT),   "%" );
}

TEST(Key_Code_To_Display, Editing) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::BACKSPACE), "BSP" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::CLEAR),     "CLR" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::ALL_CLEAR), "AC" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::NEGATE),    "+/-" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::PAREN_OPEN),  "(" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::PAREN_CLOSE), ")" );
}

TEST(Key_Code_To_Display, Scientific) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::SIN),   "sin" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::COS),   "cos" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::TAN),   "tan" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::ASIN),  "asin" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::ACOS),  "acos" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::ATAN),  "atan" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::LOG),   "log" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::LN),    "ln" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::EXP),   "e^x" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::SQRT),  "\xE2\x88\x9A" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::PI),    "\xCF\x80" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::EULER), "e" );
}

TEST(Key_Code_To_Display, Programmer) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::BIT_AND),     "AND" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::BIT_OR),      "OR" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::BIT_XOR),     "XOR" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::BIT_NOT),     "NOT" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::SHIFT_LEFT),  "<<" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::SHIFT_RIGHT), ">>" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::HEX_A),       "A" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::HEX_B),       "B" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::HEX_C),       "C" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::HEX_D),       "D" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::HEX_E),       "E" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::HEX_F),       "F" );
}

TEST(Key_Code_To_Display, Navigation) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::CURSOR_LEFT),  "<" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::CURSOR_RIGHT), ">" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::CURSOR_UP),    "^" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::CURSOR_DOWN),  "v" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::PAGE_UP),      "PgUp" );
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::PAGE_DOWN),    "PgDn" );
}

TEST(Key_Code_To_Display, NONE_Returns_Empty) {
    EXPECT_EQ( core::key_code_to_display(core::Key_Code::NONE), "" );
}
