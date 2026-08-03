/**
 * @file    matrix_node.hpp
 * @author  Marvin Smith
 * @date    2026-07-07
 *
 * @brief   AST node for matrix literals with editable cells
 *
 * Represents an m×n matrix whose cells are individual AST nodes
 * (typically placeholders until the user types values).
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Matrix literal node
 *
 * Stores a 2-D grid of child nodes in row-major order.
 * Children are accessed via child_at(row * cols + col).
 */
class Matrix_Node : public Node {

    public:

        /**
         * @brief Construct a Matrix_Node filled with placeholders
         *
         * @param rows Row count
         * @param cols Column count
         */
        Matrix_Node( int rows, int cols );

        /**
         * @brief Construct a Matrix_Node with explicit cells
         *
         * @param cells  Cell nodes in row-major order (size must be rows*cols)
         * @param rows   Row count
         * @param cols   Column count
         */
        Matrix_Node( std::vector<Node::ptr_t> cells, int rows, int cols );

        /**
         * @brief Get row count
         * @return int Number of rows
         */
        int rows() const { return m_rows; }

        /**
         * @brief Get column count
         * @return int Number of columns
         */
        int cols() const { return m_cols; }

        /**
         * @brief Evaluate the matrix node
         *
         * Matrix evaluation is not supported in the scalar evaluator.
         * Returns NaN.
         *
         * @return double NaN
         */
        double eval() const override;

        /**
         * @brief Convert to LaTeX representation
         * @return std::string LaTeX matrix representation
         */
        std::string to_latex() const override;

        /**
         * @brief Convert to string representation
         * @return std::string String representation like [[a,b],[c,d]]
         */
        std::string to_string() const override;

        /**
         * @brief Clone the matrix node
         * @return Node::ptr_t Cloned node
         */
        Node::ptr_t clone() const override;

        /**
         * @brief Simplify the matrix node
         * @return Node::ptr_t Simplified node
         */
        Node::ptr_t simplify() const override;

        /**
         * @brief Get the number of children (rows * cols)
         * @return size_t Number of cells
         */
        size_t child_count() const override { return m_cells.size(); }

        /**
         * @brief Get child node at given flat index
         * @param index Flat index (row * cols + col)
         * @return Node* Child node (nullptr if out of bounds)
         */
        Node* child_at( size_t index ) override;

        /**
         * @brief Get child node at given flat index (const)
         * @param index Flat index (row * cols + col)
         * @return const Node* Child node (nullptr if out of bounds)
         */
        const Node* child_at( size_t index ) const override;

        /**
         * @brief Release a child node (for tree restructuring)
         * @param index Flat index
         * @return Node::ptr_t Released child node
         */
        Node::ptr_t release_child( size_t index );

        /**
         * @brief Set a child node at a specific index
         * @param index Flat index
         * @param node New child node
         */
        void set_child( size_t index, Node::ptr_t node );

    private:

        /// @brief Row count
        int m_rows;

        /// @brief Column count
        int m_cols;

        /// @brief Cell nodes in row-major order
        std::vector<Node::ptr_t> m_cells;
};

} // namespace ovb::math::ast
