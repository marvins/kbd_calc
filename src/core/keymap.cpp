#include "keymap.hpp"
#include <string>

std::string label_string(Key_Label lbl) {
    switch (lbl) {
        case Key_Label::NONE:           return "";
        case Key_Label::D_0:            return "0";
        case Key_Label::D_1:            return "1";
        case Key_Label::D_2:            return "2";
        case Key_Label::D_3:            return "3";
        case Key_Label::D_4:            return "4";
        case Key_Label::D_5:            return "5";
        case Key_Label::D_6:            return "6";
        case Key_Label::D_7:            return "7";
        case Key_Label::D_8:            return "8";
        case Key_Label::D_9:            return "9";
        case Key_Label::HEX_A:          return "A";
        case Key_Label::HEX_B:          return "B";
        case Key_Label::HEX_C:          return "C";
        case Key_Label::HEX_D:          return "D";
        case Key_Label::HEX_E:          return "E";
        case Key_Label::HEX_F:          return "F";
        case Key_Label::ADD:            return "+";
        case Key_Label::SUBTRACT:       return "-";
        case Key_Label::MULTIPLY:       return "\xC3\x97";   // ×
        case Key_Label::DIVIDE:         return "\xC3\xB7";   // ÷
        case Key_Label::EQUALS:         return "=";
        case Key_Label::DECIMAL:        return ".";
        case Key_Label::PERCENT:        return "%";
        case Key_Label::BACKSPACE:      return "BSP";
        case Key_Label::CLEAR:          return "CLR";
        case Key_Label::ALL_CLEAR:      return "AC";
        case Key_Label::NEGATE:         return "+/-";
        case Key_Label::PAREN_OPEN:     return "(";
        case Key_Label::PAREN_CLOSE:    return ")";
        case Key_Label::CURSOR_LEFT:    return "<";
        case Key_Label::CURSOR_RIGHT:   return ">";
        case Key_Label::SIN:            return "sin";
        case Key_Label::COS:            return "cos";
        case Key_Label::TAN:            return "tan";
        case Key_Label::ASIN:           return "asin";
        case Key_Label::ACOS:           return "acos";
        case Key_Label::ATAN:           return "atan";
        case Key_Label::LOG:            return "log";
        case Key_Label::LN:             return "ln";
        case Key_Label::EXP:            return "exp";
        case Key_Label::SQRT:           return "\xE2\x88\x9A"; // √
        case Key_Label::FACTORIAL:      return "n!";
        case Key_Label::RECIPROCAL:     return "1/x";
        case Key_Label::PI:             return "\xCF\x80";     // π
        case Key_Label::EULER:          return "e";
        case Key_Label::POWER_2:        return "x\xC2\xB2";   // x²
        case Key_Label::POWER_N:        return "x\xCA\xB8";   // xʸ
        case Key_Label::BIT_AND:        return "AND";
        case Key_Label::BIT_OR:         return "OR";
        case Key_Label::BIT_XOR:        return "XOR";
        case Key_Label::BIT_NOT:        return "NOT";
        case Key_Label::SHIFT_LEFT:     return "<<";
        case Key_Label::SHIFT_RIGHT:    return ">>";
        case Key_Label::LAYER_NEXT_TRIG:  return "Trig";
        case Key_Label::PHI:              return "\xCF\x86";   // φ
        case Key_Label::TAU:              return "\xCF\x84";   // τ
        case Key_Label::LAYER_NEXT_CONST:    return "CST";
        case Key_Label::LAYER_NEXT_ALGEBRA:  return "ALG";
        case Key_Label::LAYER_PREV:       return "BACK";
        case Key_Label::LAYER_PREV_CONST: return "BACK";
        case Key_Label::APPROX:           return "\xE2\x89\x88";  // ≈
    }
    return "";
}

