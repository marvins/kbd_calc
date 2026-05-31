/**
 * @file    input_key.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Input key implementation
 */
#include <overboard/core/input_key.hpp>

namespace ovb::core {

/**************************************/
/*      String to Input Key          */
/**************************************/
Input_Key string_to_input_key(const std::string& str) {
    if (str == "NONE")           return Input_Key::NONE;

    // Escape and special
    if (str == "ESCAPE")         return Input_Key::ESCAPE;
    if (str == "TAB")            return Input_Key::TAB;
    if (str == "BACKSPACE")      return Input_Key::BACKSPACE;
    if (str == "RETURN")         return Input_Key::RETURN;
    if (str == "ENTER")          return Input_Key::ENTER;
    if (str == "SPACE")          return Input_Key::SPACE;

    // Modifiers
    if (str == "SHIFT")          return Input_Key::SHIFT;
    if (str == "CONTROL")        return Input_Key::CONTROL;
    if (str == "ALT")            return Input_Key::ALT;
    if (str == "META")           return Input_Key::META;

    // Navigation
    if (str == "LEFT")           return Input_Key::LEFT;
    if (str == "RIGHT")          return Input_Key::RIGHT;
    if (str == "UP")             return Input_Key::UP;
    if (str == "DOWN")           return Input_Key::DOWN;
    if (str == "HOME")           return Input_Key::HOME;
    if (str == "END")            return Input_Key::END;
    if (str == "PAGE_UP")        return Input_Key::PAGE_UP;
    if (str == "PAGE_DOWN")      return Input_Key::PAGE_DOWN;
    if (str == "INSERT")         return Input_Key::INSERT;
    if (str == "DELETE")         return Input_Key::DELETE;

    // Function keys
    if (str == "F1")             return Input_Key::F1;
    if (str == "F2")             return Input_Key::F2;
    if (str == "F3")             return Input_Key::F3;
    if (str == "F4")             return Input_Key::F4;
    if (str == "F5")             return Input_Key::F5;
    if (str == "F6")             return Input_Key::F6;
    if (str == "F7")             return Input_Key::F7;
    if (str == "F8")             return Input_Key::F8;
    if (str == "F9")             return Input_Key::F9;
    if (str == "F10")            return Input_Key::F10;
    if (str == "F11")            return Input_Key::F11;
    if (str == "F12")            return Input_Key::F12;

    // Digits
    if (str == "KEY_0")          return Input_Key::KEY_0;
    if (str == "KEY_1")          return Input_Key::KEY_1;
    if (str == "KEY_2")          return Input_Key::KEY_2;
    if (str == "KEY_3")          return Input_Key::KEY_3;
    if (str == "KEY_4")          return Input_Key::KEY_4;
    if (str == "KEY_5")          return Input_Key::KEY_5;
    if (str == "KEY_6")          return Input_Key::KEY_6;
    if (str == "KEY_7")          return Input_Key::KEY_7;
    if (str == "KEY_8")          return Input_Key::KEY_8;
    if (str == "KEY_9")          return Input_Key::KEY_9;

    // Letters
    if (str == "KEY_A")          return Input_Key::KEY_A;
    if (str == "KEY_B")          return Input_Key::KEY_B;
    if (str == "KEY_C")          return Input_Key::KEY_C;
    if (str == "KEY_D")          return Input_Key::KEY_D;
    if (str == "KEY_E")          return Input_Key::KEY_E;
    if (str == "KEY_F")          return Input_Key::KEY_F;
    if (str == "KEY_G")          return Input_Key::KEY_G;
    if (str == "KEY_H")          return Input_Key::KEY_H;
    if (str == "KEY_I")          return Input_Key::KEY_I;
    if (str == "KEY_J")          return Input_Key::KEY_J;
    if (str == "KEY_K")          return Input_Key::KEY_K;
    if (str == "KEY_L")          return Input_Key::KEY_L;
    if (str == "KEY_M")          return Input_Key::KEY_M;
    if (str == "KEY_N")          return Input_Key::KEY_N;
    if (str == "KEY_O")          return Input_Key::KEY_O;
    if (str == "KEY_P")          return Input_Key::KEY_P;
    if (str == "KEY_Q")          return Input_Key::KEY_Q;
    if (str == "KEY_R")          return Input_Key::KEY_R;
    if (str == "KEY_S")          return Input_Key::KEY_S;
    if (str == "KEY_T")          return Input_Key::KEY_T;
    if (str == "KEY_U")          return Input_Key::KEY_U;
    if (str == "KEY_V")          return Input_Key::KEY_V;
    if (str == "KEY_W")          return Input_Key::KEY_W;
    if (str == "KEY_X")          return Input_Key::KEY_X;
    if (str == "KEY_Y")          return Input_Key::KEY_Y;
    if (str == "KEY_Z")          return Input_Key::KEY_Z;

    // Numpad
    if (str == "NUMPAD_0")       return Input_Key::NUMPAD_0;
    if (str == "NUMPAD_1")       return Input_Key::NUMPAD_1;
    if (str == "NUMPAD_2")       return Input_Key::NUMPAD_2;
    if (str == "NUMPAD_3")       return Input_Key::NUMPAD_3;
    if (str == "NUMPAD_4")       return Input_Key::NUMPAD_4;
    if (str == "NUMPAD_5")       return Input_Key::NUMPAD_5;
    if (str == "NUMPAD_6")       return Input_Key::NUMPAD_6;
    if (str == "NUMPAD_7")       return Input_Key::NUMPAD_7;
    if (str == "NUMPAD_8")       return Input_Key::NUMPAD_8;
    if (str == "NUMPAD_9")       return Input_Key::NUMPAD_9;
    if (str == "NUMPAD_ADD")     return Input_Key::NUMPAD_ADD;
    if (str == "NUMPAD_SUBTRACT") return Input_Key::NUMPAD_SUBTRACT;
    if (str == "NUMPAD_MULTIPLY") return Input_Key::NUMPAD_MULTIPLY;
    if (str == "NUMPAD_DIVIDE")   return Input_Key::NUMPAD_DIVIDE;
    if (str == "NUMPAD_DECIMAL") return Input_Key::NUMPAD_DECIMAL;
    if (str == "NUMPAD_ENTER")   return Input_Key::NUMPAD_ENTER;

    // Symbols
    if (str == "PLUS")           return Input_Key::PLUS;
    if (str == "MINUS")          return Input_Key::MINUS;
    if (str == "ASTERISK")       return Input_Key::ASTERISK;
    if (str == "SLASH")          return Input_Key::SLASH;
    if (str == "PERIOD")         return Input_Key::PERIOD;
    if (str == "COMMA")          return Input_Key::COMMA;
    if (str == "SEMICOLON")      return Input_Key::SEMICOLON;
    if (str == "COLON")          return Input_Key::COLON;
    if (str == "EXCLAMATION")    return Input_Key::EXCLAMATION;
    if (str == "QUESTION")       return Input_Key::QUESTION;
    if (str == "QUOTE")          return Input_Key::QUOTE;
    if (str == "DOUBLE_QUOTE")   return Input_Key::DOUBLE_QUOTE;
    if (str == "APOSTROPHE")     return Input_Key::APOSTROPHE;
    if (str == "GRAVE")          return Input_Key::GRAVE;
    if (str == "TILDE")          return Input_Key::TILDE;
    if (str == "BACKSLASH")      return Input_Key::BACKSLASH;
    if (str == "BAR")            return Input_Key::BAR;
    if (str == "BRACKET_LEFT")   return Input_Key::BRACKET_LEFT;
    if (str == "BRACKET_RIGHT")  return Input_Key::BRACKET_RIGHT;
    if (str == "BRACE_LEFT")     return Input_Key::BRACE_LEFT;
    if (str == "BRACE_RIGHT")    return Input_Key::BRACE_RIGHT;
    if (str == "PAREN_LEFT")     return Input_Key::PAREN_LEFT;
    if (str == "PAREN_RIGHT")    return Input_Key::PAREN_RIGHT;
    if (str == "LESS")           return Input_Key::LESS;
    if (str == "GREATER")        return Input_Key::GREATER;
    if (str == "EQUAL")          return Input_Key::EQUAL;
    if (str == "PERCENT")        return Input_Key::PERCENT;
    if (str == "DOLLAR")         return Input_Key::DOLLAR;
    if (str == "HASH")           return Input_Key::HASH;
    if (str == "AT")             return Input_Key::AT;
    if (str == "CARET")          return Input_Key::CARET;
    if (str == "AMPERSAND")      return Input_Key::AMPERSAND;
    if (str == "UNDERSCORE")     return Input_Key::UNDERSCORE;

    // Calculator-specific
    if (str == "CALC_CLEAR")     return Input_Key::CALC_CLEAR;
    if (str == "CALC_ENTER")     return Input_Key::CALC_ENTER;

    return Input_Key::NONE;
}

/********************************************/
/*      Convert Input Key to String         */
/********************************************/
std::string input_key_to_string(Input_Key key) {
    switch (key) {
        case Input_Key::NONE:               return "NONE";
        case Input_Key::ESCAPE:             return "ESCAPE";
        case Input_Key::TAB:                return "TAB";
        case Input_Key::BACKSPACE:          return "BACKSPACE";
        case Input_Key::RETURN:             return "RETURN";
        case Input_Key::SPACE:              return "SPACE";
        case Input_Key::SHIFT:              return "SHIFT";
        case Input_Key::CONTROL:            return "CONTROL";
        case Input_Key::ALT:                return "ALT";
        case Input_Key::META:               return "META";
        case Input_Key::LEFT:               return "LEFT";
        case Input_Key::RIGHT:              return "RIGHT";
        case Input_Key::UP:                 return "UP";
        case Input_Key::DOWN:               return "DOWN";
        case Input_Key::HOME:               return "HOME";
        case Input_Key::END:                return "END";
        case Input_Key::PAGE_UP:            return "PAGE_UP";
        case Input_Key::PAGE_DOWN:          return "PAGE_DOWN";
        case Input_Key::INSERT:             return "INSERT";
        case Input_Key::DELETE:             return "DELETE";
        case Input_Key::F1:                 return "F1";
        case Input_Key::F2:                 return "F2";
        case Input_Key::F3:                 return "F3";
        case Input_Key::F4:                 return "F4";
        case Input_Key::F5:                 return "F5";
        case Input_Key::F6:                 return "F6";
        case Input_Key::F7:                 return "F7";
        case Input_Key::F8:                 return "F8";
        case Input_Key::F9:                 return "F9";
        case Input_Key::F10:                return "F10";
        case Input_Key::F11:                return "F11";
        case Input_Key::F12:                return "F12";
        case Input_Key::KEY_0:              return "KEY_0";
        case Input_Key::KEY_1:              return "KEY_1";
        case Input_Key::KEY_2:              return "KEY_2";
        case Input_Key::KEY_3:              return "KEY_3";
        case Input_Key::KEY_4:              return "KEY_4";
        case Input_Key::KEY_5:              return "KEY_5";
        case Input_Key::KEY_6:              return "KEY_6";
        case Input_Key::KEY_7:              return "KEY_7";
        case Input_Key::KEY_8:              return "KEY_8";
        case Input_Key::KEY_9:              return "KEY_9";
        case Input_Key::KEY_A:              return "KEY_A";
        case Input_Key::KEY_B:              return "KEY_B";
        case Input_Key::KEY_C:              return "KEY_C";
        case Input_Key::KEY_D:              return "KEY_D";
        case Input_Key::KEY_E:              return "KEY_E";
        case Input_Key::KEY_F:              return "KEY_F";
        case Input_Key::KEY_G:              return "KEY_G";
        case Input_Key::KEY_H:              return "KEY_H";
        case Input_Key::KEY_I:              return "KEY_I";
        case Input_Key::KEY_J:              return "KEY_J";
        case Input_Key::KEY_K:              return "KEY_K";
        case Input_Key::KEY_L:              return "KEY_L";
        case Input_Key::KEY_M:              return "KEY_M";
        case Input_Key::KEY_N:              return "KEY_N";
        case Input_Key::KEY_O:              return "KEY_O";
        case Input_Key::KEY_P:              return "KEY_P";
        case Input_Key::KEY_Q:              return "KEY_Q";
        case Input_Key::KEY_R:              return "KEY_R";
        case Input_Key::KEY_S:              return "KEY_S";
        case Input_Key::KEY_T:              return "KEY_T";
        case Input_Key::KEY_U:              return "KEY_U";
        case Input_Key::KEY_V:              return "KEY_V";
        case Input_Key::KEY_W:              return "KEY_W";
        case Input_Key::KEY_X:              return "KEY_X";
        case Input_Key::KEY_Y:              return "KEY_Y";
        case Input_Key::KEY_Z:              return "KEY_Z";
        case Input_Key::NUMPAD_0:           return "NUMPAD_0";
        case Input_Key::NUMPAD_1:           return "NUMPAD_1";
        case Input_Key::NUMPAD_2:           return "NUMPAD_2";
        case Input_Key::NUMPAD_3:           return "NUMPAD_3";
        case Input_Key::NUMPAD_4:           return "NUMPAD_4";
        case Input_Key::NUMPAD_5:           return "NUMPAD_5";
        case Input_Key::NUMPAD_6:           return "NUMPAD_6";
        case Input_Key::NUMPAD_7:           return "NUMPAD_7";
        case Input_Key::NUMPAD_8:           return "NUMPAD_8";
        case Input_Key::NUMPAD_9:           return "NUMPAD_9";
        case Input_Key::NUMPAD_ADD:         return "NUMPAD_ADD";
        case Input_Key::NUMPAD_SUBTRACT:    return "NUMPAD_SUBTRACT";
        case Input_Key::NUMPAD_MULTIPLY:    return "NUMPAD_MULTIPLY";
        case Input_Key::NUMPAD_DIVIDE:      return "NUMPAD_DIVIDE";
        case Input_Key::NUMPAD_DECIMAL:     return "NUMPAD_DECIMAL";
        case Input_Key::NUMPAD_ENTER:       return "NUMPAD_ENTER";
        case Input_Key::PLUS:               return "PLUS";
        case Input_Key::MINUS:              return "MINUS";
        case Input_Key::ASTERISK:           return "ASTERISK";
        case Input_Key::SLASH:              return "SLASH";
        case Input_Key::PERIOD:             return "PERIOD";
        case Input_Key::COMMA:              return "COMMA";
        case Input_Key::SEMICOLON:          return "SEMICOLON";
        case Input_Key::COLON:              return "COLON";
        case Input_Key::EXCLAMATION:        return "EXCLAMATION";
        case Input_Key::QUESTION:           return "QUESTION";
        case Input_Key::QUOTE:              return "QUOTE";
        case Input_Key::DOUBLE_QUOTE:       return "DOUBLE_QUOTE";
        case Input_Key::APOSTROPHE:         return "APOSTROPHE";
        case Input_Key::GRAVE:              return "GRAVE";
        case Input_Key::TILDE:              return "TILDE";
        case Input_Key::BACKSLASH:          return "BACKSLASH";
        case Input_Key::BAR:                return "BAR";
        case Input_Key::BRACKET_LEFT:       return "BRACKET_LEFT";
        case Input_Key::BRACKET_RIGHT:      return "BRACKET_RIGHT";
        case Input_Key::BRACE_LEFT:         return "BRACE_LEFT";
        case Input_Key::BRACE_RIGHT:        return "BRACE_RIGHT";
        case Input_Key::PAREN_LEFT:         return "PAREN_LEFT";
        case Input_Key::PAREN_RIGHT:        return "PAREN_RIGHT";
        case Input_Key::LESS:               return "LESS";
        case Input_Key::GREATER:            return "GREATER";
        case Input_Key::EQUAL:              return "EQUAL";
        case Input_Key::PERCENT:            return "PERCENT";
        case Input_Key::DOLLAR:             return "DOLLAR";
        case Input_Key::HASH:               return "HASH";
        case Input_Key::AT:                 return "AT";
        case Input_Key::CARET:              return "CARET";
        case Input_Key::AMPERSAND:          return "AMPERSAND";
        case Input_Key::UNDERSCORE:         return "UNDERSCORE";
        case Input_Key::CALC_CLEAR:         return "CALC_CLEAR";
        case Input_Key::CALC_ENTER:         return "CALC_ENTER";
    }
    return "UNKNOWN";
}

} // namespace ovb::core

