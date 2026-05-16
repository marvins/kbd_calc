#include <overboard/core/parser.hpp>
#include <cctype>
#include <stdexcept>

Parser::Parser(const std::string& input) : m_src(input) {}

// ─── Lexer helpers ────────────────────────────────────────────────────────────

void Parser::skip_ws() {
    while (m_pos < m_src.size() && m_src[m_pos] == ' ') ++m_pos;
}

char Parser::peek() {
    skip_ws();
    return m_pos < m_src.size() ? m_src[m_pos] : '\0';
}

char Parser::consume() {
    skip_ws();
    return m_pos < m_src.size() ? m_src[m_pos++] : '\0';
}

bool Parser::match(char c) {
    if (peek() == c) { ++m_pos; return true; }
    return false;
}

std::string Parser::read_ident() {
    skip_ws();
    std::string id;
    while (m_pos < m_src.size() && (std::isalpha(m_src[m_pos]) || m_src[m_pos] == '_'))
        id += m_src[m_pos++];
    return id;
}

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

// ─── Entry point ──────────────────────────────────────────────────────────────

ovb::ast::Node_Ptr Parser::parse() {
    auto node = parse_expr();
    skip_ws();
    if (m_pos != m_src.size())
        throw std::runtime_error("Unexpected: " + m_src.substr(m_pos));
    return node;
}

// ─── Grammar (precedence low → high) ─────────────────────────────────────────
//
//   expr      → add ('^' add)?          (XOR / top-level pow)
//   add       → mul (('+' | '-' | '&' | '|') mul)*
//   mul       → pow (('*' | '/' | '%' | '<<' | '>>') pow)*
//   pow       → unary ('^' pow)?        (right-assoc)
//   unary     → ('-' | '+') unary | postfix
//   postfix   → primary ('!')*
//   primary   → '(' expr ')' | ident | number

ovb::ast::Node_Ptr Parser::parse_expr() {
    auto v = parse_add();
    skip_ws();
    if (peek() == '^') {
        consume();
        auto r = parse_add();
        v = std::make_unique<ovb::ast::Binary_Op_Node>(
            ovb::ast::Binary_Op::POWER, std::move(v), std::move(r));
    }
    return v;
}

ovb::ast::Node_Ptr Parser::parse_add() {
    auto v = parse_mul();
    while (true) {
        skip_ws();
        char op = peek();
        if (op == '+') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::ADD, std::move(v), parse_mul());
        } else if (op == '-') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::SUBTRACT, std::move(v), parse_mul());
        } else if (op == '&') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::BIT_AND, std::move(v), parse_mul());
        } else if (op == '|') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::BIT_OR, std::move(v), parse_mul());
        } else {
            break;
        }
    }
    return v;
}

ovb::ast::Node_Ptr Parser::parse_mul() {
    auto v = parse_pow();
    while (true) {
        skip_ws();
        char op = peek();
        if (op == '*') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::MULTIPLY, std::move(v), parse_pow());
        } else if (op == '/') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::DIVIDE, std::move(v), parse_pow());
        } else if (op == '%') {
            consume();
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::MODULO, std::move(v), parse_pow());
        } else if (op == '<' && m_pos + 1 < m_src.size() && m_src[m_pos + 1] == '<') {
            m_pos += 2;
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::SHIFT_LEFT, std::move(v), parse_pow());
        } else if (op == '>' && m_pos + 1 < m_src.size() && m_src[m_pos + 1] == '>') {
            m_pos += 2;
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::SHIFT_RIGHT, std::move(v), parse_pow());
        } else if (std::isalpha(op) || op == '(') {
            // Implicit multiplication: e.g. 2pi, 3(x+1)
            v = std::make_unique<ovb::ast::Binary_Op_Node>(
                ovb::ast::Binary_Op::MULTIPLY, std::move(v), parse_pow());
        } else {
            break;
        }
    }
    return v;
}

ovb::ast::Node_Ptr Parser::parse_pow() {
    auto base = parse_unary();
    skip_ws();
    if (peek() == '^') {
        consume();
        auto exp = parse_pow();
        return std::make_unique<ovb::ast::Binary_Op_Node>(
            ovb::ast::Binary_Op::POWER, std::move(base), std::move(exp));
    }
    return base;
}

ovb::ast::Node_Ptr Parser::parse_unary() {
    skip_ws();
    if (peek() == '-') {
        consume();
        return std::make_unique<ovb::ast::Unary_Op_Node>(
            ovb::ast::Unary_Op::NEGATE, parse_unary());
    }
    if (peek() == '+') {
        consume();
        return parse_unary();
    }
    if (peek() == '~') {
        consume();
        return std::make_unique<ovb::ast::Unary_Op_Node>(
            ovb::ast::Unary_Op::BIT_NOT, parse_unary());
    }
    return parse_postfix();
}

ovb::ast::Node_Ptr Parser::parse_postfix() {
    auto v = parse_primary();
    while (peek() == '!') {
        consume();
        v = std::make_unique<ovb::ast::Factorial_Node>(std::move(v));
    }
    return v;
}

ovb::ast::Node_Ptr Parser::parse_primary() {
    skip_ws();

    if (peek() == '(') {
        consume();
        auto v = parse_expr();
        if (peek() == ')') consume();
        return v;
    }

    if (std::isalpha(peek())) {
        std::string id = read_ident();

        if (id == "pi")
            return std::make_unique<ovb::ast::Constant_Node>(ovb::ast::Constant_Id::PI);
        if (id == "e" && peek() != '(')
            return std::make_unique<ovb::ast::Constant_Node>(ovb::ast::Constant_Id::E);
        if (id == "phi")
            return std::make_unique<ovb::ast::Constant_Node>(ovb::ast::Constant_Id::PHI);
        if (id == "tau")
            return std::make_unique<ovb::ast::Constant_Node>(ovb::ast::Constant_Id::TAU);
        if (peek() == '(')
            return parse_func(id);

        if (id.size() == 1 && id[0] >= 'A' && id[0] <= 'F')
            return std::make_unique<ovb::ast::Number_Node>(
                static_cast<double>(id[0] - 'A' + 10));

        throw std::runtime_error("Unknown identifier: " + id);
    }

    return std::make_unique<ovb::ast::Number_Node>(read_number_literal());
}

ovb::ast::Node_Ptr Parser::parse_func(const std::string& name) {
    consume(); // '('
    std::vector<ovb::ast::Node_Ptr> args;
    if (peek() != ')') {
        args.push_back(parse_expr());
        while (peek() == ',') {
            consume();
            args.push_back(parse_expr());
        }
    }
    if (peek() == ')') consume();
    return std::make_unique<ovb::ast::Function_Node>(name, std::move(args));
}
