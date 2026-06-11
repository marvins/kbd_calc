/**
 * @file    calc_engine.hpp
 * @author  Marvin Smith
 * @date    5/20/2026
 *
 * @brief Header file for the Calc_Engine class.
 */
#pragma once

// C++ Standard Libraries
#include <deque>
#include <string>

// Project Libraries
#include <overboard/core/input_key.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/math/ast/node.hpp>
#include <overboard/math/expression.hpp>

namespace ovb::math {

/**
 * @brief History entry for calculation history
 */
struct History_Entry {
    std::string input;   // expression string
    std::string result;  // evaluated result string
};

/**
 * @brief Calculation state for the calculator
 */
struct Calc_State {
    Expression    expression;
    std::string   display_value;
    std::string   error;
    double        memory     = 0.0;
    ast::Node::ptr_t last_ast;          // populated after each successful evaluate()
    bool          use_math_layout = true; // display mode: true = math layout, false = single-line

    std::deque<History_Entry> history;    // newest entries at front
    static constexpr size_t MAX_HISTORY = 20;
};

/**
 * @brief Calculator engine for handling mathematical expressions
 */
class Calc_Engine {

    public:

        /**
         * @brief Construct a new Calc_Engine object
         */
        Calc_Engine();

        /**
         * @brief Handle a key press
         *
         * @param code Key code
         */
        void handle_key( core::Action_Code code );

        /**
         * @brief Get the current calculation state
         *
         * @return const Calc_State& Current calculation state
         */
        const Calc_State& state() const;

        /**
         * @brief Check if a result is currently displayed
         *
         * @return true if result is shown, false if expression is being edited
         */
        bool result_shown() const;

        /**
         * @brief Reset the calculator state
         */
        void reset();

        /**
         * @brief Toggle between math layout and single-line display
         */
        void toggle_math_layout();

    private:

        /// @brief Current calculation state
        Calc_State m_state;

        /// @brief Flag to indicate if result is currently displayed
        bool       m_result_shown;

        /// @brief Try to insert a key code into the expression
        void          try_insert( core::Action_Code code );

        /// @brief Evaluate the current expression
        void          evaluate();

}; // End of Calc_Engine class

} // namespace ovb::math
