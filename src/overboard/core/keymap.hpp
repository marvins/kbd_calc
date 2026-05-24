/**
 * @file   keymap.hpp
 * @author Marvin Smith
 * @date   2025-10-18
 * @brief  Key mapping and layout definitions
 */
#pragma once

// C++ Standard Libraries
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace ovb::core {

/**
 * @brief Key label enumeration
 */
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
    LAYER_CONST,
    LAYER_VAR,
    LAYER_HOME,
    APPROX,
    EVAL,

    // Display mode
    MATH_LAYOUT,

    // Navigation and variables
    PG_UP,
    PG_DN,
};

/**
 * @brief Convert key label to string
 * @param lbl Key label
 * @return String representation of the key label
 */
std::string label_string(Key_Label lbl);

/**
 * @brief Key code enumeration
 */
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
    EVAL,
    APPROX,
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

    // Constants
    PHI,
    TAU,

    // Meta / control
    LAYER_NEXT,
    LAYER_PREV,
    LAYER_CONST,
    LAYER_ALG,
    LAYER_TRIG,
    LAYER_VAR,
    LAYER_HOME,

    // Display mode
    TOGGLE_MATH_LAYOUT,

    // Navigation
    CURSOR_LEFT,
    CURSOR_RIGHT,
    CURSOR_UP,
    CURSOR_DOWN,
    PAGE_UP,
    PAGE_DOWN,
};

static constexpr int GRID_COLS = 8;
static constexpr int GRID_ROWS = 7;
static constexpr int DEFAULT_GRID_KEYS = 34;  // Default key count for MF34
static constexpr int LAYER_COUNT = 5;         // Number of keyboard layers

struct Key_Def {
    Key_Code  code;
    Key_Label label;
};

struct Layer {
    std::string                   name;
    std::vector<Key_Def>         keys;
};

/**
 * @brief Load keymap layers from JSON file
 * @param json_path Path to JSON file containing layer definitions
 * @return Array of layers loaded from JSON
 * @throws std::runtime_error if file cannot be read or parsed
 */
std::array<Layer, LAYER_COUNT> load_layers_from_json(const std::string& json_path);

/**
 * @brief Manages keyboard layer definitions and key mappings.
 *
 * The Keymap class provides access to keyboard layers loaded from JSON,
 * each containing a mapping of key indices to key codes and labels.
 * It supports layer-based keyboard layouts where different functional
 * sets can be switched between.
 */
class Keymap {
    public:
        /**
         * @brief Constructor with custom layers
         * @param layers Array of layer definitions
         */
        explicit Keymap(const std::array<Layer, LAYER_COUNT>& layers);

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
        const Key_Def& get_key( std::size_t layer,
                                std::size_t key_index ) const;

    private:
        std::array<Layer, LAYER_COUNT> m_layers;  ///< Layer definitions
};

} // namespace ovb::core
