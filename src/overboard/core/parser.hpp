#pragma once

#include <overboard/core/ast/ast.hpp>
#include <string>

class Parser {
    public:
        explicit Parser(const std::string& input);

        ovb::ast::Node_Ptr parse();

    private:
        const std::string& m_src;
        size_t             m_pos = 0;

        void          skip_ws();
        char          peek();
        char          consume();
        bool          match(char c);
        std::string   read_ident();
        double        read_number_literal();

        ovb::ast::Node_Ptr parse_expr();
        ovb::ast::Node_Ptr parse_add();
        ovb::ast::Node_Ptr parse_mul();
        ovb::ast::Node_Ptr parse_pow();
        ovb::ast::Node_Ptr parse_unary();
        ovb::ast::Node_Ptr parse_postfix();
        ovb::ast::Node_Ptr parse_primary();
        ovb::ast::Node_Ptr parse_func(const std::string& name);
};
