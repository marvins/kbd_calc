#pragma once

#include "ast.hpp"
#include "expression.hpp"
#include "keymap.hpp"
#include <string>

struct Calc_State {
    Expression    expression;
    std::string   display_value;
    std::string   error;
    double        memory     = 0.0;
    ast::Node_Ptr last_ast;          // populated after each successful evaluate()
};

class Calc_Engine {
    public:
        Calc_Engine();

        void              handle_key(Key_Code code);
        const Calc_State& state() const;
        void              reset();

    private:
        Calc_State m_state;
        bool       m_result_shown;

        void          try_insert(Key_Code code);
        void          evaluate();
};
