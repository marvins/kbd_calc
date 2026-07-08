/**
 * @file    matrix_node.cpp
 * @author  Marvin Smith
 * @date    2026-07-07
 *
 * @brief   Matrix_Node implementation
 */
#include <overboard/math/ast/matrix_node.hpp>

// C++ Standard Libraries
#include <cmath>
#include <limits>

// Project Libraries
#include <overboard/math/ast/number_node.hpp>
#include <overboard/math/ast/placeholder_node.hpp>

namespace ovb::math::ast {

/***************************/
/*  Constructor (empty)    */
/***************************/
Matrix_Node::Matrix_Node( int rows, int cols )
    : Node(Node_Kind::MATRIX)
    , m_rows(rows)
    , m_cols(cols) {
    m_cells.reserve(static_cast<std::size_t>(rows * cols));
    for (int i = 0; i < rows * cols; ++i) {
        m_cells.push_back(std::make_unique<Placeholder_Node>());
    }
}

/***************************/
/*  Constructor (cells)    */
/***************************/
Matrix_Node::Matrix_Node( std::vector<Node::ptr_t> cells, int rows, int cols )
    : Node(Node_Kind::MATRIX)
    , m_rows(rows)
    , m_cols(cols)
    , m_cells(std::move(cells)) {}

/***************************/
/*         Evaluate        */
/***************************/
double Matrix_Node::eval() const {
    return std::numeric_limits<double>::quiet_NaN();
}

/***************************/
/*        To String        */
/***************************/
std::string Matrix_Node::to_string() const {
    std::string s = "[";
    for (int r = 0; r < m_rows; ++r) {
        if (r > 0) s += ";";
        s += "[";
        for (int c = 0; c < m_cols; ++c) {
            if (c > 0) s += ",";
            s += m_cells[static_cast<std::size_t>(r * m_cols + c)]->to_string();
        }
        s += "]";
    }
    return s + "]";
}

/***************************/
/*        To LaTeX         */
/***************************/
std::string Matrix_Node::to_latex() const {
    std::string s = "\\begin{bmatrix}";
    for (int r = 0; r < m_rows; ++r) {
        if (r > 0) s += " \\\\ ";
        for (int c = 0; c < m_cols; ++c) {
            if (c > 0) s += " & ";
            s += m_cells[static_cast<std::size_t>(r * m_cols + c)]->to_latex();
        }
    }
    return s + "\\end{bmatrix}";
}

/***************************/
/*          Clone          */
/***************************/
Node::ptr_t Matrix_Node::clone() const {
    std::vector<Node::ptr_t> cells;
    cells.reserve(m_cells.size());
    for (const auto& cell : m_cells) {
        cells.push_back(cell->clone());
    }
    return std::make_unique<Matrix_Node>(std::move(cells), m_rows, m_cols);
}

/***************************/
/*         Simplify        */
/***************************/
Node::ptr_t Matrix_Node::simplify() const {
    std::vector<Node::ptr_t> cells;
    cells.reserve(m_cells.size());
    for (const auto& cell : m_cells) {
        cells.push_back(cell->simplify());
    }
    return std::make_unique<Matrix_Node>(std::move(cells), m_rows, m_cols);
}

/***************************/
/*       Child At          */
/***************************/
Node* Matrix_Node::child_at( size_t index ) {
    if (index < m_cells.size()) return m_cells[index].get();
    return nullptr;
}

const Node* Matrix_Node::child_at( size_t index ) const {
    if (index < m_cells.size()) return m_cells[index].get();
    return nullptr;
}

/***************************/
/*     Release Child       */
/***************************/
Node::ptr_t Matrix_Node::release_child( size_t index ) {
    if (index < m_cells.size()) {
        Node::ptr_t node = std::move(m_cells[index]);
        m_cells[index] = std::make_unique<Placeholder_Node>();
        return node;
    }
    return nullptr;
}

/***************************/
/*       Set Child         */
/***************************/
void Matrix_Node::set_child( size_t index, Node::ptr_t node ) {
    if (index < m_cells.size()) {
        m_cells[index] = std::move(node);
    }
}

} // namespace ovb::math::ast
