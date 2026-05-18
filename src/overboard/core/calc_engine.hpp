#pragma once

#include <overboard/core/ast/ast.hpp>
#include <overboard/core/expression.hpp>
#include <overboard/core/keymap.hpp>
#include <deque>
#include <string>

struct History_Entry {
    std::string input;   // expression string
    std::string result;  // evaluated result string
};

struct Calc_State {
    Expression    expression;
    std::string   display_value;
    std::string   error;
    double        memory     = 0.0;
    ovb::ast::Node_Ptr last_ast;          // populated after each successful evaluate()
    bool          use_math_layout = true; // display mode: true = math layout, false = single-line

    std::deque<History_Entry> history;    // newest entries at front
    static constexpr size_t MAX_HISTORY = 20;
};

class Calc_Engine {
    public:
        Calc_Engine();

        void              handle_key(Key_Code code);
        const Calc_State& state() const;
        void              reset();
        void              toggle_math_layout();  // Toggle between math layout and single-line

    private:
        Calc_State m_state;
        bool       m_result_shown;

        void          try_insert(Key_Code code);
        void          evaluate();
};
