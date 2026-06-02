/**
 * @file    action_code.hpp
 * @author  Marvin Smith
 * @date    2026-05-31
 *
 * @brief   Calculator action codes
 *
 * Defines all behaviors/actions the calculator can perform.
 * These are semantic operations, not physical keys.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <string>

namespace ovb::core {

/**
 * @brief Calculator action enumeration
 *
 * Represents behaviors the calculator can perform.
 * These are actions, not physical inputs.
 */
enum class Action_Code : uint16_t {
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
    DECIMAL,

    // Basic operators
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EQUALS,

    // Evaluation
    EVAL,
    APPROX,

    // Basic arithmetic
    NEGATE,
    PERCENT,

    // Scientific functions
    SIN,
    COS,
    TAN,
    ASIN,
    ACOS,
    ATAN,
    ATAN2,
    LOG,
    LN,
    EXP,
    SQRT,
    POWER_2,
    POWER_3,
    POWER_N,
    FACTORIAL,
    RECIPROCAL,

    // Constants
    PI,
    EULER,
    PHI,
    TAU,

    // Additional scientific
    CEIL,
    FLOOR,
    ABS,

    // Programmer mode
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

    // Memory
    MEM_STORE,
    MEM_RECALL,
    MEM_ADD,
    MEM_CLEAR,

    // Navigation actions (semantic)
    CURSOR_HOME,
    CURSOR_END,
    CURSOR_LEFT_WORD,
    CURSOR_RIGHT_WORD,

    // Grouping
    PAREN_OPEN,
    PAREN_CLOSE,

    // Layer actions
    GO_HOME_LAYER,
    GO_CONST_LAYER,
    GO_ALG_LAYER,
    GO_TRIG_LAYER,
    GO_VAR_LAYER,
    NEXT_LAYER,
    PREV_LAYER,

    // System
    ESCAPE,
    TOGGLE_MATH_LAYOUT,
    NONE_ACTION,  // Explicit no-op

    // Navigation
    CURSOR_LEFT,
    CURSOR_RIGHT,
    CURSOR_UP,
    CURSOR_DOWN,
    PAGE_UP,
    PAGE_DOWN,

    // Editing
    BACKSPACE,
    CLEAR,
    ALL_CLEAR,

    // Function keys
    FUNC_1,
    FUNC_2,
    FUNC_3,
    FUNC_4,
    FUNC_5,
    FUNC_6,
    FUNC_7,
    FUNC_8,
    FUNC_9,
    FUNC_10,
};

/**
 * @brief Convert string to action code
 * @param str String representation
 * @return Action code value, or Action_Code::NONE if not found
 */
Action_Code string_to_action_code(const std::string& str);

/**
 * @brief Get display string for an action code
 * @param code Action code
 * @return Display string suitable for UI rendering
 */
std::string action_code_to_display(Action_Code code);

/**
 * @brief Convert action code to string for testing/debugging
 * @param code Action code
 * @return String representation (e.g., "SIN", "EVAL", "GO_HOME_LAYER")
 */
std::string action_code_to_string(Action_Code code);

} // namespace ovb::core

