/**
 * @file    test_action_code.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 * @brief   Unit tests for action code string conversion functions.
 */

// C++ Standard Libraries
#include <string>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/core/action_code.hpp>

using namespace ovb;

// ─── string_to_action_code ───────────────────────────────────────────────────

TEST(String_To_Action_Code, Evaluation) {
    EXPECT_EQ( core::string_to_action_code("EVAL"),      core::Action_Code::EVAL );
    EXPECT_EQ( core::string_to_action_code("APPROX"),    core::Action_Code::APPROX );
}

TEST(String_To_Action_Code, Basic_Arithmetic) {
    EXPECT_EQ( core::string_to_action_code("NEGATE"),    core::Action_Code::NEGATE );
    EXPECT_EQ( core::string_to_action_code("PERCENT"),   core::Action_Code::PERCENT );
}

TEST(String_To_Action_Code, Scientific) {
    EXPECT_EQ( core::string_to_action_code("SIN"),   core::Action_Code::SIN );
    EXPECT_EQ( core::string_to_action_code("COS"),   core::Action_Code::COS );
    EXPECT_EQ( core::string_to_action_code("TAN"),   core::Action_Code::TAN );
    EXPECT_EQ( core::string_to_action_code("ASIN"),  core::Action_Code::ASIN );
    EXPECT_EQ( core::string_to_action_code("ACOS"),  core::Action_Code::ACOS );
    EXPECT_EQ( core::string_to_action_code("ATAN"),  core::Action_Code::ATAN );
    EXPECT_EQ( core::string_to_action_code("ATAN2"), core::Action_Code::ATAN2 );
    EXPECT_EQ( core::string_to_action_code("LOG"),   core::Action_Code::LOG );
    EXPECT_EQ( core::string_to_action_code("LN"),    core::Action_Code::LN );
    EXPECT_EQ( core::string_to_action_code("EXP"),   core::Action_Code::EXP );
    EXPECT_EQ( core::string_to_action_code("SQRT"),  core::Action_Code::SQRT );
    EXPECT_EQ( core::string_to_action_code("POWER_2"),  core::Action_Code::POWER_2 );
    EXPECT_EQ( core::string_to_action_code("POWER_3"),  core::Action_Code::POWER_3 );
    EXPECT_EQ( core::string_to_action_code("POWER_N"),  core::Action_Code::POWER_N );
    EXPECT_EQ( core::string_to_action_code("FACTORIAL"), core::Action_Code::FACTORIAL );
    EXPECT_EQ( core::string_to_action_code("RECIPROCAL"), core::Action_Code::RECIPROCAL );
}

TEST(String_To_Action_Code, Constants) {
    EXPECT_EQ( core::string_to_action_code("PI"),    core::Action_Code::PI );
    EXPECT_EQ( core::string_to_action_code("EULER"), core::Action_Code::EULER );
    EXPECT_EQ( core::string_to_action_code("PHI"),   core::Action_Code::PHI );
    EXPECT_EQ( core::string_to_action_code("TAU"),   core::Action_Code::TAU );
}

TEST(String_To_Action_Code, Math_Functions) {
    EXPECT_EQ( core::string_to_action_code("CEIL"),  core::Action_Code::CEIL );
    EXPECT_EQ( core::string_to_action_code("FLOOR"), core::Action_Code::FLOOR );
    EXPECT_EQ( core::string_to_action_code("ABS"),   core::Action_Code::ABS );
}

TEST(String_To_Action_Code, Programmer) {
    EXPECT_EQ( core::string_to_action_code("BIT_AND"),     core::Action_Code::BIT_AND );
    EXPECT_EQ( core::string_to_action_code("BIT_OR"),      core::Action_Code::BIT_OR );
    EXPECT_EQ( core::string_to_action_code("BIT_XOR"),     core::Action_Code::BIT_XOR );
    EXPECT_EQ( core::string_to_action_code("BIT_NOT"),     core::Action_Code::BIT_NOT );
    EXPECT_EQ( core::string_to_action_code("SHIFT_LEFT"),  core::Action_Code::SHIFT_LEFT );
    EXPECT_EQ( core::string_to_action_code("SHIFT_RIGHT"), core::Action_Code::SHIFT_RIGHT );
    EXPECT_EQ( core::string_to_action_code("HEX_A"),       core::Action_Code::HEX_A );
    EXPECT_EQ( core::string_to_action_code("HEX_B"),       core::Action_Code::HEX_B );
    EXPECT_EQ( core::string_to_action_code("HEX_C"),       core::Action_Code::HEX_C );
    EXPECT_EQ( core::string_to_action_code("HEX_D"),       core::Action_Code::HEX_D );
    EXPECT_EQ( core::string_to_action_code("HEX_E"),       core::Action_Code::HEX_E );
    EXPECT_EQ( core::string_to_action_code("HEX_F"),       core::Action_Code::HEX_F );
}

TEST(String_To_Action_Code, Memory) {
    EXPECT_EQ( core::string_to_action_code("MEM_STORE"),  core::Action_Code::MEM_STORE );
    EXPECT_EQ( core::string_to_action_code("MEM_RECALL"), core::Action_Code::MEM_RECALL );
    EXPECT_EQ( core::string_to_action_code("MEM_ADD"),    core::Action_Code::MEM_ADD );
    EXPECT_EQ( core::string_to_action_code("MEM_CLEAR"),  core::Action_Code::MEM_CLEAR );
}

TEST(String_To_Action_Code, Cursor) {
    EXPECT_EQ( core::string_to_action_code("CURSOR_HOME"),       core::Action_Code::CURSOR_HOME );
    EXPECT_EQ( core::string_to_action_code("CURSOR_END"),        core::Action_Code::CURSOR_END );
    EXPECT_EQ( core::string_to_action_code("CURSOR_LEFT_WORD"),  core::Action_Code::CURSOR_LEFT_WORD );
    EXPECT_EQ( core::string_to_action_code("CURSOR_RIGHT_WORD"), core::Action_Code::CURSOR_RIGHT_WORD );
}

TEST(String_To_Action_Code, Layer_Control) {
    EXPECT_EQ( core::string_to_action_code("GO_HOME_LAYER"),  core::Action_Code::GO_HOME_LAYER );
    EXPECT_EQ( core::string_to_action_code("GO_CONST_LAYER"), core::Action_Code::GO_CONST_LAYER );
    EXPECT_EQ( core::string_to_action_code("GO_ALG_LAYER"),   core::Action_Code::GO_ALG_LAYER );
    EXPECT_EQ( core::string_to_action_code("GO_TRIG_LAYER"),  core::Action_Code::GO_TRIG_LAYER );
    EXPECT_EQ( core::string_to_action_code("GO_VAR_LAYER"),   core::Action_Code::GO_VAR_LAYER );
    EXPECT_EQ( core::string_to_action_code("NEXT_LAYER"),     core::Action_Code::NEXT_LAYER );
    EXPECT_EQ( core::string_to_action_code("PREV_LAYER"),     core::Action_Code::PREV_LAYER );
}

TEST(String_To_Action_Code, System) {
    EXPECT_EQ( core::string_to_action_code("TOGGLE_MATH_LAYOUT"), core::Action_Code::TOGGLE_MATH_LAYOUT );
    EXPECT_EQ( core::string_to_action_code("NONE_ACTION"),        core::Action_Code::NONE_ACTION );
}

TEST(String_To_Action_Code, Invalid_String_Returns_NONE) {
    EXPECT_EQ( core::string_to_action_code("INVALID_CODE"), core::Action_Code::NONE );
    EXPECT_EQ( core::string_to_action_code(""),             core::Action_Code::NONE );
    EXPECT_EQ( core::string_to_action_code("foobar"),       core::Action_Code::NONE );
}

// ─── action_code_to_display ───────────────────────────────────────────────────

TEST(Action_Code_To_Display, Evaluation) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::EVAL),      "Eval" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::APPROX),    "Aprx" );
}

TEST(Action_Code_To_Display, Basic_Arithmetic) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::NEGATE),    "+/-" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::PERCENT),   "%" );
}

TEST(Action_Code_To_Display, Scientific) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::SIN),   "sin" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::COS),   "cos" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::TAN),   "tan" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::ASIN),  "asin" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::ACOS),  "acos" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::ATAN),  "atan" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::ATAN2), "atan2" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::LOG),   "log" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::LN),    "ln" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::EXP),   "e^x" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::SQRT),  "\xE2\x88\x9A" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::POWER_2),  "x\xC2\xB2" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::POWER_3),  "x\xC2\xB3" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::POWER_N),  "x^n" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::FACTORIAL), "n!" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::RECIPROCAL), "1/x" );
}

TEST(Action_Code_To_Display, Constants) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::PI),    "\xCF\x80" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::EULER), "e" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::PHI),   "\xCF\x86" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::TAU),   "\xCF\x84" );
}

TEST(Action_Code_To_Display, Math_Functions) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::CEIL),  "ceil" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::FLOOR), "floor" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::ABS),   "abs" );
}

TEST(Action_Code_To_Display, Programmer) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::BIT_AND),     "&" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::BIT_OR),      "|" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::BIT_XOR),     "^" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::BIT_NOT),     "~" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::SHIFT_LEFT),  "<<" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::SHIFT_RIGHT), ">>" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::HEX_A),       "A" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::HEX_B),       "B" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::HEX_C),       "C" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::HEX_D),       "D" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::HEX_E),       "E" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::HEX_F),       "F" );
}

TEST(Action_Code_To_Display, Memory) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::MEM_STORE),  "MS" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::MEM_RECALL), "MR" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::MEM_ADD),    "M+" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::MEM_CLEAR),  "MC" );
}

TEST(Action_Code_To_Display, Cursor) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::CURSOR_HOME),       "Home" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::CURSOR_END),        "End" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::CURSOR_LEFT_WORD),  "\xE2\x8C\x98\xE2\x86\x90" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::CURSOR_RIGHT_WORD), "\xE2\x8C\x98\xE2\x86\x92" );
}

TEST(Action_Code_To_Display, Layer_Control) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::GO_HOME_LAYER),  "Home" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::GO_CONST_LAYER), "CST" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::GO_ALG_LAYER),   "ALG" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::GO_TRIG_LAYER),  "TRG" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::GO_VAR_LAYER),   "Var" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::NEXT_LAYER),     "Next" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::PREV_LAYER),     "Prev" );
}

TEST(Action_Code_To_Display, System) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::TOGGLE_MATH_LAYOUT), "MATH" );
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::NONE_ACTION),        "" );
}

TEST(Action_Code_To_Display, NONE_Returns_Empty) {
    EXPECT_EQ( core::action_code_to_display(core::Action_Code::NONE), "" );
}

// ─── action_code_to_string ────────────────────────────────────────────────────

TEST(Action_Code_To_String, Evaluation) {
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::EVAL),      "EVAL" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::APPROX),    "APPROX" );
}

TEST(Action_Code_To_String, Basic_Arithmetic) {
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::NEGATE),    "NEGATE" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::PERCENT),   "PERCENT" );
}

TEST(Action_Code_To_String, Scientific) {
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::SIN),   "SIN" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::COS),   "COS" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::TAN),   "TAN" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::ASIN),  "ASIN" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::ACOS),  "ACOS" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::ATAN),  "ATAN" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::ATAN2), "ATAN2" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::LOG),   "LOG" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::LN),    "LN" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::EXP),   "EXP" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::SQRT),  "SQRT" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::POWER_2),  "POWER_2" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::POWER_3),  "POWER_3" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::POWER_N),  "POWER_N" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::FACTORIAL), "FACTORIAL" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::RECIPROCAL), "RECIPROCAL" );
}

TEST(Action_Code_To_String, Constants) {
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::PI),    "PI" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::EULER), "EULER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::PHI),   "PHI" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::TAU),   "TAU" );
}

TEST(Action_Code_To_String, Layer_Control) {
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::GO_HOME_LAYER),  "GO_HOME_LAYER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::GO_CONST_LAYER), "GO_CONST_LAYER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::GO_ALG_LAYER),   "GO_ALG_LAYER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::GO_TRIG_LAYER),  "GO_TRIG_LAYER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::GO_VAR_LAYER),   "GO_VAR_LAYER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::NEXT_LAYER),     "NEXT_LAYER" );
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::PREV_LAYER),     "PREV_LAYER" );
}

TEST(Action_Code_To_String, NONE_Returns_NONE) {
    EXPECT_EQ( core::action_code_to_string(core::Action_Code::NONE), "NONE" );
}
