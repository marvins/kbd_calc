/**
 * @file parser.hpp
 * @brief Parser for mathematical expressions
 */
#pragma once

// C++ Standard Libraries
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math {

/**
 * @brief Parser for mathematical expressions
 */
class Parser {

    public:

        /**
         * @brief Construct a new Parser object
         *
         * @param input Input string to parse
         */
        explicit Parser(const std::string& input);

        /**
         * @brief Parse the input string into an AST
         *
         * @return ast::Node::ptr_t Parsed AST
         */
        ast::Node::ptr_t parse();

    private:
        /**
         * @brief Skip whitespace characters
         */
        void          skip_ws();

        /**
         * @brief Peek at the current character without consuming it
         *
         * @return char Current character
         */
        char          peek();

        /**
         * @brief Consume and return the current character
         *
         * @return char Consumed character
         */
        char          consume();

        /**
         * @brief Check if the current character matches the expected character
         *
         * @param c Expected character
         * @return true If the current character matches
         * @return false If the current character does not match
         */
        bool          match(char c);

        /**
         * @brief Read an identifier from the input string
         *
         * @return std::string Read identifier
         */
        std::string   read_ident();

        /**
         * @brief Read a number literal from the input string
         *
         * @return double Read number
         */
        double        read_number_literal();

        /**
         * @brief Parse an expression
         *
         * @return ast::Node::ptr_t Parsed expression
         */
        ast::Node::ptr_t parse_expr();

        /**
         * @brief Parse an addition or subtraction operation
         *
         * @return ast::Node::ptr_t Parsed addition or subtraction
         */
        ast::Node::ptr_t parse_add();

        /**
         * @brief Parse a multiplication or division operation
         *
         * @return ast::Node::ptr_t Parsed multiplication or division
         */
        ast::Node::ptr_t parse_mul();

        /**
         * @brief Parse a power operation
         *
         * @return ast::Node::ptr_t Parsed power
         */
        ast::Node::ptr_t parse_pow();

        /**
         * @brief Parse a unary operation
         *
         * @return ast::Node::ptr_t Parsed unary operation
         */
        ast::Node::ptr_t parse_unary();

        /**
         * @brief Parse a postfix operation
         *
         * @return ast::Node::ptr_t Parsed postfix operation
         */
        ast::Node::ptr_t parse_postfix();

        /**
         * @brief Parse a primary expression
         *
         * @return ast::Node::ptr_t Parsed primary expression
         */
        ast::Node::ptr_t parse_primary();

        /**
         * @brief Parse a function call
         *
         * @param name Function name
         * @return ast::Node::ptr_t Parsed function call
         */
        ast::Node::ptr_t parse_func(const std::string& name);

        /// @brief Input string to parse
        const std::string& m_src;

        /// @brief Current position in the input string
        size_t             m_pos = 0;
};

} // namespace ovb::math
