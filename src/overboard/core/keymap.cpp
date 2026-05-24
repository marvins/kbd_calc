/**
 * @file   keymap.cpp
 * @author Marvin
 * @date   2025-10-18
 *
 * @brief  Keymap implementation
 */
// C++ Standard Libraries
#include <fstream>
#include <string>

// Third-Party Libraries
#include <nlohmann/json.hpp>

// Project Libraries
#include <overboard/core/keymap.hpp>

namespace ovb::core {

/*****************************************/
/*              Constructor              */
/*****************************************/
Keymap::Keymap(const std::array<Layer, LAYER_COUNT>& layers) {
    m_layers = layers;
}

/*****************************************/
/*              Getters                  */
/*****************************************/
constexpr const Layer& Keymap::get_layer(std::size_t index) const {
    return m_layers[index];
}

const Key_Def& Keymap::get_key(std::size_t layer, std::size_t key_index) const {
    return m_layers[layer].keys[key_index];
}

} // namespace ovb::core


/*****************************************/
/*              Utilities                */
/*****************************************/
std::string label_string(Key_Label lbl) {
    switch (lbl) {
        case Key_Label::NONE:                return "";

        // Numbers
        case Key_Label::D_0:                 return "0";
        case Key_Label::D_1:                 return "1";
        case Key_Label::D_2:                 return "2";
        case Key_Label::D_3:                 return "3";
        case Key_Label::D_4:                 return "4";
        case Key_Label::D_5:                 return "5";
        case Key_Label::D_6:                 return "6";
        case Key_Label::D_7:                 return "7";
        case Key_Label::D_8:                 return "8";
        case Key_Label::D_9:                 return "9";
        case Key_Label::HEX_A:               return "A";
        case Key_Label::HEX_B:               return "B";
        case Key_Label::HEX_C:               return "C";
        case Key_Label::HEX_D:               return "D";
        case Key_Label::HEX_E:               return "E";
        case Key_Label::HEX_F:               return "F";

        // Operators
        case Key_Label::ADD:                 return "+";
        case Key_Label::SUBTRACT:            return "-";
        case Key_Label::MULTIPLY:            return "\xC3\x97";   // ×
        case Key_Label::DIVIDE:              return "\xC3\xB7";   // ÷
        case Key_Label::EQUALS:              return "=";
        case Key_Label::DECIMAL:             return ".";
        case Key_Label::PERCENT:             return "%";
        case Key_Label::BACKSPACE:           return "BSP";
        case Key_Label::CLEAR:               return "CLR";
        case Key_Label::ALL_CLEAR:           return "AC";
        case Key_Label::NEGATE:              return "+/-";
        case Key_Label::PAREN_OPEN:          return "(";
        case Key_Label::PAREN_CLOSE:         return ")";
        case Key_Label::CURSOR_LEFT:         return "<";
        case Key_Label::CURSOR_RIGHT:        return ">";
        case Key_Label::CURSOR_UP:           return "^";
        case Key_Label::CURSOR_DOWN:         return "v";
        case Key_Label::SIN:                 return "sin";
        case Key_Label::COS:                 return "cos";
        case Key_Label::TAN:                 return "tan";
        case Key_Label::ASIN:                return "asin";
        case Key_Label::ACOS:                return "acos";
        case Key_Label::ATAN:                return "atan";
        case Key_Label::LOG:                 return "log";
        case Key_Label::LN:                  return "ln";
        case Key_Label::EXP:                 return "exp";
        case Key_Label::SQRT:                return "\xE2\x88\x9A"; // √
        case Key_Label::FACTORIAL:           return "n!";
        case Key_Label::RECIPROCAL:          return "1/x";
        case Key_Label::PI:                  return "\xCF\x80";     // π
        case Key_Label::EULER:               return "e";
        case Key_Label::POWER_2:             return "x\xC2\xB2";   // x²
        case Key_Label::POWER_N:             return "x\xCA\xB8";   // xʸ
        case Key_Label::BIT_AND:             return "AND";
        case Key_Label::BIT_OR:              return "OR";
        case Key_Label::BIT_XOR:             return "XOR";
        case Key_Label::BIT_NOT:             return "NOT";
        case Key_Label::SHIFT_LEFT:          return "<<";
        case Key_Label::SHIFT_RIGHT:         return ">>";
        case Key_Label::LAYER_TRIG:          return "TRG";
        case Key_Label::LAYER_CONST:         return "CST";
        case Key_Label::LAYER_VAR:           return "Var";
        case Key_Label::PHI:                 return "\xCF\x86";   // φ
        case Key_Label::TAU:                 return "\xCF\x84";   // τ
        case Key_Label::LAYER_ALGEBRA:       return "ALG";
        case Key_Label::LAYER_HOME:          return "Home";
        case Key_Label::APPROX:              return "Aprx";
        case Key_Label::EVAL:                return "Eval";
        case Key_Label::MATH_LAYOUT:         return "MATH";
        case Key_Label::PG_UP:               return "PgUp";
        case Key_Label::PG_DN:               return "PgDn";
    }
    return "";
}

// Helper function to convert string to Key_Code
Key_Code string_to_key_code(const std::string& str) {
    // Digits
    if (str == "DIGIT_0") return Key_Code::DIGIT_0;
    if (str == "DIGIT_1") return Key_Code::DIGIT_1;
    if (str == "DIGIT_2") return Key_Code::DIGIT_2;
    if (str == "DIGIT_3") return Key_Code::DIGIT_3;
    if (str == "DIGIT_4") return Key_Code::DIGIT_4;
    if (str == "DIGIT_5") return Key_Code::DIGIT_5;
    if (str == "DIGIT_6") return Key_Code::DIGIT_6;
    if (str == "DIGIT_7") return Key_Code::DIGIT_7;
    if (str == "DIGIT_8") return Key_Code::DIGIT_8;
    if (str == "DIGIT_9") return Key_Code::DIGIT_9;

    // Basic ops
    if (str == "ADD") return Key_Code::ADD;
    if (str == "SUBTRACT") return Key_Code::SUBTRACT;
    if (str == "MULTIPLY") return Key_Code::MULTIPLY;
    if (str == "DIVIDE") return Key_Code::DIVIDE;
    if (str == "EQUALS") return Key_Code::EQUALS;
    if (str == "EVAL") return Key_Code::EVAL;
    if (str == "APPROX") return Key_Code::APPROX;
    if (str == "DECIMAL") return Key_Code::DECIMAL;
    if (str == "CLEAR") return Key_Code::CLEAR;
    if (str == "ALL_CLEAR") return Key_Code::ALL_CLEAR;
    if (str == "BACKSPACE") return Key_Code::BACKSPACE;

    // Grouping / extra basic
    if (str == "PAREN_OPEN") return Key_Code::PAREN_OPEN;
    if (str == "PAREN_CLOSE") return Key_Code::PAREN_CLOSE;
    if (str == "PERCENT") return Key_Code::PERCENT;
    if (str == "NEGATE") return Key_Code::NEGATE;

    // Memory
    if (str == "MEM_STORE") return Key_Code::MEM_STORE;
    if (str == "MEM_RECALL") return Key_Code::MEM_RECALL;
    if (str == "MEM_ADD") return Key_Code::MEM_ADD;
    if (str == "MEM_CLEAR") return Key_Code::MEM_CLEAR;

    // Scientific
    if (str == "SIN") return Key_Code::SIN;
    if (str == "COS") return Key_Code::COS;
    if (str == "TAN") return Key_Code::TAN;
    if (str == "ASIN") return Key_Code::ASIN;
    if (str == "ACOS") return Key_Code::ACOS;
    if (str == "ATAN") return Key_Code::ATAN;
    if (str == "LOG") return Key_Code::LOG;
    if (str == "LN") return Key_Code::LN;
    if (str == "EXP") return Key_Code::EXP;
    if (str == "SQRT") return Key_Code::SQRT;
    if (str == "POWER_2") return Key_Code::POWER_2;
    if (str == "POWER_3") return Key_Code::POWER_3;
    if (str == "POWER_N") return Key_Code::POWER_N;
    if (str == "FACTORIAL") return Key_Code::FACTORIAL;
    if (str == "RECIPROCAL") return Key_Code::RECIPROCAL;
    if (str == "PI") return Key_Code::PI;
    if (str == "EULER") return Key_Code::EULER;

    // Programmer
    if (str == "BIT_AND") return Key_Code::BIT_AND;
    if (str == "BIT_OR") return Key_Code::BIT_OR;
    if (str == "BIT_XOR") return Key_Code::BIT_XOR;
    if (str == "BIT_NOT") return Key_Code::BIT_NOT;
    if (str == "SHIFT_LEFT") return Key_Code::SHIFT_LEFT;
    if (str == "SHIFT_RIGHT") return Key_Code::SHIFT_RIGHT;
    if (str == "HEX_A") return Key_Code::HEX_A;
    if (str == "HEX_B") return Key_Code::HEX_B;
    if (str == "HEX_C") return Key_Code::HEX_C;
    if (str == "HEX_D") return Key_Code::HEX_D;
    if (str == "HEX_E") return Key_Code::HEX_E;
    if (str == "HEX_F") return Key_Code::HEX_F;

    // Constants
    if (str == "PHI") return Key_Code::PHI;
    if (str == "TAU") return Key_Code::TAU;

    // Meta / control
    if (str == "LAYER_NEXT") return Key_Code::LAYER_NEXT;
    if (str == "LAYER_PREV") return Key_Code::LAYER_PREV;
    if (str == "LAYER_CONST") return Key_Code::LAYER_CONST;
    if (str == "LAYER_ALG") return Key_Code::LAYER_ALG;
    if (str == "LAYER_TRIG") return Key_Code::LAYER_TRIG;
    if (str == "LAYER_VAR") return Key_Code::LAYER_VAR;
    if (str == "LAYER_HOME") return Key_Code::LAYER_HOME;

    // Display mode
    if (str == "TOGGLE_MATH_LAYOUT") return Key_Code::TOGGLE_MATH_LAYOUT;

    // Navigation
    if (str == "CURSOR_LEFT") return Key_Code::CURSOR_LEFT;
    if (str == "CURSOR_RIGHT") return Key_Code::CURSOR_RIGHT;
    if (str == "CURSOR_UP") return Key_Code::CURSOR_UP;
    if (str == "CURSOR_DOWN") return Key_Code::CURSOR_DOWN;
    if (str == "PAGE_UP") return Key_Code::PAGE_UP;
    if (str == "PAGE_DOWN") return Key_Code::PAGE_DOWN;

    return Key_Code::NONE;
}

/*************************************************/
/*          Convert String to Key Label          */
/*************************************************/
Key_Label string_to_key_label(const std::string& str) {
    // Digits
    if (str == "D_0") return Key_Label::D_0;
    if (str == "D_1") return Key_Label::D_1;
    if (str == "D_2") return Key_Label::D_2;
    if (str == "D_3") return Key_Label::D_3;
    if (str == "D_4") return Key_Label::D_4;
    if (str == "D_5") return Key_Label::D_5;
    if (str == "D_6") return Key_Label::D_6;
    if (str == "D_7") return Key_Label::D_7;
    if (str == "D_8") return Key_Label::D_8;
    if (str == "D_9") return Key_Label::D_9;

    // Hex digits
    if (str == "HEX_A") return Key_Label::HEX_A;
    if (str == "HEX_B") return Key_Label::HEX_B;
    if (str == "HEX_C") return Key_Label::HEX_C;
    if (str == "HEX_D") return Key_Label::HEX_D;
    if (str == "HEX_E") return Key_Label::HEX_E;
    if (str == "HEX_F") return Key_Label::HEX_F;

    // Arithmetic
    if (str == "ADD") return Key_Label::ADD;
    if (str == "SUBTRACT") return Key_Label::SUBTRACT;
    if (str == "MULTIPLY") return Key_Label::MULTIPLY;
    if (str == "DIVIDE") return Key_Label::DIVIDE;
    if (str == "EQUALS") return Key_Label::EQUALS;
    if (str == "DECIMAL") return Key_Label::DECIMAL;
    if (str == "PERCENT") return Key_Label::PERCENT;

    // Editing
    if (str == "BACKSPACE") return Key_Label::BACKSPACE;
    if (str == "CLEAR") return Key_Label::CLEAR;
    if (str == "ALL_CLEAR") return Key_Label::ALL_CLEAR;
    if (str == "NEGATE") return Key_Label::NEGATE;
    if (str == "PAREN_OPEN") return Key_Label::PAREN_OPEN;
    if (str == "PAREN_CLOSE") return Key_Label::PAREN_CLOSE;

    // Cursor
    if (str == "CURSOR_LEFT") return Key_Label::CURSOR_LEFT;
    if (str == "CURSOR_RIGHT") return Key_Label::CURSOR_RIGHT;
    if (str == "CURSOR_UP") return Key_Label::CURSOR_UP;
    if (str == "CURSOR_DOWN") return Key_Label::CURSOR_DOWN;

    // Scientific
    if (str == "SIN") return Key_Label::SIN;
    if (str == "COS") return Key_Label::COS;
    if (str == "TAN") return Key_Label::TAN;
    if (str == "ASIN") return Key_Label::ASIN;
    if (str == "ACOS") return Key_Label::ACOS;
    if (str == "ATAN") return Key_Label::ATAN;
    if (str == "LOG") return Key_Label::LOG;
    if (str == "LN") return Key_Label::LN;
    if (str == "EXP") return Key_Label::EXP;
    if (str == "SQRT") return Key_Label::SQRT;
    if (str == "FACTORIAL") return Key_Label::FACTORIAL;
    if (str == "RECIPROCAL") return Key_Label::RECIPROCAL;
    if (str == "PI") return Key_Label::PI;
    if (str == "EULER") return Key_Label::EULER;
    if (str == "PHI") return Key_Label::PHI;
    if (str == "TAU") return Key_Label::TAU;
    if (str == "POWER_2") return Key_Label::POWER_2;
    if (str == "POWER_N") return Key_Label::POWER_N;

    // Programmer
    if (str == "BIT_AND") return Key_Label::BIT_AND;
    if (str == "BIT_OR") return Key_Label::BIT_OR;
    if (str == "BIT_XOR") return Key_Label::BIT_XOR;
    if (str == "BIT_NOT") return Key_Label::BIT_NOT;
    if (str == "SHIFT_LEFT") return Key_Label::SHIFT_LEFT;
    if (str == "SHIFT_RIGHT") return Key_Label::SHIFT_RIGHT;

    // Layer / meta
    if (str == "LAYER_TRIG") return Key_Label::LAYER_TRIG;
    if (str == "LAYER_ALGEBRA") return Key_Label::LAYER_ALGEBRA;
    if (str == "LAYER_CONST") return Key_Label::LAYER_CONST;
    if (str == "LAYER_VAR") return Key_Label::LAYER_VAR;
    if (str == "LAYER_HOME") return Key_Label::LAYER_HOME;
    if (str == "APPROX") return Key_Label::APPROX;
    if (str == "EVAL") return Key_Label::EVAL;

    // Display mode
    if (str == "MATH_LAYOUT") return Key_Label::MATH_LAYOUT;

    // Navigation
    if (str == "PG_UP") return Key_Label::PG_UP;
    if (str == "PG_DN") return Key_Label::PG_DN;

    return Key_Label::NONE;
}

std::array<Layer, Keymap::LAYER_COUNT> load_layers_from_json(const std::string& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open keymap JSON file: " + json_path);
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse keymap JSON: " + std::string(e.what()));
    }

    if (!j.contains("layers") || !j["layers"].is_array()) {
        throw std::runtime_error("Invalid keymap JSON: missing or invalid 'layers' array");
    }

    auto layers_json = j["layers"];
    if (layers_json.size() != Keymap::LAYER_COUNT) {
        throw std::runtime_error("Invalid keymap JSON: expected " + std::to_string(Keymap::LAYER_COUNT) +
                                 " layers, got " + std::to_string(layers_json.size()));
    }

    std::array<Layer, Keymap::LAYER_COUNT> layers;

    for (size_t i = 0; i < layers_json.size(); ++i) {
        auto layer_json = layers_json[i];

        if (!layer_json.contains("name") || !layer_json["name"].is_string()) {
            throw std::runtime_error("Invalid layer at index " + std::to_string(i) + ": missing or invalid 'name'");
        }

        if (!layer_json.contains("keys") || !layer_json["keys"].is_array()) {
            throw std::runtime_error("Invalid layer at index " + std::to_string(i) + ": missing or invalid 'keys' array");
        }

        auto keys_json = layer_json["keys"];

        // Set layer name from JSON
        layers[i].name = layer_json["name"].get<std::string>();

        // Build keys from JSON
        layers[i].keys.clear();
        layers[i].keys.reserve(keys_json.size());

        for (const auto& key_json : keys_json) {
            Key_Def key_def;
            if (key_json.contains("code") && key_json["code"].is_string()) {
                key_def.code = string_to_key_code(key_json["code"].get<std::string>());
            } else {
                key_def.code = Key_Code::NONE;
            }
            if (key_json.contains("label") && key_json["label"].is_string()) {
                key_def.label = string_to_key_label(key_json["label"].get<std::string>());
            } else {
                key_def.label = Key_Label::NONE;
            }
            layers[i].keys.push_back(key_def);
        }
    }

    return layers;
}
