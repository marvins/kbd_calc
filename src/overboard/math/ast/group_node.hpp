/**
 * @file    group_node.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   AST node for parenthesis grouping
 *
 * Represents explicit grouping with parentheses — (child).
 * Used for editing mode where user opens a group and types inside.
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Group node — parenthesis wrapper
 *
 * Wraps a child expression with parentheses for explicit grouping.
 * Evaluates to the child's value (parens don't change semantics).
 */
class Group_Node : public Node {

    public:

        /**
         * @brief Construct a new Group_Node object
         *
         * @param child The child expression to wrap
         */
        explicit Group_Node(Node::ptr_t child)
            : Node(Node_Kind::GROUP), m_child(std::move(child)) {}

        /**
         * @brief Get the child
         *
         * @return const Node::ptr_t& The child
         */
        const Node::ptr_t& child() const { return m_child; }

        /**
         * @brief Release ownership of the child
         *
         * @return Node::ptr_t The released child
         */
        Node::ptr_t release_child() { return std::move(m_child); }

        /**
         * @brief Set a new child
         *
         * @param new_child The new child to set
         */
        void set_child(Node::ptr_t new_child) { m_child = std::move(new_child); }

        /**
         * @brief Evaluate the group (returns child's value)
         *
         * @return double The child's evaluated result
         */
        double eval() const override {
            return m_child->eval();
        }

        /**
         * @brief Convert the group to LaTeX
         *
         * @return std::string The LaTeX representation
         */
        std::string to_latex() const override {
            return "(" + m_child->to_latex() + ")";
        }

        /**
         * @brief Convert the group to a string
         *
         * @return std::string The string representation with parens
         */
        std::string to_string() const override {
            return "(" + m_child->to_string() + ")";
        }

        /**
         * @brief Clone the group node
         *
         * @return Node::ptr_t The cloned node
         */
        Node::ptr_t clone() const override {
            return std::make_unique<Group_Node>(m_child->clone());
        }

        /**
         * @brief Simplify the group node
         *
         * @return Node::ptr_t The simplified node (child if group not needed)
         */
        Node::ptr_t simplify() const override {
            auto simplified_child = m_child->simplify();
            // If child is a number, we can drop the parentheses
            if (simplified_child->kind() == Node_Kind::NUMBER) {
                return simplified_child;
            }
            return std::make_unique<Group_Node>(std::move(simplified_child));
        }

        /**
         * @brief Get the number of children (always 1 for groups)
         *
         * @return size_t Number of children
         */
        size_t child_count() const override { return 1; }

        /**
         * @brief Get child node at given index
         *
         * @param index Child index (0 for the wrapped child)
         * @return Node* Child node (nullptr if index out of bounds)
         */
        Node* child_at(size_t index) override {
            if (index == 0) return m_child.get();
            return nullptr;
        }

        /**
         * @brief Get child node at given index (const version)
         *
         * @param index Child index (0 for the wrapped child)
         * @return const Node* Child node (nullptr if index out of bounds)
         */
        const Node* child_at(size_t index) const override {
            if (index == 0) return m_child.get();
            return nullptr;
        }

    private:

        /// @brief The wrapped child expression
        Node::ptr_t m_child;
};

} // namespace ovb::math::ast
