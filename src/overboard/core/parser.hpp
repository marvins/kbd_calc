#pragma once

#include <overboard/core/ast/ast.hpp>
#include <string>

class Parser {
    public:
        explicit Parser(const std::string& input);

        ast::Node_Ptr parse();

    private:
        const std::string& m_src;
        size_t             m_pos = 0;

        void          skip_ws();
        char          peek();
        char          consume();
        bool          match(char c);
        std::string   read_ident();
        double        read_number_literal();

        ast::Node_Ptr parse_expr();
        ast::Node_Ptr parse_add();
        ast::Node_Ptr parse_mul();
        ast::Node_Ptr parse_pow();
        ast::Node_Ptr parse_unary();
        ast::Node_Ptr parse_postfix();
        ast::Node_Ptr parse_primary();
        ast::Node_Ptr parse_func(const std::string& name);
};
