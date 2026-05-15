#pragma once

#include "ast.hpp"
#include <string>

class Parser {
    public:
        explicit Parser(const std::string& input);

        Ast::Node_Ptr parse();

    private:
        const std::string& m_src;
        size_t             m_pos = 0;

        void          skip_ws();
        char          peek();
        char          consume();
        bool          match(char c);
        std::string   read_ident();
        double        read_number_literal();

        Ast::Node_Ptr parse_expr();
        Ast::Node_Ptr parse_add();
        Ast::Node_Ptr parse_mul();
        Ast::Node_Ptr parse_pow();
        Ast::Node_Ptr parse_unary();
        Ast::Node_Ptr parse_postfix();
        Ast::Node_Ptr parse_primary();
        Ast::Node_Ptr parse_func(const std::string& name);
};
