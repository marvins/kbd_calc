/**
 * @file   keymap.cpp
 * @author Marvin
 * @date   2025-10-18
 *
 * @brief  Keymap implementation
 */
// C++ Standard Libraries
#include <filesystem>
#include <fstream>
#include <string>

// Third-Party Libraries
#include <lvgl.h>
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
const Layer& Keymap::get_layer(std::size_t index) const {
    return m_layers[index];
}

Key_Code Keymap::get_key(std::size_t layer, std::size_t key_index) const {
    return m_layers[layer].keys[key_index];
}

/*****************************************/
/*              Utilities                */
/*****************************************/

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

/****************************************************/
/*         Convert Key Code to Display String       */
/****************************************************/
std::string key_code_to_display(Key_Code code) {
    switch (code) {
        case Key_Code::NONE:               return "";
        case Key_Code::DIGIT_0:            return "0";
        case Key_Code::DIGIT_1:            return "1";
        case Key_Code::DIGIT_2:            return "2";
        case Key_Code::DIGIT_3:            return "3";
        case Key_Code::DIGIT_4:            return "4";
        case Key_Code::DIGIT_5:            return "5";
        case Key_Code::DIGIT_6:            return "6";
        case Key_Code::DIGIT_7:            return "7";
        case Key_Code::DIGIT_8:            return "8";
        case Key_Code::DIGIT_9:            return "9";
        case Key_Code::ADD:                return "+";
        case Key_Code::SUBTRACT:           return "-";
        case Key_Code::MULTIPLY:           return "x";
        case Key_Code::DIVIDE:             return "/";
        case Key_Code::EQUALS:             return "=";
        case Key_Code::EVAL:               return "Eval";
        case Key_Code::APPROX:             return "Aprx";
        case Key_Code::DECIMAL:            return ".";
        case Key_Code::CLEAR:              return "CLR";
        case Key_Code::ALL_CLEAR:          return "AC";
        case Key_Code::BACKSPACE:          return "BSP";
        case Key_Code::PAREN_OPEN:         return "(";
        case Key_Code::PAREN_CLOSE:        return ")";
        case Key_Code::PERCENT:            return "%";
        case Key_Code::NEGATE:             return "+/-";
        case Key_Code::MEM_STORE:          return "MS";
        case Key_Code::MEM_RECALL:         return "MR";
        case Key_Code::MEM_ADD:            return "M+";
        case Key_Code::MEM_CLEAR:          return "MC";
        case Key_Code::SIN:                return "sin";
        case Key_Code::COS:                return "cos";
        case Key_Code::TAN:                return "tan";
        case Key_Code::ASIN:               return "asin";
        case Key_Code::ACOS:               return "acos";
        case Key_Code::ATAN:               return "atan";
        case Key_Code::LOG:                return "log";
        case Key_Code::LN:                 return "ln";
        case Key_Code::EXP:                return "e^x";
        case Key_Code::SQRT:               return "\xE2\x88\x9A";
        case Key_Code::CEIL:               return "ceil";
        case Key_Code::FLOOR:              return "floor";
        case Key_Code::ABS:                return "abs";
        case Key_Code::POWER_2:            return "x\xC2\xB2";
        case Key_Code::POWER_3:            return "x\xC2\xB3";
        case Key_Code::POWER_N:            return "x\xE2\x81\xBF";
        case Key_Code::FACTORIAL:          return "n!";
        case Key_Code::RECIPROCAL:         return "1/x";
        case Key_Code::PI:                 return "\xCF\x80";
        case Key_Code::EULER:              return "e";
        case Key_Code::BIT_AND:            return "AND";
        case Key_Code::BIT_OR:             return "OR";
        case Key_Code::BIT_XOR:            return "XOR";
        case Key_Code::BIT_NOT:            return "NOT";
        case Key_Code::SHIFT_LEFT:         return "<<";
        case Key_Code::SHIFT_RIGHT:        return ">>";
        case Key_Code::HEX_A:              return "A";
        case Key_Code::HEX_B:              return "B";
        case Key_Code::HEX_C:              return "C";
        case Key_Code::HEX_D:              return "D";
        case Key_Code::HEX_E:              return "E";
        case Key_Code::HEX_F:              return "F";
        case Key_Code::PHI:                return "\xCF\x86";
        case Key_Code::TAU:                return "\xCF\x84";
        case Key_Code::LAYER_NEXT:         return "Next";
        case Key_Code::LAYER_PREV:         return "Prev";
        case Key_Code::LAYER_CONST:        return "CST";
        case Key_Code::LAYER_ALG:          return "ALG";
        case Key_Code::LAYER_TRIG:         return "TRG";
        case Key_Code::LAYER_VAR:          return "Var";
        case Key_Code::LAYER_HOME:         return "Home";
        case Key_Code::TOGGLE_MATH_LAYOUT: return "MATH";
        case Key_Code::CURSOR_LEFT:        return LV_SYMBOL_LEFT;
        case Key_Code::CURSOR_RIGHT:       return LV_SYMBOL_RIGHT;
        case Key_Code::CURSOR_UP:          return LV_SYMBOL_UP;
        case Key_Code::CURSOR_DOWN:        return LV_SYMBOL_DOWN;
        case Key_Code::PAGE_UP:            return "PgUp";
        case Key_Code::PAGE_DOWN:          return "PgDn";
    }
    return "";
}

/****************************************/
/*      Load Layers from JSON File      */
/****************************************/
std::array<Layer, LAYER_COUNT> load_layers_from_json(
    const std::filesystem::path& json_path,
    const std::map<std::pair<int, int>, int>& matrix_index_map)
{
    std::ifstream file(json_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open keymap JSON file: " + json_path.string());
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
    if (layers_json.size() != LAYER_COUNT) {
        throw std::runtime_error("Invalid keymap JSON: expected " + std::to_string(LAYER_COUNT) +
                                 " layers, got " + std::to_string(layers_json.size()));
    }

    std::array<Layer, LAYER_COUNT> layers;

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

        // Build keys from JSON using matrix position -> visual index mapping
        // Resize to match the number of keys in the layout (matrix_index_map size)
        layers[i].keys.resize(matrix_index_map.size(), Key_Code::NONE);

        for (const auto& key_json : keys_json) {
            int row = 0;
            int col = 0;
            if (key_json.contains("row") && key_json["row"].is_number()) {
                row = key_json["row"].get<int>();
            }
            if (key_json.contains("col") && key_json["col"].is_number()) {
                col = key_json["col"].get<int>();
            }

            Key_Code code = Key_Code::NONE;
            if (key_json.contains("code") && key_json["code"].is_string()) {
                code = string_to_key_code(key_json["code"].get<std::string>());
            }

            // Map matrix position to visual key index
            auto it = matrix_index_map.find({row, col});
            if (it != matrix_index_map.end()) {
                int key_index = it->second;
                if (key_index >= 0 && key_index < static_cast<int>(layers[i].keys.size())) {
                    layers[i].keys[static_cast<std::size_t>(key_index)] = code;
                }
            }
        }
    }

    return layers;
}

} // namespace ovb::core
