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
#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace ovb::core {

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

/**
 * @brief Convert string to key code
 * @param str String representation of key code
 * @return Key code value, or Key_Code::NONE if not found
 */
Key_Code string_to_key_code(const std::string& str);

/**
 * @brief Get display string for a key code
 * @param code Key code
 * @return Display string suitable for LVGL rendering
 */
std::string key_code_to_display(Key_Code code);

static constexpr int GRID_COLS = 8;
static constexpr int GRID_ROWS = 7;
static constexpr int DEFAULT_GRID_KEYS = 34;  // Default key count for MF34
static constexpr int LAYER_COUNT = 5;         // Number of keyboard layers

/**
 * @brief Represents a keyboard layer with its name and key mappings
 */
struct Layer {
    std::string             name;
    std::vector<Key_Code>   keys;
};

/**
 * @brief Load keymap layers from JSON file
 * @param json_path Path to JSON file containing layer definitions
 * @param matrix_index_map Map from (row, col) to visual key index
 * @return Array of layers loaded from JSON
 * @throws std::runtime_error if file cannot be read or parsed
 */
std::array<Layer, LAYER_COUNT> load_layers_from_json(
    const std::filesystem::path& json_path,
    const std::map<std::pair<int, int>, int>& matrix_index_map);

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

        Keymap() = default;

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
        const Layer& get_layer(std::size_t index) const;

        /**
         * @brief Retrieves a key code from a specific layer.
         * @param layer The layer index.
         * @param key_index The key index within that layer.
         * @return Key code for the specified key.
         */
        Key_Code get_key( std::size_t layer,
                          std::size_t key_index ) const;

    private:
        std::array<Layer, LAYER_COUNT> m_layers;  ///< Layer definitions
};

} // namespace ovb::core
