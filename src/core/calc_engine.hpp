#pragma once

#include "ast.hpp"
#include "keymap.hpp"
#include <string>

struct Calc_State {
    std::string   expression;
    std::string   display_value;
    std::string   error;
    double        memory     = 0.0;
    int           cursor_pos = 0;
    Ast::Node_Ptr last_ast;          // populated after each successful evaluate()
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

        void          append_to_expression(const std::string& token);
        void          evaluate();
};
