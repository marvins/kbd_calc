/**
 * @file    test_input_key.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 * @brief   Unit tests for input key string conversion functions.
 */

// C++ Standard Libraries
#include <string>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/core/input_key.hpp>

using namespace ovb;

/**********************************/
/*       Test Special Keys        */
/**********************************/
TEST(String_To_Input_Key, Special_Keys) {
    EXPECT_EQ( core::string_to_input_key("NONE"),      core::Input_Key::NONE );
    EXPECT_EQ( core::string_to_input_key("ESCAPE"),    core::Input_Key::ESCAPE );
    EXPECT_EQ( core::string_to_input_key("TAB"),       core::Input_Key::TAB );
    EXPECT_EQ( core::string_to_input_key("BACKSPACE"), core::Input_Key::BACKSPACE );
    EXPECT_EQ( core::string_to_input_key("RETURN"),    core::Input_Key::RETURN );
    EXPECT_EQ( core::string_to_input_key("SPACE"),     core::Input_Key::SPACE );
}

/**********************************/
/*       Test Modifier Keys       */
/**********************************/
TEST(String_To_Input_Key, Modifier_Keys) {
    EXPECT_EQ( core::string_to_input_key("SHIFT"),   core::Input_Key::SHIFT );
    EXPECT_EQ( core::string_to_input_key("CONTROL"), core::Input_Key::CONTROL );
    EXPECT_EQ( core::string_to_input_key("ALT"),     core::Input_Key::ALT );
    EXPECT_EQ( core::string_to_input_key("META"),    core::Input_Key::META );
}

/**********************************/
/*       Test Navigation Keys     */
/**********************************/
TEST(String_To_Input_Key, Navigation_Keys) {
    EXPECT_EQ( core::string_to_input_key("LEFT"),       core::Input_Key::LEFT );
    EXPECT_EQ( core::string_to_input_key("RIGHT"),      core::Input_Key::RIGHT );
    EXPECT_EQ( core::string_to_input_key("UP"),         core::Input_Key::UP );
    EXPECT_EQ( core::string_to_input_key("DOWN"),       core::Input_Key::DOWN );
    EXPECT_EQ( core::string_to_input_key("HOME"),       core::Input_Key::HOME );
    EXPECT_EQ( core::string_to_input_key("END"),        core::Input_Key::END );
    EXPECT_EQ( core::string_to_input_key("PAGE_UP"),    core::Input_Key::PAGE_UP );
    EXPECT_EQ( core::string_to_input_key("PAGE_DOWN"), core::Input_Key::PAGE_DOWN );
    EXPECT_EQ( core::string_to_input_key("INSERT"),    core::Input_Key::INSERT );
    EXPECT_EQ( core::string_to_input_key("DELETE"),    core::Input_Key::DELETE );
}

/**********************************/
/*       Test Function Keys       */
/**********************************/
TEST(String_To_Input_Key, Function_Keys) {
    EXPECT_EQ( core::string_to_input_key("F1"),  core::Input_Key::F1 );
    EXPECT_EQ( core::string_to_input_key("F2"),  core::Input_Key::F2 );
    EXPECT_EQ( core::string_to_input_key("F3"),  core::Input_Key::F3 );
    EXPECT_EQ( core::string_to_input_key("F4"),  core::Input_Key::F4 );
    EXPECT_EQ( core::string_to_input_key("F5"),  core::Input_Key::F5 );
    EXPECT_EQ( core::string_to_input_key("F6"),  core::Input_Key::F6 );
    EXPECT_EQ( core::string_to_input_key("F7"),  core::Input_Key::F7 );
    EXPECT_EQ( core::string_to_input_key("F8"),  core::Input_Key::F8 );
    EXPECT_EQ( core::string_to_input_key("F9"),  core::Input_Key::F9 );
    EXPECT_EQ( core::string_to_input_key("F10"), core::Input_Key::F10 );
    EXPECT_EQ( core::string_to_input_key("F11"), core::Input_Key::F11 );
    EXPECT_EQ( core::string_to_input_key("F12"), core::Input_Key::F12 );
}

/**********************************/
/*       Test Digit Keys          */
/**********************************/
TEST(String_To_Input_Key, Digit_Keys) {
    EXPECT_EQ( core::string_to_input_key("KEY_0"), core::Input_Key::KEY_0 );
    EXPECT_EQ( core::string_to_input_key("KEY_1"), core::Input_Key::KEY_1 );
    EXPECT_EQ( core::string_to_input_key("KEY_2"), core::Input_Key::KEY_2 );
    EXPECT_EQ( core::string_to_input_key("KEY_3"), core::Input_Key::KEY_3 );
    EXPECT_EQ( core::string_to_input_key("KEY_4"), core::Input_Key::KEY_4 );
    EXPECT_EQ( core::string_to_input_key("KEY_5"), core::Input_Key::KEY_5 );
    EXPECT_EQ( core::string_to_input_key("KEY_6"), core::Input_Key::KEY_6 );
    EXPECT_EQ( core::string_to_input_key("KEY_7"), core::Input_Key::KEY_7 );
    EXPECT_EQ( core::string_to_input_key("KEY_8"), core::Input_Key::KEY_8 );
    EXPECT_EQ( core::string_to_input_key("KEY_9"), core::Input_Key::KEY_9 );
}

/**********************************/
/*       Test Alpha Keys          */
/**********************************/
TEST(String_To_Input_Key, Alpha_Keys) {
    EXPECT_EQ( core::string_to_input_key("KEY_A"), core::Input_Key::KEY_A );
    EXPECT_EQ( core::string_to_input_key("KEY_B"), core::Input_Key::KEY_B );
    EXPECT_EQ( core::string_to_input_key("KEY_C"), core::Input_Key::KEY_C );
    EXPECT_EQ( core::string_to_input_key("KEY_Z"), core::Input_Key::KEY_Z );
}

/**********************************/
/*       Test Numpad Keys         */
/**********************************/
TEST(String_To_Input_Key, Numpad_Keys) {
    EXPECT_EQ( core::string_to_input_key("NUMPAD_0"), core::Input_Key::NUMPAD_0 );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_1"), core::Input_Key::NUMPAD_1 );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_9"), core::Input_Key::NUMPAD_9 );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_ADD"),      core::Input_Key::NUMPAD_ADD );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_SUBTRACT"), core::Input_Key::NUMPAD_SUBTRACT );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_MULTIPLY"),  core::Input_Key::NUMPAD_MULTIPLY );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_DIVIDE"),   core::Input_Key::NUMPAD_DIVIDE );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_DECIMAL"),  core::Input_Key::NUMPAD_DECIMAL );
    EXPECT_EQ( core::string_to_input_key("NUMPAD_ENTER"),    core::Input_Key::NUMPAD_ENTER );
}

/**********************************/
/*       Test Punctuation Keys    */
/**********************************/
TEST(String_To_Input_Key, Punctuation_Keys) {
    EXPECT_EQ( core::string_to_input_key("PLUS"),        core::Input_Key::PLUS );
    EXPECT_EQ( core::string_to_input_key("MINUS"),      core::Input_Key::MINUS );
    EXPECT_EQ( core::string_to_input_key("ASTERISK"),    core::Input_Key::ASTERISK );
    EXPECT_EQ( core::string_to_input_key("SLASH"),       core::Input_Key::SLASH );
    EXPECT_EQ( core::string_to_input_key("PERIOD"),      core::Input_Key::PERIOD );
    EXPECT_EQ( core::string_to_input_key("COMMA"),       core::Input_Key::COMMA );
    EXPECT_EQ( core::string_to_input_key("SEMICOLON"),   core::Input_Key::SEMICOLON );
    EXPECT_EQ( core::string_to_input_key("COLON"),       core::Input_Key::COLON );
    EXPECT_EQ( core::string_to_input_key("EQUAL"),       core::Input_Key::EQUAL );
    EXPECT_EQ( core::string_to_input_key("PERCENT"),     core::Input_Key::PERCENT );
}

/**********************************/
/*       Test Bracket Keys        */
/**********************************/
TEST(String_To_Input_Key, Bracket_Keys) {
    EXPECT_EQ( core::string_to_input_key("BRACKET_LEFT"),  core::Input_Key::BRACKET_LEFT );
    EXPECT_EQ( core::string_to_input_key("BRACKET_RIGHT"), core::Input_Key::BRACKET_RIGHT );
    EXPECT_EQ( core::string_to_input_key("BRACE_LEFT"),    core::Input_Key::BRACE_LEFT );
    EXPECT_EQ( core::string_to_input_key("BRACE_RIGHT"),   core::Input_Key::BRACE_RIGHT );
    EXPECT_EQ( core::string_to_input_key("PAREN_LEFT"),     core::Input_Key::PAREN_LEFT );
    EXPECT_EQ( core::string_to_input_key("PAREN_RIGHT"),    core::Input_Key::PAREN_RIGHT );
}

/**********************************/
/*       Test Calculator Keys     */
/**********************************/
TEST(String_To_Input_Key, Calculator_Keys) {
    EXPECT_EQ( core::string_to_input_key("CALC_CLEAR"), core::Input_Key::CALC_CLEAR );
    EXPECT_EQ( core::string_to_input_key("CALC_ENTER"), core::Input_Key::CALC_ENTER );
}

/**********************************/
/*       Test Invalid Strings     */
/**********************************/
TEST(String_To_Input_Key, Invalid_String_Returns_NONE) {
    EXPECT_EQ( core::string_to_input_key("INVALID_KEY"), core::Input_Key::NONE );
    EXPECT_EQ( core::string_to_input_key(""),           core::Input_Key::NONE );
    EXPECT_EQ( core::string_to_input_key("foobar"),      core::Input_Key::NONE );
}


/**********************************/
/*       Test Special Keys        */
/**********************************/
TEST(Input_Key_To_String, Special_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::NONE),      "NONE" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::ESCAPE),    "ESCAPE" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::TAB),       "TAB" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::BACKSPACE), "BACKSPACE" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::RETURN),    "RETURN" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::SPACE),     "SPACE" );
}

/**********************************/
/*       Test Modifier Keys       */
/**********************************/
TEST(Input_Key_To_String, Modifier_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::SHIFT),   "SHIFT" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::CONTROL), "CONTROL" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::ALT),     "ALT" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::META),    "META" );
}

/**********************************/
/*       Test Navigation Keys     */
/**********************************/
TEST(Input_Key_To_String, Navigation_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::LEFT),       "LEFT" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::RIGHT),      "RIGHT" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::UP),         "UP" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::DOWN),       "DOWN" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::HOME),       "HOME" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::END),        "END" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::PAGE_UP),    "PAGE_UP" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::PAGE_DOWN),  "PAGE_DOWN" );
}

/**********************************/
/*       Test Function Keys       */
/**********************************/
TEST(Input_Key_To_String, Function_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::F1),  "F1" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::F2),  "F2" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::F12), "F12" );
}

/**********************************/
/*       Test Digit Keys          */
/**********************************/
TEST(Input_Key_To_String, Digit_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::KEY_0), "KEY_0" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::KEY_5), "KEY_5" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::KEY_9), "KEY_9" );
}

/**********************************/
/*       Test Alpha Keys          */
/**********************************/
TEST(Input_Key_To_String, Alpha_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::KEY_A), "KEY_A" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::KEY_Z), "KEY_Z" );
}

/**********************************/
/*       Test Numpad Keys         */
/**********************************/
TEST(Input_Key_To_String, Numpad_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::NUMPAD_0), "NUMPAD_0" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::NUMPAD_9), "NUMPAD_9" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::NUMPAD_ADD), "NUMPAD_ADD" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::NUMPAD_ENTER), "NUMPAD_ENTER" );
}

/**********************************/
/*       Test Punctuation Keys    */
/**********************************/
TEST(Input_Key_To_String, Punctuation_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::PLUS),     "PLUS" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::MINUS),    "MINUS" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::PERIOD),   "PERIOD" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::EQUAL),     "EQUAL" );
}

/**********************************/
/*       Test Calculator Keys     */
/**********************************/
TEST(Input_Key_To_String, Calculator_Keys) {
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::CALC_CLEAR), "CALC_CLEAR" );
    EXPECT_EQ( core::input_key_to_string(core::Input_Key::CALC_ENTER), "CALC_ENTER" );
}

/**********************************/
/*       Test Unknown Keys        */
/**********************************/
TEST(Input_Key_To_String, Unknown_Key_Returns_UNKNOWN) {
    // Test with a cast to an undefined value
    EXPECT_EQ( core::input_key_to_string(static_cast<core::Input_Key>(65535)), "UNKNOWN" );
}
