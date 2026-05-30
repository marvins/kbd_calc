/**
 * @file    constant_node.hpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   AST node for symbolic constants (pi, e, etc.)
 */
#pragma once

// C++ Standard Libraries
#include <cmath>
#include <stdexcept>

// Project Libraries
#include <overboard/math/ast/node.hpp>

namespace ovb::math::ast {

/**
 * @brief Constant node
 *
 * Represents symbolic constants like pi, e, phi, tau.
 */
class Constant_Node : public Node {

    public:

        /**
         * @brief Construct a new Constant_Node object
         *
         * @param c The constant to represent
         */
        explicit Constant_Node(Constant_Id c)
            : Node(Node_Kind::CONSTANT), m_id(c) {}

        /**
         * @brief Get the constant id
         *
         * @return Constant_Id The constant id
         */
        Constant_Id id() const { return m_id; }

        /**
         * @brief Set the constant id
         *
         * @param c The constant id
         */
        void id(Constant_Id c) { m_id = c; }

        /**
         * @brief Evaluate the constant
         *
         * @return double The evaluated constant
         */
        double eval() const override {
            switch (m_id) {
                case Constant_Id::PI:  return M_PI;
                case Constant_Id::E:   return M_E;
                case Constant_Id::PHI: return (1.0 + std::sqrt(5.0)) / 2.0;
                case Constant_Id::TAU: return 2.0 * M_PI;
            }
            return 0.0;
        }

        /**
         * @brief Clone the constant node
         *
         * @return Node::ptr_t The cloned node
         */
        Node::ptr_t clone() const override { return std::make_unique<Constant_Node>(m_id); }

        /**
         * @brief Simplify the constant node
         *
         * @return Node::ptr_t The simplified node
         */
        Node::ptr_t simplify() const override { return clone(); }

        /**
         * @brief Convert the constant to LaTeX
         *
         * @return std::string The LaTeX representation
         */
        std::string to_latex() const override {
            switch (m_id) {
                case Constant_Id::PI:  return "\\pi";
                case Constant_Id::E:   return "e";
                case Constant_Id::PHI: return "\\varphi";
                case Constant_Id::TAU: return "\\tau";
            }
            return "";
        }

        /**
         * @brief Convert the constant to a string
         *
         * @return std::string The string representation
         */
        std::string to_string() const override {
            switch (m_id) {
                case Constant_Id::PI:  return "pi";
                case Constant_Id::E:   return "e";
                case Constant_Id::PHI: return "phi";
                case Constant_Id::TAU: return "tau";
            }
            return "";
        }

    private:

        /// @brief The constant to represent
        Constant_Id m_id;
};

} // namespace ovb::math::ast
