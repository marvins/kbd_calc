/**
 * @file    action_code.cpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Action code implementation
 */
#include <overboard/core/action_code.hpp>

namespace ovb::core {

/**************************************/
/*      String to Action Code        */
/**************************************/
Action_Code string_to_action_code(const std::string& str) {
    if (str == "NONE")             return Action_Code::NONE;

    // Digits
    if (str == "DIGIT_0")          return Action_Code::DIGIT_0;
    if (str == "DIGIT_1")          return Action_Code::DIGIT_1;
    if (str == "DIGIT_2")          return Action_Code::DIGIT_2;
    if (str == "DIGIT_3")          return Action_Code::DIGIT_3;
    if (str == "DIGIT_4")          return Action_Code::DIGIT_4;
    if (str == "DIGIT_5")          return Action_Code::DIGIT_5;
    if (str == "DIGIT_6")          return Action_Code::DIGIT_6;
    if (str == "DIGIT_7")          return Action_Code::DIGIT_7;
    if (str == "DIGIT_8")          return Action_Code::DIGIT_8;
    if (str == "DIGIT_9")          return Action_Code::DIGIT_9;
    if (str == "DECIMAL")          return Action_Code::DECIMAL;

    // Basic operators
    if (str == "ADD")              return Action_Code::ADD;
    if (str == "SUBTRACT")         return Action_Code::SUBTRACT;
    if (str == "MULTIPLY")         return Action_Code::MULTIPLY;
    if (str == "DIVIDE")           return Action_Code::DIVIDE;
    if (str == "EQUALS")           return Action_Code::EQUALS;

    // Evaluation
    if (str == "EVAL")             return Action_Code::EVAL;
    if (str == "APPROX")           return Action_Code::APPROX;

    // Basic arithmetic
    if (str == "NEGATE")           return Action_Code::NEGATE;
    if (str == "PERCENT")          return Action_Code::PERCENT;

    // Scientific functions
    if (str == "SIN")              return Action_Code::SIN;
    if (str == "COS")              return Action_Code::COS;
    if (str == "TAN")              return Action_Code::TAN;
    if (str == "ASIN")             return Action_Code::ASIN;
    if (str == "ACOS")             return Action_Code::ACOS;
    if (str == "ATAN")             return Action_Code::ATAN;
    if (str == "ATAN2")            return Action_Code::ATAN2;
    if (str == "LOG")              return Action_Code::LOG;
    if (str == "LN")               return Action_Code::LN;
    if (str == "EXP")              return Action_Code::EXP;
    if (str == "SQRT")             return Action_Code::SQRT;
    if (str == "POWER_2")          return Action_Code::POWER_2;
    if (str == "POWER_3")          return Action_Code::POWER_3;
    if (str == "POWER_N")          return Action_Code::POWER_N;
    if (str == "FACTORIAL")        return Action_Code::FACTORIAL;
    if (str == "RECIPROCAL")       return Action_Code::RECIPROCAL;

    // Constants
    if (str == "PI")               return Action_Code::PI;
    if (str == "EULER")            return Action_Code::EULER;
    if (str == "PHI")              return Action_Code::PHI;
    if (str == "TAU")              return Action_Code::TAU;

    // Additional scientific
    if (str == "CEIL")             return Action_Code::CEIL;
    if (str == "FLOOR")            return Action_Code::FLOOR;
    if (str == "ABS")              return Action_Code::ABS;

    // Programmer mode
    if (str == "BIT_AND")          return Action_Code::BIT_AND;
    if (str == "BIT_OR")           return Action_Code::BIT_OR;
    if (str == "BIT_XOR")          return Action_Code::BIT_XOR;
    if (str == "BIT_NOT")          return Action_Code::BIT_NOT;
    if (str == "SHIFT_LEFT")       return Action_Code::SHIFT_LEFT;
    if (str == "SHIFT_RIGHT")      return Action_Code::SHIFT_RIGHT;
    if (str == "HEX_A")            return Action_Code::HEX_A;
    if (str == "HEX_B")            return Action_Code::HEX_B;
    if (str == "HEX_C")            return Action_Code::HEX_C;
    if (str == "HEX_D")            return Action_Code::HEX_D;
    if (str == "HEX_E")            return Action_Code::HEX_E;
    if (str == "HEX_F")            return Action_Code::HEX_F;

    // Memory
    if (str == "MEM_STORE")        return Action_Code::MEM_STORE;
    if (str == "MEM_RECALL")       return Action_Code::MEM_RECALL;
    if (str == "MEM_ADD")          return Action_Code::MEM_ADD;
    if (str == "MEM_CLEAR")        return Action_Code::MEM_CLEAR;

    // Navigation actions
    if (str == "CURSOR_HOME")      return Action_Code::CURSOR_HOME;
    if (str == "CURSOR_END")       return Action_Code::CURSOR_END;
    if (str == "CURSOR_LEFT_WORD") return Action_Code::CURSOR_LEFT_WORD;
    if (str == "CURSOR_RIGHT_WORD") return Action_Code::CURSOR_RIGHT_WORD;

    // Layer actions
    if (str == "GO_HOME_LAYER")    return Action_Code::GO_HOME_LAYER;
    if (str == "GO_CONST_LAYER")   return Action_Code::GO_CONST_LAYER;
    if (str == "GO_ALG_LAYER")     return Action_Code::GO_ALG_LAYER;
    if (str == "GO_TRIG_LAYER")    return Action_Code::GO_TRIG_LAYER;
    if (str == "GO_VAR_LAYER")     return Action_Code::GO_VAR_LAYER;
    if (str == "NEXT_LAYER")       return Action_Code::NEXT_LAYER;
    if (str == "PREV_LAYER")       return Action_Code::PREV_LAYER;

    // Grouping
    if (str == "PAREN_OPEN")       return Action_Code::PAREN_OPEN;
    if (str == "PAREN_CLOSE")      return Action_Code::PAREN_CLOSE;

    // System
    if (str == "ESCAPE")           return Action_Code::ESCAPE;
    if (str == "TOGGLE_MATH_LAYOUT") return Action_Code::TOGGLE_MATH_LAYOUT;
    if (str == "NONE_ACTION")      return Action_Code::NONE_ACTION;

    // Navigation
    if (str == "CURSOR_LEFT")      return Action_Code::CURSOR_LEFT;
    if (str == "CURSOR_RIGHT")     return Action_Code::CURSOR_RIGHT;
    if (str == "CURSOR_UP")        return Action_Code::CURSOR_UP;
    if (str == "CURSOR_DOWN")      return Action_Code::CURSOR_DOWN;
    if (str == "PAGE_UP")          return Action_Code::PAGE_UP;
    if (str == "PAGE_DOWN")        return Action_Code::PAGE_DOWN;

    // Editing
    if (str == "BACKSPACE")        return Action_Code::BACKSPACE;
    if (str == "TAB")              return Action_Code::TAB;
    if (str == "DELETE")           return Action_Code::DELETE;
    if (str == "CAPS_LOCK")        return Action_Code::CAPS_LOCK;
    if (str == "CLEAR")            return Action_Code::CLEAR;
    if (str == "ALL_CLEAR")        return Action_Code::ALL_CLEAR;

    // Function keys
    if (str == "FUNC_1")           return Action_Code::FUNC_1;
    if (str == "FUNC_2")           return Action_Code::FUNC_2;
    if (str == "FUNC_3")           return Action_Code::FUNC_3;
    if (str == "FUNC_4")           return Action_Code::FUNC_4;
    if (str == "FUNC_5")           return Action_Code::FUNC_5;
    if (str == "FUNC_6")           return Action_Code::FUNC_6;
    if (str == "FUNC_7")           return Action_Code::FUNC_7;
    if (str == "FUNC_8")           return Action_Code::FUNC_8;
    if (str == "FUNC_9")           return Action_Code::FUNC_9;
    if (str == "FUNC_10")          return Action_Code::FUNC_10;

    return Action_Code::NONE;
}

/********************************************/
/*      Convert Action Code to Display     */
/********************************************/
std::string action_code_to_display(Action_Code code) {
    switch (code) {
        case Action_Code::NONE:             return "";

        // Digits
        case Action_Code::DIGIT_0:          return "0";
        case Action_Code::DIGIT_1:          return "1";
        case Action_Code::DIGIT_2:          return "2";
        case Action_Code::DIGIT_3:          return "3";
        case Action_Code::DIGIT_4:          return "4";
        case Action_Code::DIGIT_5:          return "5";
        case Action_Code::DIGIT_6:          return "6";
        case Action_Code::DIGIT_7:          return "7";
        case Action_Code::DIGIT_8:          return "8";
        case Action_Code::DIGIT_9:          return "9";
        case Action_Code::DECIMAL:          return ".";

        // Basic operators
        case Action_Code::ADD:              return "+";
        case Action_Code::SUBTRACT:         return "-";
        case Action_Code::MULTIPLY:         return "x";
        case Action_Code::DIVIDE:           return "\xC3\xB7";  // ÷
        case Action_Code::EQUALS:           return "=";

        case Action_Code::EVAL:             return "Eval";
        case Action_Code::APPROX:           return "Aprx";
        case Action_Code::NEGATE:           return "+/-";
        case Action_Code::PERCENT:          return "%";
        case Action_Code::SIN:              return "sin";
        case Action_Code::COS:              return "cos";
        case Action_Code::TAN:              return "tan";
        case Action_Code::ASIN:             return "asin";
        case Action_Code::ACOS:             return "acos";
        case Action_Code::ATAN:             return "atan";
        case Action_Code::ATAN2:            return "atan2";
        case Action_Code::LOG:              return "log";
        case Action_Code::LN:               return "ln";
        case Action_Code::EXP:              return "e^x";
        case Action_Code::SQRT:             return "\xE2\x88\x9A";  // √
        case Action_Code::POWER_2:          return "x\xC2\xB2";   // x²
        case Action_Code::POWER_3:          return "x\xC2\xB3";   // x³
        case Action_Code::POWER_N:          return "x^n";
        case Action_Code::FACTORIAL:        return "n!";
        case Action_Code::RECIPROCAL:       return "1/x";
        case Action_Code::PI:               return "\xCF\x80";    // π
        case Action_Code::EULER:            return "e";
        case Action_Code::PHI:              return "\xCF\x86";    // φ
        case Action_Code::TAU:              return "\xCF\x84";    // τ
        case Action_Code::CEIL:             return "ceil";
        case Action_Code::FLOOR:            return "floor";
        case Action_Code::ABS:              return "abs";
        case Action_Code::BIT_AND:          return "&";
        case Action_Code::BIT_OR:           return "|";
        case Action_Code::BIT_XOR:          return "^";
        case Action_Code::BIT_NOT:          return "~";
        case Action_Code::SHIFT_LEFT:       return "<<";
        case Action_Code::SHIFT_RIGHT:      return ">>";
        case Action_Code::HEX_A:            return "A";
        case Action_Code::HEX_B:            return "B";
        case Action_Code::HEX_C:            return "C";
        case Action_Code::HEX_D:            return "D";
        case Action_Code::HEX_E:            return "E";
        case Action_Code::HEX_F:            return "F";
        case Action_Code::MEM_STORE:        return "MS";
        case Action_Code::MEM_RECALL:       return "MR";
        case Action_Code::MEM_ADD:          return "M+";
        case Action_Code::MEM_CLEAR:        return "MC";
        case Action_Code::CURSOR_HOME:      return "Home";
        case Action_Code::CURSOR_END:       return "End";
        case Action_Code::CURSOR_LEFT_WORD: return "\xE2\x8C\x98\xE2\x86\x90"; // ⌘←
        case Action_Code::CURSOR_RIGHT_WORD: return "\xE2\x8C\x98\xE2\x86\x92"; // ⌘→
        case Action_Code::GO_HOME_LAYER:    return "Home";
        case Action_Code::GO_CONST_LAYER:   return "CST";
        case Action_Code::GO_ALG_LAYER:     return "ALG";
        case Action_Code::GO_TRIG_LAYER:    return "TRG";
        case Action_Code::GO_VAR_LAYER:     return "Var";
        case Action_Code::NEXT_LAYER:       return "Next";
        case Action_Code::PREV_LAYER:       return "Prev";
        case Action_Code::ESCAPE:            return "ESC";
        case Action_Code::TOGGLE_MATH_LAYOUT: return "MATH";
        case Action_Code::NONE_ACTION:      return "";

        // Grouping
        case Action_Code::PAREN_OPEN:       return "(";
        case Action_Code::PAREN_CLOSE:      return ")";

        // Navigation
        case Action_Code::CURSOR_LEFT:      return "\xEF\x81\x93";  // LV_SYMBOL_LEFT
        case Action_Code::CURSOR_RIGHT:     return "\xEF\x81\x94";  // LV_SYMBOL_RIGHT
        case Action_Code::CURSOR_UP:        return "\xEF\x81\xB7";  // LV_SYMBOL_UP
        case Action_Code::CURSOR_DOWN:      return "\xEF\x81\xB8";  // LV_SYMBOL_DOWN
        case Action_Code::PAGE_UP:          return "PgUp";
        case Action_Code::PAGE_DOWN:        return "PgDn";

        // Editing
        case Action_Code::BACKSPACE:        return "BSP";
        case Action_Code::TAB:              return "Tab";
        case Action_Code::DELETE:           return "Del";
        case Action_Code::CAPS_LOCK:        return "Caps";
        case Action_Code::CLEAR:            return "CLR";
        case Action_Code::ALL_CLEAR:        return "AC";

        // Function keys
        case Action_Code::FUNC_1:           return "F1";
        case Action_Code::FUNC_2:           return "F2";
        case Action_Code::FUNC_3:           return "F3";
        case Action_Code::FUNC_4:           return "F4";
        case Action_Code::FUNC_5:           return "F5";
        case Action_Code::FUNC_6:           return "F6";
        case Action_Code::FUNC_7:           return "F7";
        case Action_Code::FUNC_8:           return "F8";
        case Action_Code::FUNC_9:           return "F9";
        case Action_Code::FUNC_10:          return "F10";
    }
    return "";
}

/********************************************/
/*      Convert Action Code to String       */
/********************************************/
std::string action_code_to_string(Action_Code code) {
    switch (code) {
        case Action_Code::NONE:             return "NONE";

        // Digits
        case Action_Code::DIGIT_0:          return "DIGIT_0";
        case Action_Code::DIGIT_1:          return "DIGIT_1";
        case Action_Code::DIGIT_2:          return "DIGIT_2";
        case Action_Code::DIGIT_3:          return "DIGIT_3";
        case Action_Code::DIGIT_4:          return "DIGIT_4";
        case Action_Code::DIGIT_5:          return "DIGIT_5";
        case Action_Code::DIGIT_6:          return "DIGIT_6";
        case Action_Code::DIGIT_7:          return "DIGIT_7";
        case Action_Code::DIGIT_8:          return "DIGIT_8";
        case Action_Code::DIGIT_9:          return "DIGIT_9";
        case Action_Code::DECIMAL:          return "DECIMAL";

        // Basic operators
        case Action_Code::ADD:              return "ADD";
        case Action_Code::SUBTRACT:         return "SUBTRACT";
        case Action_Code::MULTIPLY:         return "MULTIPLY";
        case Action_Code::DIVIDE:           return "DIVIDE";
        case Action_Code::EQUALS:           return "EQUALS";

        case Action_Code::EVAL:             return "EVAL";
        case Action_Code::APPROX:           return "APPROX";
        case Action_Code::NEGATE:           return "NEGATE";
        case Action_Code::PERCENT:          return "PERCENT";
        case Action_Code::SIN:              return "SIN";
        case Action_Code::COS:              return "COS";
        case Action_Code::TAN:              return "TAN";
        case Action_Code::ASIN:             return "ASIN";
        case Action_Code::ACOS:             return "ACOS";
        case Action_Code::ATAN:             return "ATAN";
        case Action_Code::ATAN2:            return "ATAN2";
        case Action_Code::LOG:              return "LOG";
        case Action_Code::LN:               return "LN";
        case Action_Code::EXP:              return "EXP";
        case Action_Code::SQRT:             return "SQRT";
        case Action_Code::POWER_2:          return "POWER_2";
        case Action_Code::POWER_3:          return "POWER_3";
        case Action_Code::POWER_N:          return "POWER_N";
        case Action_Code::FACTORIAL:        return "FACTORIAL";
        case Action_Code::RECIPROCAL:       return "RECIPROCAL";
        case Action_Code::PI:               return "PI";
        case Action_Code::EULER:            return "EULER";
        case Action_Code::PHI:              return "PHI";
        case Action_Code::TAU:              return "TAU";
        case Action_Code::CEIL:             return "CEIL";
        case Action_Code::FLOOR:            return "FLOOR";
        case Action_Code::ABS:              return "ABS";
        case Action_Code::BIT_AND:          return "BIT_AND";
        case Action_Code::BIT_OR:           return "BIT_OR";
        case Action_Code::BIT_XOR:          return "BIT_XOR";
        case Action_Code::BIT_NOT:          return "BIT_NOT";
        case Action_Code::SHIFT_LEFT:       return "SHIFT_LEFT";
        case Action_Code::SHIFT_RIGHT:      return "SHIFT_RIGHT";
        case Action_Code::HEX_A:            return "HEX_A";
        case Action_Code::HEX_B:            return "HEX_B";
        case Action_Code::HEX_C:            return "HEX_C";
        case Action_Code::HEX_D:            return "HEX_D";
        case Action_Code::HEX_E:            return "HEX_E";
        case Action_Code::HEX_F:            return "HEX_F";
        case Action_Code::MEM_STORE:        return "MEM_STORE";
        case Action_Code::MEM_RECALL:       return "MEM_RECALL";
        case Action_Code::MEM_ADD:          return "MEM_ADD";
        case Action_Code::MEM_CLEAR:        return "MEM_CLEAR";
        case Action_Code::CURSOR_HOME:      return "CURSOR_HOME";
        case Action_Code::CURSOR_END:       return "CURSOR_END";
        case Action_Code::CURSOR_LEFT_WORD: return "CURSOR_LEFT_WORD";
        case Action_Code::CURSOR_RIGHT_WORD: return "CURSOR_RIGHT_WORD";
        case Action_Code::GO_HOME_LAYER:    return "GO_HOME_LAYER";
        case Action_Code::GO_CONST_LAYER:   return "GO_CONST_LAYER";
        case Action_Code::GO_ALG_LAYER:     return "GO_ALG_LAYER";
        case Action_Code::GO_TRIG_LAYER:    return "GO_TRIG_LAYER";
        case Action_Code::GO_VAR_LAYER:     return "GO_VAR_LAYER";
        case Action_Code::NEXT_LAYER:       return "NEXT_LAYER";
        case Action_Code::PREV_LAYER:       return "PREV_LAYER";
        case Action_Code::ESCAPE:            return "ESCAPE";
        case Action_Code::TOGGLE_MATH_LAYOUT: return "TOGGLE_MATH_LAYOUT";
        case Action_Code::NONE_ACTION:      return "NONE_ACTION";

        // Grouping
        case Action_Code::PAREN_OPEN:       return "PAREN_OPEN";
        case Action_Code::PAREN_CLOSE:      return "PAREN_CLOSE";

        // Navigation
        case Action_Code::CURSOR_LEFT:      return "CURSOR_LEFT";
        case Action_Code::CURSOR_RIGHT:     return "CURSOR_RIGHT";
        case Action_Code::CURSOR_UP:        return "CURSOR_UP";
        case Action_Code::CURSOR_DOWN:      return "CURSOR_DOWN";
        case Action_Code::PAGE_UP:          return "PAGE_UP";
        case Action_Code::PAGE_DOWN:        return "PAGE_DOWN";

        // Editing
        case Action_Code::BACKSPACE:        return "BACKSPACE";
        case Action_Code::TAB:              return "TAB";
        case Action_Code::DELETE:           return "DELETE";
        case Action_Code::CAPS_LOCK:        return "CAPS_LOCK";
        case Action_Code::CLEAR:            return "CLEAR";
        case Action_Code::ALL_CLEAR:        return "ALL_CLEAR";

        // Function keys
        case Action_Code::FUNC_1:           return "FUNC_1";
        case Action_Code::FUNC_2:           return "FUNC_2";
        case Action_Code::FUNC_3:           return "FUNC_3";
        case Action_Code::FUNC_4:           return "FUNC_4";
        case Action_Code::FUNC_5:           return "FUNC_5";
        case Action_Code::FUNC_6:           return "FUNC_6";
        case Action_Code::FUNC_7:           return "FUNC_7";
        case Action_Code::FUNC_8:           return "FUNC_8";
        case Action_Code::FUNC_9:           return "FUNC_9";
        case Action_Code::FUNC_10:          return "FUNC_10";
    }
    return "UNKNOWN";
}

} // namespace ovb::core

