/**
 * @file    test_matrix_ops.cpp
 * @author  Marvin Smith
 * @date    2026-07-05
 * @brief   Unit tests for matrix/vector expression insertion via Calc_Engine.
 */

// C++ Standard Libraries
#include <string>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/calc_engine.hpp>

using namespace ovb;

/************************************************************/
/*                insert_matrix Tests                       */
/************************************************************/
TEST(Matrix_Ops, Insert_Matrix_2x2_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_matrix(2, 2);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(2,2)");
}

TEST(Matrix_Ops, Insert_Matrix_3x4_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_matrix(3, 4);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(3,4)");
}

TEST(Matrix_Ops, Insert_Matrix_1x1_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_matrix(1, 1);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(1,1)");
}

TEST(Matrix_Ops, Insert_Matrix_9x9_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_matrix(9, 9);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(9,9)");
}

/************************************************************/
/*                insert_vector Tests                       */
/************************************************************/
TEST(Matrix_Ops, Insert_Vector_3_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_vector(3);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(3)");
}

TEST(Matrix_Ops, Insert_Vector_1_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_vector(1);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(1)");
}

TEST(Matrix_Ops, Insert_Vector_9_Produces_Zeros_Call) {
    math::Calc_Engine eng;
    eng.insert_vector(9);
    EXPECT_EQ(eng.state().expression.eval_string(), "zeros(9)");
}

/************************************************************/
/*       Display string matches eval string shape           */
/************************************************************/
TEST(Matrix_Ops, Insert_Matrix_Display_Value_Updated) {
    math::Calc_Engine eng;
    eng.insert_matrix(2, 3);
    EXPECT_FALSE(eng.state().display_value.empty());
    EXPECT_NE(eng.state().display_value, "0");
}

TEST(Matrix_Ops, Insert_Vector_Display_Value_Updated) {
    math::Calc_Engine eng;
    eng.insert_vector(4);
    EXPECT_FALSE(eng.state().display_value.empty());
    EXPECT_NE(eng.state().display_value, "0");
}

/************************************************************/
/*        Expression starts empty, matrix replaces it       */
/************************************************************/
TEST(Matrix_Ops, Fresh_Engine_Expression_Is_Empty) {
    math::Calc_Engine eng;
    EXPECT_TRUE(eng.state().expression.empty());
}

TEST(Matrix_Ops, Insert_Matrix_Into_Empty_Expression_Fills_It) {
    math::Calc_Engine eng;
    ASSERT_TRUE(eng.state().expression.empty());
    eng.insert_matrix(2, 2);
    EXPECT_FALSE(eng.state().expression.empty());
}

TEST(Matrix_Ops, Insert_Vector_Into_Empty_Expression_Fills_It) {
    math::Calc_Engine eng;
    ASSERT_TRUE(eng.state().expression.empty());
    eng.insert_vector(5);
    EXPECT_FALSE(eng.state().expression.empty());
}

/************************************************************/
/*        Clear resets expression after matrix insert       */
/************************************************************/
TEST(Matrix_Ops, Clear_After_Matrix_Insert_Resets_To_Empty) {
    math::Calc_Engine eng;
    eng.insert_matrix(3, 3);
    ASSERT_FALSE(eng.state().expression.empty());
    eng.handle_key(core::Action_Code::CLEAR);
    EXPECT_TRUE(eng.state().expression.empty());
    EXPECT_EQ(eng.state().display_value, "0");
}
