/**
 * @file    node_traits.hpp
 * @author  Marvin Smith
 * @date    2026-05-30
 *
 * @brief   Compile-time traits for AST node types
 *
 * Provides template metaprogramming utilities for type-safe
 * AST operations and compile-time type checking.
 */
#pragma once

// C++ Standard Libraries
#include <array>
#include <cstddef>
#include <vector>

namespace ovb::math::ast {

// Forward declarations
class Number_Node;
class Constant_Node;
class Variable_Node;
class Placeholder_Node;
class Binary_Op_Node;
class Unary_Op_Node;
class Function_Node;
class Factorial_Node;

/**
 * @brief Base trait for all node types
 *
 * Default template assumes node is not a leaf, not an operator,
 * not a function, and has 0 children. Specializations override
 * these values for specific node types.
 */
template<typename Node_Type>
struct Node_Traits {
    static constexpr bool IS_LEAF      { false };
    static constexpr bool IS_OPERATOR  { false };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 0 };
};

/**
 * @brief Trait specialization for Number_Node
 */
template<>
struct Node_Traits<Number_Node> {
    static constexpr bool IS_LEAF      { true };
    static constexpr bool IS_OPERATOR  { false };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 0 };
};

/**
 * @brief Trait specialization for Constant_Node
 */
template<>
struct Node_Traits<Constant_Node> {
    static constexpr bool IS_LEAF      { true };
    static constexpr bool IS_OPERATOR  { false };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 0 };
};

/**
 * @brief Trait specialization for Variable_Node
 */
template<>
struct Node_Traits<Variable_Node> {
    static constexpr bool IS_LEAF      { true };
    static constexpr bool IS_OPERATOR  { false };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 0 };
};

/**
 * @brief Trait specialization for Placeholder_Node
 */
template<>
struct Node_Traits<Placeholder_Node> {
    static constexpr bool IS_LEAF      { true };
    static constexpr bool IS_OPERATOR  { false };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 0 };
};

/**
 * @brief Trait specialization for Binary_Op_Node
 */
template<>
struct Node_Traits<Binary_Op_Node> {
    static constexpr bool IS_LEAF      { false };
    static constexpr bool IS_OPERATOR  { true };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 2 };
};

/**
 * @brief Trait specialization for Unary_Op_Node
 */
template<>
struct Node_Traits<Unary_Op_Node> {
    static constexpr bool IS_LEAF      { false };
    static constexpr bool IS_OPERATOR  { true };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 1 };
};

/**
 * @brief Trait specialization for Function_Node
 *
 * Functions have variable child counts, so we use a sentinel value
 * to indicate dynamic extent.
 */
template<>
struct Node_Traits<Function_Node> {
    static constexpr bool IS_LEAF      { false };
    static constexpr bool IS_OPERATOR  { false };
    static constexpr bool IS_FUNCTION  { true };
    static constexpr size_t CHILD_COUNT { static_cast<size_t>(-1) }; // Dynamic extent
};

/**
 * @brief Trait specialization for Factorial_Node
 */
template<>
struct Node_Traits<Factorial_Node> {
    static constexpr bool IS_LEAF      { false };
    static constexpr bool IS_OPERATOR  { true };
    static constexpr bool IS_FUNCTION  { false };
    static constexpr size_t CHILD_COUNT { 1 };
};

/**
 * @brief Concept for leaf nodes (no children)
 */
template<typename T>
concept Leaf_Node = Node_Traits<T>::IS_LEAF;

/**
 * @brief Concept for operator nodes (binary or unary)
 */
template<typename T>
concept Operator_Node = Node_Traits<T>::IS_OPERATOR;

/**
 * @brief Concept for function nodes (variable child count)
 */
template<typename T>
concept Is_Function_Node = Node_Traits<T>::IS_FUNCTION;

/**
 * @brief Concept for nodes with fixed child count
 */
template<typename T>
concept Has_Fixed_Children = Node_Traits<T>::CHILD_COUNT != static_cast<size_t>(-1);

/**
 * @brief Concept for nodes with variable child count
 */
template<typename T>
concept Has_Variable_Children = Node_Traits<T>::CHILD_COUNT == static_cast<size_t>(-1);

/**
 * @brief Type-safe child access with compile-time bounds checking
 *
 * This function provides compile-time bounds checking for nodes with
 * fixed child counts. For nodes with variable child counts (like functions),
 * it falls back to runtime bounds checking.
 *
 * @tparam Node_Type The type of the node
 * @param node The node to access
 * @param index The child index
 * @return Node* The child node, or nullptr if out of bounds
 */
template<typename Node_Type>
Node* get_child_safe(Node_Type& node, size_t index) {
    static_assert(Has_Fixed_Children<Node_Type> || Has_Variable_Children<Node_Type>,
                  "Node type must have either fixed or variable child count");

    if constexpr (Has_Fixed_Children<Node_Type>) {
        // Compile-time bounds checking for fixed child count nodes
        static_assert(index < Node_Traits<Node_Type>::CHILD_COUNT,
                      "Child index out of bounds at compile time");
        return node.child_at(index);
    } else {
        // Runtime bounds checking for variable child count nodes
        return node.child_at(index);
    }
}

/**
 * @brief Type-safe child access with compile-time bounds checking (const version)
 *
 * @tparam Node_Type The type of the node
 * @param node The node to access
 * @param index The child index
 * @return const Node* The child node, or nullptr if out of bounds
 */
template<typename Node_Type>
const Node* get_child_safe(const Node_Type& node, size_t index) {
    static_assert(Has_Fixed_Children<Node_Type> || Has_Variable_Children<Node_Type>,
                  "Node type must have either fixed or variable child count");

    if constexpr (Has_Fixed_Children<Node_Type>) {
        // Compile-time bounds checking for fixed child count nodes
        static_assert(index < Node_Traits<Node_Type>::CHILD_COUNT,
                      "Child index out of bounds at compile time");
        return node.child_at(index);
    } else {
        // Runtime bounds checking for variable child count nodes
        return node.child_at(index);
    }
}

/**
 * @brief Typed cursor path for compile-time path validation
 *
 * This template provides compile-time type safety for fixed paths through
 * the AST. Useful for tests and operations with known path structures.
 *
 * @tparam Indices... The sequence of child indices
 */
template<size_t... Indices>
struct Cursor_Path {
    static constexpr size_t DEPTH { sizeof...(Indices) };
    static constexpr std::array<size_t, DEPTH> PATH { {Indices...} };

    /**
     * @brief Get the path as a vector for runtime use
     */
    static constexpr std::vector<size_t> to_vector() {
        return std::vector<size_t>(PATH.begin(), PATH.end());
    }
};

/**
 * @brief Runtime cursor path for dynamic navigation
 *
 * This class provides a runtime cursor path that can be modified
 * during cursor navigation operations.
 */
class Cursor_Path_Runtime {
    public:
        Cursor_Path_Runtime() = default;

        explicit Cursor_Path_Runtime(const std::vector<size_t>& path)
            : m_path(path) {}

        /**
         * @brief Get the current path
         */
        const std::vector<size_t>& path() const { return m_path; }

        /**
         * @brief Get the current path (mutable)
         */
        std::vector<size_t>& path() { return m_path; }

        /**
         * @brief Get the depth of the path
         */
        size_t depth() const { return m_path.size(); }

        /**
         * @brief Push a child index onto the path
         */
        void push(size_t index) { m_path.push_back(index); }

        /**
         * @brief Pop the last child index from the path
         */
        void pop() {
            if (!m_path.empty()) {
                m_path.pop_back();
            }
        }

        /**
         * @brief Get the parent path (path without last element)
         */
        Cursor_Path_Runtime parent_path() const {
            if (m_path.empty()) {
                return Cursor_Path_Runtime();
            }
            std::vector<size_t> parent_vec(m_path.begin(), m_path.end() - 1);
            return Cursor_Path_Runtime(parent_vec);
        }

        /**
         * @brief Clear the path
         */
        void clear() { m_path.clear(); }

        /**
         * @brief Check if path is empty
         */
        bool empty() const { return m_path.empty(); }

        /**
         * @brief Get child index at given depth
         */
        size_t operator[](size_t index) const { return m_path[index]; }

        /**
         * @brief Compare two paths for equality
         */
        bool operator==(const Cursor_Path_Runtime& other) const {
            return m_path == other.m_path;
        }

        /**
         * @brief Compare two paths for inequality
         */
        bool operator!=(const Cursor_Path_Runtime& other) const {
            return m_path != other.m_path;
        }

    private:
        std::vector<size_t> m_path;
};

/**
 * @brief Get node at path in AST tree
 *
 * Navigates the AST from the root following the given path and returns
 * the node at that position. Returns nullptr if the path is invalid.
 *
 * @param root The root node of the AST
 * @param path The cursor path to navigate
 * @return Node* The node at the path, or nullptr if invalid
 */
inline Node* get_node_at_path(Node* root, const std::vector<size_t>& path) {
    if (!root) {
        return nullptr;
    }

    Node* current = root;
    for (size_t index : path) {
        current = current->child_at(index);
        if (!current) {
            return nullptr;
        }
    }
    return current;
}

/**
 * @brief Get node at path in AST tree (const version)
 *
 * @param root The root node of the AST
 * @param path The cursor path to navigate
 * @return const Node* The node at the path, or nullptr if invalid
 */
inline const Node* get_node_at_path(const Node* root, const std::vector<size_t>& path) {
    if (!root) {
        return nullptr;
    }

    const Node* current = root;
    for (size_t index : path) {
        current = current->child_at(index);
        if (!current) {
            return nullptr;
        }
    }
    return current;
}

/**
 * @brief Get node at path using Cursor_Path_Runtime
 *
 * @param root The root node of the AST
 * @param cursor_path The cursor path to navigate
 * @return Node* The node at the path, or nullptr if invalid
 */
inline Node* get_node_at_path(Node* root, const Cursor_Path_Runtime& cursor_path) {
    return get_node_at_path(root, cursor_path.path());
}

/**
 * @brief Get node at path using Cursor_Path_Runtime (const version)
 *
 * @param root The root node of the AST
 * @param cursor_path The cursor path to navigate
 * @return const Node* The node at the path, or nullptr if invalid
 */
inline const Node* get_node_at_path(const Node* root, const Cursor_Path_Runtime& cursor_path) {
    return get_node_at_path(root, cursor_path.path());
}

/**
 * @brief Move cursor left in the AST tree
 *
 * Moves the cursor to the previous logical position in the tree.
 * This is a complex operation that depends on the tree structure
 * and the current cursor position.
 *
 * @param root The root node of the AST
 * @param current_path The current cursor path
 * @return Cursor_Path_Runtime The new cursor path after moving left
 */
inline Cursor_Path_Runtime cursor_left(const Node* root, const Cursor_Path_Runtime& current_path) {
    if (!root) {
        return current_path;
    }

    // If path is empty, try to move to last child of root
    if (current_path.empty()) {
        if (root->child_count() > 0) {
            Cursor_Path_Runtime new_path;
            new_path.push(root->child_count() - 1);
            return new_path;
        }
        return current_path;
    }

    // Get the current node
    const Node* current = get_node_at_path(root, current_path);
    if (!current) {
        return current_path;
    }

    // If we're at a leaf node, try to move to previous sibling
    if (current->child_count() == 0) {
        Cursor_Path_Runtime new_path = current_path;
        if (new_path.depth() > 0) {
            size_t last_index = new_path[new_path.depth() - 1];
            if (last_index > 0) {
                // Move to previous sibling
                new_path.path()[new_path.depth() - 1] = last_index - 1;
                return new_path;
            } else {
                // Move up to parent
                new_path.pop();
                return new_path;
            }
        }
        return current_path;
    }

    // If we're at an internal node, move to last child
    Cursor_Path_Runtime new_path = current_path;
    new_path.push(current->child_count() - 1);
    return new_path;
}

/**
 * @brief Move cursor right in the AST tree
 *
 * Moves the cursor to the next logical position in the tree.
 * This is a complex operation that depends on the tree structure
 * and the current cursor position.
 *
 * @param root The root node of the AST
 * @param current_path The current cursor path
 * @return Cursor_Path_Runtime The new cursor path after moving right
 */
inline Cursor_Path_Runtime cursor_right(const Node* root, const Cursor_Path_Runtime& current_path) {
    if (!root) {
        return current_path;
    }

    // If path is empty, try to move to first child of root
    if (current_path.empty()) {
        if (root->child_count() > 0) {
            Cursor_Path_Runtime new_path;
            new_path.push(0);
            return new_path;
        }
        return current_path;
    }

    // Get the current node
    const Node* current = get_node_at_path(root, current_path);
    if (!current) {
        return current_path;
    }

    // If we're at a leaf node, try to move to next sibling
    if (current->child_count() == 0) {
        Cursor_Path_Runtime new_path = current_path;
        size_t last_index = new_path[new_path.depth() - 1];

        // Get parent to check if there's a next sibling
        new_path.pop();
        const Node* parent = get_node_at_path(root, new_path);
        if (parent && last_index + 1 < parent->child_count()) {
            // Move to next sibling
            new_path.push(last_index + 1);
            return new_path;
        }
        return current_path;
    }

    // If we're at an internal node, move to first child
    Cursor_Path_Runtime new_path = current_path;
    new_path.push(0);
    return new_path;
}

} // namespace ovb::math::ast
