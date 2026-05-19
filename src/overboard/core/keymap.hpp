#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string_view>

enum class Key_Label : uint8_t {
    NONE = 0,

    // Digits
    D_0, D_1, D_2, D_3, D_4,
    D_5, D_6, D_7, D_8, D_9,

    // Hex digits
    HEX_A, HEX_B, HEX_C, HEX_D, HEX_E, HEX_F,

    // Arithmetic
    ADD, SUBTRACT, MULTIPLY, DIVIDE, EQUALS, DECIMAL, PERCENT,

    // Editing
    BACKSPACE, CLEAR, ALL_CLEAR, NEGATE, PAREN_OPEN, PAREN_CLOSE,

    // Cursor
    CURSOR_LEFT, CURSOR_RIGHT, CURSOR_UP, CURSOR_DOWN,

    // Scientific
    SIN, COS, TAN, ASIN, ACOS, ATAN,
    LOG, LN, EXP, SQRT, FACTORIAL, RECIPROCAL,
    PI, EULER, PHI, TAU,
    POWER_2, POWER_N,

    // Programmer
    BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, SHIFT_LEFT, SHIFT_RIGHT,

    // Layer / meta
    LAYER_TRIG,
    LAYER_ALGEBRA,
    LAYER_HOME,
    LAYER_CONST,
    APPROX,

    // Display mode
    MATH_LAYOUT,
};

std::string label_string(Key_Label lbl);

enum class Key_Code : uint16_t {
    NONE = 0,

    // Digits
    DIGIT_0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,

    // Basic ops
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EQUALS,
    DECIMAL,
    CLEAR,
    ALL_CLEAR,
    BACKSPACE,

    // Grouping / extra basic
    PAREN_OPEN,
    PAREN_CLOSE,
    PERCENT,
    NEGATE,

    // Memory
    MEM_STORE,
    MEM_RECALL,
    MEM_ADD,
    MEM_CLEAR,

    // Scientific
    SIN,
    COS,
    TAN,
    ASIN,
    ACOS,
    ATAN,
    LOG,
    LN,
    EXP,
    SQRT,
    POWER_2,
    POWER_3,
    POWER_N,
    FACTORIAL,
    RECIPROCAL,
    PI,
    EULER,

    // Programmer
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    BIT_NOT,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    HEX_A,
    HEX_B,
    HEX_C,
    HEX_D,
    HEX_E,
    HEX_F,

    // Cursor / editing
    CURSOR_LEFT,
    CURSOR_RIGHT,
    CURSOR_UP,
    CURSOR_DOWN,

    // Constants
    PHI,
    TAU,

    // Meta / control
    LAYER_NEXT,
    LAYER_PREV,
    CONST_LAYER,
    ALGEBRA_LAYER,
    LAYER_HOME,
    APPROX,

    // Display mode
    TOGGLE_MATH_LAYOUT,
};

static constexpr int GRID_COLS = 8;
static constexpr int GRID_ROWS = 6;
static constexpr int GRID_KEYS = 30;  // Actual key count (not grid cells)

struct Key_Def {
    Key_Code  code;
    Key_Label label;
};

struct Layer {
    std::string_view                    name;
    std::array<Key_Def, GRID_KEYS>      keys;
};

/**
 * @brief Manages keyboard layer definitions and key mappings.
 *
 * The Keymap class provides access to predefined keyboard layers, each containing
 * a mapping of key indices to key codes and labels. It supports layer-based
 * keyboard layouts where different functional sets can be switched between.
 */
class Keymap {
    public:
        /// Number of predefined keyboard layers
        static constexpr int LAYER_COUNT = 5;

        /**
         * @brief Returns the total number of layers in the keymap.
         * @return The layer count (always LAYER_COUNT).
         */
        constexpr int layer_count() const { return LAYER_COUNT; }

        /**
         * @brief Retrieves a layer by index.
         * @param index The layer index (0 to LAYER_COUNT-1).
         * @return Reference to the requested Layer.
         */
        constexpr const Layer& get_layer(std::size_t index) const;

        /**
         * @brief Retrieves a key definition from a specific layer.
         * @param layer The layer index.
         * @param key_index The key index within that layer.
         * @return Reference to the Key_Def containing code and label.
         */
        constexpr const Key_Def& get_key(std::size_t layer, std::size_t key_index) const;

        /**
         * @brief Builds and returns the array of all predefined layers.
         * @return An array containing LAYER_COUNT Layer objects.
         *
         * This static factory method creates the layer definitions for Basic,
         * TRG, Constants, Programmer, and Algebra modes.
         */
        static constexpr std::array<Layer, LAYER_COUNT> build_layers() {
            using KC = Key_Code;
            using KL = Key_Label;

            return {{
                { "Basic", {{
                    // 30 keys (7+7+6+4+3+3=30)
                    // Row 0 (7 keys)
                    { KC::LAYER_HOME, KL::LAYER_HOME }, { KC::NONE, KL::NONE }, { KC::BACKSPACE, KL::BACKSPACE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NEGATE, KL::NEGATE },
                    // Row 1 (7 keys) - 3 left + numpad 7-9 + tall +
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::DIGIT_7, KL::D_7 }, { KC::DIGIT_8, KL::D_8 }, { KC::DIGIT_9, KL::D_9 }, { KC::ADD, KL::ADD },
                    // Row 2 (6 keys) - 3 left + numpad 4-6
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::DIGIT_4, KL::D_4 }, { KC::DIGIT_5, KL::D_5 }, { KC::DIGIT_6, KL::D_6 },
                    // Row 3 (4 keys) - numpad 1-3 + Enter
                    { KC::DIGIT_1, KL::D_1 }, { KC::DIGIT_2, KL::D_2 }, { KC::DIGIT_3, KL::D_3 }, { KC::EQUALS, KL::EQUALS },
                    // Row 4 (3 keys) - blank, 0, period
                    { KC::CURSOR_UP, KL::CURSOR_UP }, { KC::DIGIT_0, KL::D_0 }, { KC::DECIMAL, KL::DECIMAL },
                    // Row 5 (3 keys) - <, ↓, ↑ (swapped)
                    { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_DOWN, KL::CURSOR_DOWN }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT },
                }}},
                { "TRG", {{
                    // 30 keys (7+7+6+4+3+3)
                    { KC::LAYER_HOME, KL::LAYER_HOME }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::BACKSPACE, KL::BACKSPACE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::SIN, KL::SIN }, { KC::COS, KL::COS }, { KC::TAN, KL::TAN }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::ASIN, KL::ASIN }, { KC::ACOS, KL::ACOS }, { KC::ATAN, KL::ATAN },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT },
                    { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_DOWN, KL::CURSOR_DOWN }, { KC::CURSOR_UP, KL::CURSOR_UP },
                }}},
                { "Constants", {{
                    // 30 keys (7+7+6+4+3+3)
                    { KC::LAYER_HOME, KL::LAYER_HOME }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::BACKSPACE, KL::BACKSPACE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::PI, KL::PI }, { KC::EULER, KL::EULER }, { KC::PHI, KL::PHI }, { KC::TAU, KL::TAU },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT },
                    { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_DOWN, KL::CURSOR_DOWN }, { KC::CURSOR_UP, KL::CURSOR_UP },
                }}},
                { "Programmer", {{
                    // 30 keys (7+7+6+4+3+3)
                    { KC::LAYER_HOME, KL::LAYER_HOME }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::ALL_CLEAR, KL::ALL_CLEAR }, { KC::BACKSPACE, KL::BACKSPACE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::BIT_AND, KL::BIT_AND }, { KC::BIT_OR, KL::BIT_OR }, { KC::BIT_XOR, KL::BIT_XOR }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::HEX_A, KL::HEX_A }, { KC::HEX_B, KL::HEX_B }, { KC::HEX_C, KL::HEX_C },
                    { KC::NONE, KL::NONE }, { KC::HEX_D, KL::HEX_D }, { KC::HEX_E, KL::HEX_E }, { KC::HEX_F, KL::HEX_F },
                    { KC::NONE, KL::NONE }, { KC::DIGIT_0, KL::D_0 }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT },
                    { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_DOWN, KL::CURSOR_DOWN }, { KC::CURSOR_UP, KL::CURSOR_UP },
                }}},
                { "Algebra", {{
                    // 30 keys (7+7+6+4+3+3)
                    { KC::LAYER_HOME, KL::LAYER_HOME }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::BACKSPACE, KL::BACKSPACE }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::RECIPROCAL, KL::RECIPROCAL }, { KC::FACTORIAL, KL::FACTORIAL }, { KC::PERCENT, KL::PERCENT }, { KC::NONE, KL::NONE },
                    { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::NONE, KL::NONE }, { KC::LOG, KL::LOG }, { KC::LN, KL::LN }, { KC::EXP, KL::EXP },
                    { KC::NONE, KL::NONE }, { KC::PAREN_OPEN, KL::PAREN_OPEN }, { KC::PAREN_CLOSE, KL::PAREN_CLOSE }, { KC::ADD, KL::ADD },
                    { KC::NONE, KL::NONE }, { KC::LAYER_HOME, KL::LAYER_HOME }, { KC::CURSOR_RIGHT, KL::CURSOR_RIGHT },
                    { KC::CURSOR_LEFT, KL::CURSOR_LEFT }, { KC::CURSOR_DOWN, KL::CURSOR_DOWN }, { KC::CURSOR_UP, KL::CURSOR_UP },
                }}},
            }};
        }

};

inline constexpr std::array<Layer, Keymap::LAYER_COUNT> KEYMAP_LAYERS = Keymap::build_layers();

inline constexpr const Layer& Keymap::get_layer(std::size_t index) const {
    return KEYMAP_LAYERS[index];
}

inline constexpr const Key_Def& Keymap::get_key(std::size_t layer, std::size_t key_index) const {
    return KEYMAP_LAYERS[layer].keys[key_index];
}
