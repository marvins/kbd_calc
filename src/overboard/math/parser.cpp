/**
 * @file   parser.cpp
 * @author Marvin Smith
 * @date   5/20/2026
 *
 * @brief  Implementation of the Parser class for parsing mathematical expressions.
 */
#include <overboard/math/parser.hpp>

// C++ Standard Libraries
#include <cctype>
#include <stdexcept>

// Project Libraries
#include <overboard/math/ast/binary_op_node.hpp>
#include <overboard/math/ast/constant_node.hpp>
#include <overboard/math/ast/factorial_node.hpp>
#include <overboard/math/ast/function_node.hpp>
#include <overboard/math/ast/number_node.hpp>
#include <overboard/math/ast/placeholder_node.hpp>
#include <overboard/math/ast/unary_op_node.hpp>
#include <overboard/math/ast/variable_node.hpp>

namespace ovb::math {

/********************************/
/*         Constructor          */
/********************************/
Parser::Parser(const std::string& input) : m_src(input) {}

/********************************/
/*         Lexer Helpers        */
/********************************/
void Parser::skip_ws() {
    while (m_pos < m_src.size() && m_src[m_pos] == ' ') ++m_pos;
}

/********************************/
/*           Peek               */
/********************************/
char Parser::peek() {
    skip_ws();
    return m_pos < m_src.size() ? m_src[m_pos] : '\0';
}

/********************************/
/*           Consume            */
/********************************/
char Parser::consume() {
    skip_ws();
    return m_pos < m_src.size() ? m_src[m_pos++] : '\0';
}

/********************************/
/*           Match              */
/********************************/
bool Parser::match(char c) {
    if (peek() == c) { ++m_pos; return true; }
    return false;
}

/********************************/
/*           Read Ident         */
/********************************/
std::string Parser::read_ident() {
    skip_ws();
    std::string id;
    while (m_pos < m_src.size() && (std::isalpha(m_src[m_pos]) || m_src[m_pos] == '_'))
        id += m_src[m_pos++];
    return id;
}

/********************************/
/*       Read Number Literal    */
/********************************/
double Parser::read_number_literal() {
    skip_ws();
    size_t start = m_pos;
    bool   is_hex = false;

    if (m_pos + 1 < m_src.size() && m_src[m_pos] == '0' &&
        (m_src[m_pos + 1] == 'x' || m_src[m_pos + 1] == 'X')) {
        m_pos += 2;
        is_hex = true;
        while (m_pos < m_src.size() && std::isxdigit(m_src[m_pos])) ++m_pos;
    } else {
        while (m_pos < m_src.size() && (std::isdigit(m_src[m_pos]) || m_src[m_pos] == '.')) ++m_pos;
        if (m_pos < m_src.size() && (m_src[m_pos] == 'e' || m_src[m_pos] == 'E')) {
            ++m_pos;
            if (m_pos < m_src.size() && (m_src[m_pos] == '+' || m_src[m_pos] == '-')) ++m_pos;
            while (m_pos < m_src.size() && std::isdigit(m_src[m_pos])) ++m_pos;
        }
    }

    if (m_pos == start) throw std::runtime_error("Expected number");
    std::string tok = m_src.substr(start, m_pos - start);
    if (is_hex) return static_cast<double>(std::stoll(tok, nullptr, 16));
    return std::stod(tok);
}

/********************************/
/*           Parse              */
/********************************/
ast::Node::ptr_t Parser::parse() {
    auto node = parse_expr();
    skip_ws();
    if (m_pos != m_src.size())
        throw std::runtime_error("Unexpected: " + m_src.substr(m_pos));
    return node;
}

/********************************/
/*         Parse Expr           */
/********************************/
ast::Node::ptr_t Parser::parse_expr() {
    auto v = parse_add();
    skip_ws();
    if (peek() == '^') {
        consume();
        auto r = parse_add();
        v = std::make_unique<ast::Binary_Op_Node>(
            ast::Binary_Op::POWER, std::move(v), std::move(r));
    }
    return v;
}

/********************************/
/*         Parse Add            */
/********************************/
ast::Node::ptr_t Parser::parse_add() {
    auto v = parse_mul();
    while (true) {
        skip_ws();
        char op = peek();
        if (op == '+') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::ADD, std::move(v), parse_mul());
        } else if (op == '-') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::SUBTRACT, std::move(v), parse_mul());
        } else if (op == '&') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::BIT_AND, std::move(v), parse_mul());
        } else if (op == '|') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::BIT_OR, std::move(v), parse_mul());
        } else {
            break;
        }
    }
    return v;
}

/********************************/
/*         Parse Mul            */
/********************************/
ast::Node::ptr_t Parser::parse_mul() {
    auto v = parse_pow();
    while (true) {
        skip_ws();
        char op = peek();
        if (op == '*') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::MULTIPLY, std::move(v), parse_pow());
        } else if (op == '/') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::DIVIDE, std::move(v), parse_pow());
        } else if (op == '%') {
            consume();
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::MODULO, std::move(v), parse_pow());
        } else if (op == '<' && m_pos + 1 < m_src.size() && m_src[m_pos + 1] == '<') {
            m_pos += 2;
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::SHIFT_LEFT, std::move(v), parse_pow());
        } else if (op == '>' && m_pos + 1 < m_src.size() && m_src[m_pos + 1] == '>') {
            m_pos += 2;
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::SHIFT_RIGHT, std::move(v), parse_pow());
        } else if (op == '(' || (op > 0 && std::isalpha(op))) {
            // Implicit multiplication: e.g. 2pi, 3(x+1)
            // Note: op > 0 check prevents UTF-8 continuation bytes from triggering this
            v = std::make_unique<ast::Binary_Op_Node>(
                ast::Binary_Op::MULTIPLY, std::move(v), parse_pow());
        } else {
            break;
        }
    }
    return v;
}

/********************************/
/*         Parse Pow            */
/********************************/
ast::Node::ptr_t Parser::parse_pow() {
    auto base = parse_unary();
    skip_ws();
    if (peek() == '^') {
        consume();
        auto exp = parse_pow();
        return std::make_unique<ast::Binary_Op_Node>(
            ast::Binary_Op::POWER, std::move(base), std::move(exp));
    }
    return base;
}

/************************************/
/*          Parse Unary             */
/************************************/
ast::Node::ptr_t Parser::parse_unary() {
    skip_ws();
    if (peek() == '-') {
        consume();
        return std::make_unique<ast::Unary_Op_Node>(
            ast::Unary_Op::NEGATE, parse_unary());
    }
    if (peek() == '+') {
        consume();
        return parse_unary();
    }
    if (peek() == '~') {
        consume();
        return std::make_unique<ast::Unary_Op_Node>(
            ast::Unary_Op::BIT_NOT, parse_unary());
    }
    return parse_postfix();
}

/************************************/
/*          Parse Postfix           */
/************************************/
ast::Node::ptr_t Parser::parse_postfix() {
    auto v = parse_primary();
    while (peek() == '!') {
        consume();
        v = std::make_unique<ast::Factorial_Node>(std::move(v));
    }
    return v;
}

/************************************/
/*          Parse Primary           */
/************************************/
ast::Node::ptr_t Parser::parse_primary() {
    skip_ws();

    // Check for placeholder (Unicode white square □)
    if (m_pos + 2 < m_src.size() &&
        static_cast<unsigned char>(m_src[m_pos])   == 0xE2 &&
        static_cast<unsigned char>(m_src[m_pos+1]) == 0x96 &&
        static_cast<unsigned char>(m_src[m_pos+2]) == 0xA1) {
        m_pos += 3;
        return std::make_unique<ast::Placeholder_Node>();
    }

    if (peek() == '(') {
        consume();
        auto v = parse_expr();
        if (peek() == ')') consume();
        return v;
    }

    if (std::isalpha(peek())) {
        std::string id = read_ident();

        if (id == "pi")
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::PI);
        if (id == "e" && peek() != '(')
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::E);
        if (id == "phi")
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::PHI);
        if (id == "tau")
            return std::make_unique<ast::Constant_Node>(ast::Constant_Id::TAU);
        if (peek() == '(')
            return parse_func(id);

        if (id.size() == 1 && id[0] >= 'A' && id[0] <= 'F')
            return std::make_unique<ast::Number_Node>(
                static_cast<double>(id[0] - 'A' + 10));

        // Treat unknown identifiers as variables for rendering purposes
        return std::make_unique<ast::Variable_Node>(id);
    }

    return std::make_unique<ast::Number_Node>(read_number_literal());
}

/************************************/
/*          Parse Function          */
/************************************/
ast::Node::ptr_t Parser::parse_func(const std::string& name) {
    consume(); // '('
    std::vector<ast::Node::ptr_t> args;
    if (peek() != ')') {
        args.push_back(parse_expr());
        while (peek() == ',') {
            consume();
            args.push_back(parse_expr());
        }
    }
    if (peek() == ')') consume();
    return std::make_unique<ast::Function_Node>(name, std::move(args));
}

} // namespace ovb::math
