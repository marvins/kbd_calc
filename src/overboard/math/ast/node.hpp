/**
 * @file    node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   Base AST node class
 *
 * All expression AST nodes inherit from this base class.
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

namespace ovb::math::ast {

// Node kinds
enum class Node_Kind {
    NUMBER,       // literal double
    CONSTANT,     // symbolic constant — pi, e
    VARIABLE,     // variable name — x, y, n
    PLACEHOLDER,  // empty box for incomplete expressions
    BINARY_OP,    // left op right
    UNARY_OP,     // op operand
    FUNCTION,     // named function, one or more args
    FACTORIAL,    // n!
    GROUP,        // parenthesis grouping — (child)
};

// Binary operations
enum class Binary_Op {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    POWER,
    MODULO,
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,
};

// Unary operations
enum class Unary_Op {
    NEGATE,
    BIT_NOT,
    PERCENT,
};

// Constant identifiers
enum class Constant_Id {
    PI,
    E,
    PHI,
    TAU,
};

/**
 * @brief Base AST node
 *
 * All expression AST nodes inherit from this base class.
 * Provides interface for evaluation, string conversion,
 * simplification, and cloning.
 */
class Node {

    public:

        /// Pointer type
        using ptr_t = std::unique_ptr<Node>;

        /**
         * @brief Construct a new Node object
         *
         * @param k Node kind
         */
        explicit Node(Node_Kind k)
            : m_kind(k) {}

        /**
         * @brief Destroy the Node object
         */
        virtual ~Node() = default;

        /**
         * @brief Get the node kind
         *
         * @return Node_Kind Node kind
         */
        Node_Kind kind() const { return m_kind; }

        /**
         * @brief Set the node kind
         *
         * @param k Node kind
         */
        void kind(Node_Kind k) { m_kind = k; }

        /**
         * @brief Evaluate the node
         *
         * @return double Evaluated result
         */
        virtual double eval() const = 0;

        /**
         * @brief Convert to LaTeX representation
         *
         * @return std::string LaTeX representation
         */
        virtual std::string to_latex() const = 0;

        /**
         * @brief Convert to string representation
         *
         * @return std::string String representation
         */
        virtual std::string to_string() const = 0;

        /**
         * @brief Simplify the node
         *
         * @return Node::ptr_t Simplified node
         */
        virtual Node::ptr_t simplify() const = 0;

        /**
         * @brief Clone the node
         *
         * @return Node::ptr_t Cloned node
         */
        virtual Node::ptr_t clone() const = 0;

        /**
         * @brief Get the number of children this node has
         *
         * @return size_t Number of children (0 for leaf nodes)
         */
        virtual size_t child_count() const = 0;

        /**
         * @brief Get child node at given index
         *
         * @param index Child index
         * @return Node* Child node (nullptr if index out of bounds)
         */
        virtual Node* child_at( size_t index ) = 0;

        /**
         * @brief Get child node at given index (const version)
         *
         * @param index Child index
         * @return const Node* Child node (nullptr if index out of bounds)
         */
        virtual const Node* child_at( size_t index ) const = 0;

    private:

        /// @brief Node kind
        Node_Kind m_kind;

}; // End of Node class

} // namespace ovb::math::ast
