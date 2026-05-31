// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/ast/binary_op_node.hpp>
#include <overboard/math/ast/constant_node.hpp>
#include <overboard/math/ast/factorial_node.hpp>
#include <overboard/math/ast/function_node.hpp>
#include <overboard/math/ast/node.hpp>
#include <overboard/math/ast/node_traits.hpp>
#include <overboard/math/ast/number_node.hpp>
#include <overboard/math/ast/placeholder_node.hpp>
#include <overboard/math/ast/unary_op_node.hpp>
#include <overboard/math/ast/variable_node.hpp>

using namespace ovb::math::ast;

// ─── Number_Node ─────────────────────────────────────────────────────────────

TEST(Ast_Number, Eval) {
    Number_Node n(42.0);
    EXPECT_DOUBLE_EQ(n.eval(), 42.0);
}

TEST(Ast_Number, To_String_Integer) {
    EXPECT_EQ(Number_Node(7.0).to_string(), "7");
}

TEST(Ast_Number, To_Latex_Integer) {
    EXPECT_EQ(Number_Node(3.0).to_latex(), "3");
}

// ─── Constant_Node ───────────────────────────────────────────────────────────

TEST(Ast_Constant, Pi_Eval) {
    Constant_Node c(Constant_Id::PI);
    EXPECT_NEAR(c.eval(), 3.14159265358979, 1e-10);
}

TEST(Ast_Constant, E_Eval) {
    Constant_Node c(Constant_Id::E);
    EXPECT_NEAR(c.eval(), 2.71828182845904, 1e-10);
}

TEST(Ast_Constant, Pi_Latex) {
    EXPECT_EQ(Constant_Node(Constant_Id::PI).to_latex(), "\\pi");
}

TEST(Ast_Constant, E_Latex) {
    EXPECT_EQ(Constant_Node(Constant_Id::E).to_latex(), "e");
}

// ─── Binary_Op_Node ──────────────────────────────────────────────────────────

TEST(Ast_Binary, Add) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_DOUBLE_EQ(n->eval(), 7.0);
}

TEST(Ast_Binary, Subtract) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::SUBTRACT,
        std::make_unique<Number_Node>(10.0),
        std::make_unique<Number_Node>(3.0));
    EXPECT_DOUBLE_EQ(n->eval(), 7.0);
}

TEST(Ast_Binary, Multiply) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(6.0),
        std::make_unique<Number_Node>(7.0));
    EXPECT_DOUBLE_EQ(n->eval(), 42.0);
}

TEST(Ast_Binary, Divide) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::DIVIDE,
        std::make_unique<Number_Node>(10.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_DOUBLE_EQ(n->eval(), 2.5);
}

TEST(Ast_Binary, Divide_By_Zero) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::DIVIDE,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(0.0));
    EXPECT_THROW(n->eval(), std::runtime_error);
}

TEST(Ast_Binary, Power) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::POWER,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 1024.0);
}

TEST(Ast_Binary, Modulo) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MODULO,
        std::make_unique<Number_Node>(10.0),
        std::make_unique<Number_Node>(3.0));
    EXPECT_DOUBLE_EQ(n->eval(), 1.0);
}

TEST(Ast_Binary, Bit_And) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::BIT_AND,
        std::make_unique<Number_Node>(12.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 8.0); // 1100 & 1010 = 1000
}

TEST(Ast_Binary, Bit_Or) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::BIT_OR,
        std::make_unique<Number_Node>(12.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 14.0); // 1100 | 1010 = 1110
}

TEST(Ast_Binary, Bit_Xor) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::BIT_XOR,
        std::make_unique<Number_Node>(12.0),
        std::make_unique<Number_Node>(10.0));
    EXPECT_DOUBLE_EQ(n->eval(), 6.0); // 1100 ^ 1010 = 0110
}

TEST(Ast_Binary, Shift_Left) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::SHIFT_LEFT,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_DOUBLE_EQ(n->eval(), 16.0);
}

TEST(Ast_Binary, Shift_Right) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::SHIFT_RIGHT,
        std::make_unique<Number_Node>(16.0),
        std::make_unique<Number_Node>(2.0));
    EXPECT_DOUBLE_EQ(n->eval(), 4.0);
}

// ─── Binary to_latex ─────────────────────────────────────────────────────────

TEST(Ast_Binary, Divide_Latex) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::DIVIDE,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(2.0));
    EXPECT_EQ(n->to_latex(), "\\frac{1}{2}");
}

TEST(Ast_Binary, Power_Latex) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::POWER,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(8.0));
    EXPECT_EQ(n->to_latex(), "2^{8}");
}

TEST(Ast_Binary, Multiply_Latex) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_EQ(n->to_latex(), "3 \\cdot 4");
}

// ─── Unary_Op_Node ───────────────────────────────────────────────────────────

TEST(Ast_Unary, Negate) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::NEGATE,
        std::make_unique<Number_Node>(5.0));
    EXPECT_DOUBLE_EQ(n->eval(), -5.0);
}

TEST(Ast_Unary, Percent) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::PERCENT,
        std::make_unique<Number_Node>(50.0));
    EXPECT_DOUBLE_EQ(n->eval(), 0.5);
}

TEST(Ast_Unary, Bit_Not) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::BIT_NOT,
        std::make_unique<Number_Node>(0.0));
    EXPECT_DOUBLE_EQ(n->eval(), -1.0); // ~0 = -1 (two's complement)
}

// ─── Function_Node ───────────────────────────────────────────────────────────

TEST(Ast_Function, Sin) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(0.0));
    EXPECT_DOUBLE_EQ(Function_Node("sin", std::move(args)).eval(), 0.0);
}

TEST(Ast_Function, Cos) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(0.0));
    EXPECT_DOUBLE_EQ(Function_Node("cos", std::move(args)).eval(), 1.0);
}

TEST(Ast_Function, Sqrt) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(9.0));
    EXPECT_DOUBLE_EQ(Function_Node("sqrt", std::move(args)).eval(), 3.0);
}

TEST(Ast_Function, Log) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(100.0));
    EXPECT_DOUBLE_EQ(Function_Node("log", std::move(args)).eval(), 2.0);
}

TEST(Ast_Function, Ln) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(1.0));
    EXPECT_DOUBLE_EQ(Function_Node("ln", std::move(args)).eval(), 0.0);
}

TEST(Ast_Function, Unknown_Throws) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(1.0));
    EXPECT_THROW(Function_Node("foo", std::move(args)).eval(), std::runtime_error);
}

TEST(Ast_Function, Sqrt_Latex) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(2.0));
    EXPECT_EQ(Function_Node("sqrt", std::move(args)).to_latex(), "\\sqrt{2}");
}

TEST(Ast_Function, Sin_Latex) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(0.0));
    EXPECT_EQ(Function_Node("sin", std::move(args)).to_latex(), "\\sin\\left(0\\right)");
}

// ─── Factorial_Node ──────────────────────────────────────────────────────────

TEST(Ast_Factorial, Basic) {
    EXPECT_DOUBLE_EQ(
        Factorial_Node(std::make_unique<Number_Node>(5.0)).eval(), 120.0);
}

TEST(Ast_Factorial, Zero) {
    EXPECT_DOUBLE_EQ(
        Factorial_Node(std::make_unique<Number_Node>(0.0)).eval(), 1.0);
}

TEST(Ast_Factorial, Negative_Throws) {
    EXPECT_THROW(
        Factorial_Node(std::make_unique<Number_Node>(-1.0)).eval(),
        std::runtime_error);
}

TEST(Ast_Factorial, To_String) {
    EXPECT_EQ(
        Factorial_Node(std::make_unique<Number_Node>(5.0)).to_string(), "5!");
}

// ─── simplify() ──────────────────────────────────────────────────────────────

TEST(Ast_Simplify, Number_Returns_Clone) {
    Number_Node n(3.0);
    auto s = n.simplify();
    EXPECT_EQ(s->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 3.0);
}

TEST(Ast_Simplify, Constant_Stays_Symbolic) {
    Constant_Node c(Constant_Id::PI);
    auto s = c.simplify();
    EXPECT_EQ(s->kind(), Node_Kind::CONSTANT);
    EXPECT_EQ(s->to_string(), "pi");
}

TEST(Ast_Simplify, Binary_Constant_Folds_Two_Numbers) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 7.0);
}

TEST(Ast_Simplify, Binary_Stays_Symbolic_With_Constant) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Constant_Node>(Constant_Id::PI));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::BINARY_OP);
    EXPECT_EQ(s->to_string(), "(2*pi)");
}

TEST(Ast_Simplify, Function_Folds_Numeric_Sqrt) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(9.0));
    auto n = std::make_unique<Function_Node>("sqrt", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 3.0);
}

TEST(Ast_Simplify, Function_Stays_Symbolic_With_Constant_Arg) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Constant_Node>(Constant_Id::PI));
    auto n = std::make_unique<Function_Node>("sin", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::FUNCTION);
    EXPECT_EQ(s->to_string(), "sin(pi)");
}

// ─── approx() ────────────────────────────────────────────────────────────────

TEST(Ast_Approx, Forces_Numeric_Eval_Of_Constant) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Constant_Node>(Constant_Id::PI));
    auto n = std::make_unique<Function_Node>("approx", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::NUMBER);
    EXPECT_NEAR(s->eval(), 3.14159265358979, 1e-10);
}

TEST(Ast_Approx, Forces_Numeric_Eval_Of_Symbolic_Expression) {
    std::vector<Node::ptr_t> inner_args;
    inner_args.push_back(std::make_unique<Constant_Node>(Constant_Id::PI));
    auto sin_pi = std::make_unique<Function_Node>("sin", std::move(inner_args));

    std::vector<Node::ptr_t> args;
    args.push_back(std::move(sin_pi));
    auto n = std::make_unique<Function_Node>("approx", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::NUMBER);
    EXPECT_NEAR(s->eval(), 0.0, 1e-10); // sin(π) ≈ 0
}

TEST(Ast_Approx, Two_Plus_Two_Stays_Four) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(2.0)));
    auto n = std::make_unique<Function_Node>("approx", std::move(args));
    auto s = n->simplify();
    EXPECT_EQ(s->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(s->eval(), 4.0);
}

// ─── clone() ─────────────────────────────────────────────────────────────────

TEST(Ast_Clone, Number_Clone_Is_Independent) {
    auto orig = std::make_unique<Number_Node>(99.0);
    auto copy = orig->clone();
    EXPECT_DOUBLE_EQ(copy->eval(), 99.0);
    EXPECT_NE(orig.get(), copy.get());
}

TEST(Ast_Clone, Binary_Deep_Clone) {
    auto orig = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(1.0),
        std::make_unique<Number_Node>(2.0));
    auto copy = orig->clone();
    EXPECT_DOUBLE_EQ(copy->eval(), 3.0);
    EXPECT_NE(orig.get(), copy.get());
}

TEST(Ast_Clone, Constant_Clone_Stays_Symbolic) {
    auto orig = std::make_unique<Constant_Node>(Constant_Id::E);
    auto copy = orig->clone();
    EXPECT_EQ(copy->to_string(), "e");
    EXPECT_EQ(copy->kind(), Node_Kind::CONSTANT);
}

// ─── child_count() ─────────────────────────────────────────────────────────────

TEST(Ast_Child_Count, Number_Node_Has_Zero_Children) {
    Number_Node n(42.0);
    EXPECT_EQ(n.child_count(), 0);
}

TEST(Ast_Child_Count, Constant_Node_Has_Zero_Children) {
    Constant_Node c(Constant_Id::PI);
    EXPECT_EQ(c.child_count(), 0);
}

TEST(Ast_Child_Count, Variable_Node_Has_Zero_Children) {
    Variable_Node v("x");
    EXPECT_EQ(v.child_count(), 0);
}

TEST(Ast_Child_Count, Placeholder_Node_Has_Zero_Children) {
    Placeholder_Node p;
    EXPECT_EQ(p.child_count(), 0);
}

TEST(Ast_Child_Count, Binary_Op_Node_Has_Two_Children) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_EQ(n->child_count(), 2);
}

TEST(Ast_Child_Count, Unary_Op_Node_Has_One_Child) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::NEGATE,
        std::make_unique<Number_Node>(5.0));
    EXPECT_EQ(n->child_count(), 1);
}

TEST(Ast_Child_Count, Function_Node_Has_Variable_Children) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(1.0));
    args.push_back(std::make_unique<Number_Node>(2.0));
    Function_Node f("max", std::move(args));
    EXPECT_EQ(f.child_count(), 2);
}

TEST(Ast_Child_Count, Factorial_Node_Has_One_Child) {
    Factorial_Node f(std::make_unique<Number_Node>(5.0));
    EXPECT_EQ(f.child_count(), 1);
}

// ─── child_at() ────────────────────────────────────────────────────────────────

TEST(Ast_Child_At, Leaf_Node_Returns_Nullptr) {
    Number_Node n(42.0);
    EXPECT_EQ(n.child_at(0), nullptr);
    EXPECT_EQ(n.child_at(1), nullptr);
}

TEST(Ast_Child_At, Binary_Op_Node_Left_Child) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    const Node* left = n->child_at(0);
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(left)->value(), 3.0);
}

TEST(Ast_Child_At, Binary_Op_Node_Right_Child) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    const Node* right = n->child_at(1);
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(right)->value(), 4.0);
}

TEST(Ast_Child_At, Binary_Op_Node_Out_Of_Bounds) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));
    EXPECT_EQ(n->child_at(2), nullptr);
    EXPECT_EQ(n->child_at(100), nullptr);
}

TEST(Ast_Child_At, Unary_Op_Node_Child) {
    auto n = std::make_unique<Unary_Op_Node>(
        Unary_Op::NEGATE,
        std::make_unique<Number_Node>(5.0));
    const Node* child = n->child_at(0);
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(child)->value(), 5.0);
}

TEST(Ast_Child_At, Function_Node_Children) {
    std::vector<Node::ptr_t> args;
    args.push_back(std::make_unique<Number_Node>(1.0));
    args.push_back(std::make_unique<Number_Node>(2.0));
    Function_Node f("max", std::move(args));

    const Node* first = f.child_at(0);
    ASSERT_NE(first, nullptr);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(first)->value(), 1.0);

    const Node* second = f.child_at(1);
    ASSERT_NE(second, nullptr);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(second)->value(), 2.0);
}

// ─── get_node_at_path() ────────────────────────────────────────────────────────

TEST(Ast_Get_Node_At_Path, Null_Root_Returns_Nullptr) {
    Cursor_Path_Runtime path;
    path.push(0);
    EXPECT_EQ(get_node_at_path(static_cast<Node*>(nullptr), path), nullptr);
}

TEST(Ast_Get_Node_At_Path, Empty_Path_Returns_Root) {
    Number_Node n(42.0);
    Cursor_Path_Runtime path;
    const Node* result = get_node_at_path(&n, path);
    EXPECT_EQ(result, &n);
}

TEST(Ast_Get_Node_At_Path, Simple_Path_To_Binary_Child) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(0);

    const Node* result = get_node_at_path(n.get(), path);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(result)->value(), 3.0);
}

TEST(Ast_Get_Node_At_Path, Nested_Path) {
    auto inner = std::make_unique<Binary_Op_Node>(
        Binary_Op::MULTIPLY,
        std::make_unique<Number_Node>(2.0),
        std::make_unique<Number_Node>(3.0));

    auto outer = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::move(inner),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(0);  // Left child of outer (the multiply)
    path.push(1);  // Right child of inner (the 3.0)

    const Node* result = get_node_at_path(outer.get(), path);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->kind(), Node_Kind::NUMBER);
    EXPECT_DOUBLE_EQ(static_cast<const Number_Node*>(result)->value(), 3.0);
}

TEST(Ast_Get_Node_At_Path, Invalid_Path_Returns_Nullptr) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(0);
    path.push(0);  // Try to go deeper than possible

    const Node* result = get_node_at_path(n.get(), path);
    EXPECT_EQ(result, nullptr);
}

// ─── Cursor_Path_Runtime ───────────────────────────────────────────────────────

TEST(Ast_Cursor_Path_Runtime, Default_Constructor) {
    Cursor_Path_Runtime path;
    EXPECT_TRUE(path.empty());
    EXPECT_EQ(path.depth(), 0);
}

TEST(Ast_Cursor_Path_Runtime, Vector_Constructor) {
    std::vector<size_t> vec = {0, 1, 2};
    Cursor_Path_Runtime path(vec);
    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.depth(), 3);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[1], 1);
    EXPECT_EQ(path[2], 2);
}

TEST(Ast_Cursor_Path_Runtime, Push_And_Pop) {
    Cursor_Path_Runtime path;
    path.push(0);
    EXPECT_EQ(path.depth(), 1);
    EXPECT_EQ(path[0], 0);

    path.push(1);
    EXPECT_EQ(path.depth(), 2);
    EXPECT_EQ(path[1], 1);

    path.pop();
    EXPECT_EQ(path.depth(), 1);

    path.pop();
    EXPECT_TRUE(path.empty());
}

TEST(Ast_Cursor_Path_Runtime, Clear) {
    Cursor_Path_Runtime path;
    path.push(0);
    path.push(1);
    path.clear();
    EXPECT_TRUE(path.empty());
}

TEST(Ast_Cursor_Path_Runtime, Equality) {
    Cursor_Path_Runtime path1;
    path1.push(0);
    path1.push(1);

    Cursor_Path_Runtime path2;
    path2.push(0);
    path2.push(1);

    EXPECT_EQ(path1, path2);

    path2.push(2);
    EXPECT_NE(path1, path2);
}

// ─── cursor_left() ─────────────────────────────────────────────────────────────

TEST(Ast_Cursor_Left, From_Empty_Path_Stays_Empty) {
    Number_Node n(42.0);
    Cursor_Path_Runtime path;
    Cursor_Path_Runtime result = cursor_left(&n, path);
    EXPECT_TRUE(result.empty());
}

TEST(Ast_Cursor_Left, From_Leaf_Moves_To_Previous_Sibling) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(1);  // At right child (4.0)

    Cursor_Path_Runtime result = cursor_left(n.get(), path);
    EXPECT_EQ(result.depth(), 1);
    EXPECT_EQ(result[0], 0);  // Moved to left child (3.0)
}

TEST(Ast_Cursor_Left, From_First_Child_Moves_To_Parent) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(0);  // At left child (3.0)

    Cursor_Path_Runtime result = cursor_left(n.get(), path);
    EXPECT_TRUE(result.empty());  // Moved to root
}

TEST(Ast_Cursor_Left, From_Internal_Node_Moves_To_Last_Child) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;  // At root (internal node)

    Cursor_Path_Runtime result = cursor_left(n.get(), path);
    EXPECT_EQ(result.depth(), 1);
    EXPECT_EQ(result[0], 1);  // Moved to last child (right)
}

// ─── cursor_right() ───────────────────────────────────────────────────────────

TEST(Ast_Cursor_Right, From_Empty_Path_Moves_To_First_Child) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;  // At root

    Cursor_Path_Runtime result = cursor_right(n.get(), path);
    EXPECT_EQ(result.depth(), 1);
    EXPECT_EQ(result[0], 0);  // Moved to first child
}

TEST(Ast_Cursor_Right, From_Leaf_Moves_To_Next_Sibling) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(0);  // At left child (3.0)

    Cursor_Path_Runtime result = cursor_right(n.get(), path);
    EXPECT_EQ(result.depth(), 1);
    EXPECT_EQ(result[0], 1);  // Moved to right child (4.0)
}

TEST(Ast_Cursor_Right, From_Last_Child_Stays_Put) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;
    path.push(1);  // At right child (4.0)

    Cursor_Path_Runtime result = cursor_right(n.get(), path);
    EXPECT_EQ(result.depth(), 1);
    EXPECT_EQ(result[0], 1);  // Stayed at same position
}

TEST(Ast_Cursor_Right, From_Internal_Node_Moves_To_First_Child) {
    auto n = std::make_unique<Binary_Op_Node>(
        Binary_Op::ADD,
        std::make_unique<Number_Node>(3.0),
        std::make_unique<Number_Node>(4.0));

    Cursor_Path_Runtime path;  // At root (internal node)

    Cursor_Path_Runtime result = cursor_right(n.get(), path);
    EXPECT_EQ(result.depth(), 1);
    EXPECT_EQ(result[0], 0);  // Moved to first child
}

// ─── Node_Traits ──────────────────────────────────────────────────────────────

TEST(Ast_Node_Traits, Number_Node_Is_Leaf) {
    static_assert(Node_Traits<Number_Node>::IS_LEAF, "Number_Node should be a leaf");
    static_assert(!Node_Traits<Number_Node>::IS_OPERATOR, "Number_Node should not be an operator");
    static_assert(Node_Traits<Number_Node>::CHILD_COUNT == 0, "Number_Node should have 0 children");
}

TEST(Ast_Node_Traits, Binary_Op_Node_Is_Operator) {
    static_assert(!Node_Traits<Binary_Op_Node>::IS_LEAF, "Binary_Op_Node should not be a leaf");
    static_assert(Node_Traits<Binary_Op_Node>::IS_OPERATOR, "Binary_Op_Node should be an operator");
    static_assert(Node_Traits<Binary_Op_Node>::CHILD_COUNT == 2, "Binary_Op_Node should have 2 children");
}

TEST(Ast_Node_Traits, Unary_Op_Node_Is_Operator) {
    static_assert(!Node_Traits<Unary_Op_Node>::IS_LEAF, "Unary_Op_Node should not be a leaf");
    static_assert(Node_Traits<Unary_Op_Node>::IS_OPERATOR, "Unary_Op_Node should be an operator");
    static_assert(Node_Traits<Unary_Op_Node>::CHILD_COUNT == 1, "Unary_Op_Node should have 1 child");
}

TEST(Ast_Node_Traits, Function_Node_Is_Function) {
    static_assert(!Node_Traits<Function_Node>::IS_LEAF, "Function_Node should not be a leaf");
    static_assert(Node_Traits<Function_Node>::IS_FUNCTION, "Function_Node should be a function");
    static_assert(Node_Traits<Function_Node>::CHILD_COUNT == static_cast<size_t>(-1), "Function_Node should have variable children");
}

// ─── Concepts ─────────────────────────────────────────────────────────────────

TEST(Ast_Concepts, Leaf_Node_Concept) {
    static_assert(Leaf_Node<Number_Node>, "Number_Node should satisfy Leaf_Node concept");
    static_assert(!Leaf_Node<Binary_Op_Node>, "Binary_Op_Node should not satisfy Leaf_Node concept");
}

TEST(Ast_Concepts, Operator_Node_Concept) {
    static_assert(Operator_Node<Binary_Op_Node>, "Binary_Op_Node should satisfy Operator_Node concept");
    static_assert(Operator_Node<Unary_Op_Node>, "Unary_Op_Node should satisfy Operator_Node concept");
    static_assert(!Operator_Node<Number_Node>, "Number_Node should not satisfy Operator_Node concept");
}

TEST(Ast_Concepts, Function_Node_Concept) {
    static_assert(Is_Function_Node<Function_Node>, "Function_Node should satisfy Is_Function_Node concept");
    static_assert(!Is_Function_Node<Binary_Op_Node>, "Binary_Op_Node should not satisfy Is_Function_Node concept");
}

TEST(Ast_Concepts, Has_Fixed_Children_Concept) {
    static_assert(Has_Fixed_Children<Binary_Op_Node>, "Binary_Op_Node should have fixed children");
    static_assert(Has_Fixed_Children<Number_Node>, "Number_Node should have fixed children (0)");
    static_assert(!Has_Fixed_Children<Function_Node>, "Function_Node should have variable children");
}

TEST(Ast_Concepts, Has_Variable_Children_Concept) {
    static_assert(Has_Variable_Children<Function_Node>, "Function_Node should have variable children");
    static_assert(!Has_Variable_Children<Binary_Op_Node>, "Binary_Op_Node should not have variable children");
}
