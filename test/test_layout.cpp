/**
 * @file    test_layout.cpp
 * @author  Marvin Smith
 * @date    2025-10-19
 * @brief   Unit tests for the Layout system.
 */

// C++ Standard Libraries
#include <iostream>
#include <string>

// Third-Party Libraries
#include <gtest/gtest.h>

// Project Libraries
#include <overboard/math/ast/ast.hpp>
#include <overboard/math/layout/box.hpp>
#include <overboard/math/layout/engine.hpp>
#include <overboard/math/parser.hpp>
#include "test_utils.hpp"

using namespace ovb;

/****************************/
/*       Test Helpers       */
/****************************/
static layout::Layout_Box parse_and_layout(const std::string& expr, int scale = 2) {
    math::Parser parser(expr);
    auto tree = parser.parse();
    layout::Layout_Engine engine(scale);
    auto box = engine.build(tree.get());
    engine.measure(box);
    return box;
}

/****************************/
/*       Test Fixture       */
/****************************/
class LayoutTest : public ::testing::Test {
    protected:
        void dump(const layout::Layout_Box& box) const {
            std::cout << "\n";
            test::print_box(box);
            std::cout << "\n";
        }
};

/****************************/
/*     Basic Atom Tests     */
/****************************/

TEST_F(LayoutTest, number_atom) {
    auto box = parse_and_layout("42");
    EXPECT_EQ(box.kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.text, "42");
    EXPECT_EQ(box.scale, 2);
    // "42" = 2 chars * 6px * scale2 = 24px wide, 7*2 = 14px tall
    EXPECT_EQ(box.size.x, 24);
    EXPECT_EQ(box.size.y, 14);
}

TEST_F(LayoutTest, constant_atom) {
    auto box = parse_and_layout("pi");
    EXPECT_EQ(box.kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.text, "pi");
}

/****************************/
/*   Power/Exponent Tests   */
/****************************/
TEST_F(LayoutTest, simple_power) {
    auto box = parse_and_layout("4^3");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::POWER);
    ASSERT_EQ(box.children.size(), 2u);

    // Base
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[0].text, "4");
    EXPECT_EQ(box.children[0].scale, 2);  // Base keeps original scale

    // Exponent
    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[1].text, "3");
    EXPECT_EQ(box.children[1].scale, 1);  // Exponent is smaller (scale-1)
}

TEST_F(LayoutTest, nested_power) {
    auto box = parse_and_layout("2^3^2");
    dump(box);

    // Right-associative: 2^(3^2)
    EXPECT_EQ(box.kind, layout::Box_Kind::POWER);
    ASSERT_EQ(box.children.size(), 2u);

    // Base is "2"
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[0].text, "2");

    // Exponent is another POWER (3^2)
    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::POWER);
}

/****************************/
/*      Fraction Tests      */
/****************************/
TEST_F(LayoutTest, simple_fraction) {
    auto box = parse_and_layout("1/2");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::FRACTION);
    ASSERT_EQ(box.children.size(), 2u);

    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[0].text, "1");

    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[1].text, "2");
}

TEST_F(LayoutTest, fraction_with_power_numerator) {
    auto box = parse_and_layout("4^3/3");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::FRACTION);
    ASSERT_EQ(box.children.size(), 2u);

    // Numerator should be a POWER
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::POWER);
    EXPECT_EQ(box.children[0].children[0].text, "4");
    EXPECT_EQ(box.children[0].children[1].text, "3");

    // Denominator is simple
    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[1].text, "3");
}

TEST_F(LayoutTest, fraction_with_power_denominator) {
    auto box = parse_and_layout("3/4^3");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::FRACTION);

    // Numerator is simple
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[0].text, "3");

    // Denominator should be a POWER
    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::POWER);
    EXPECT_EQ(box.children[1].children[0].text, "4");
    EXPECT_EQ(box.children[1].children[1].text, "3");
}

/****************************/
/*      Function Tests      */
/****************************/
TEST_F(LayoutTest, simple_function) {
    auto box = parse_and_layout("sin(4)");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::SEQUENCE);
    ASSERT_GE(box.children.size(), 3u);

    // sin ( 4 )
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[0].text, "sin");
}

TEST_F(LayoutTest, function_with_power) {
    auto box = parse_and_layout("sin(4^3)");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::SEQUENCE);

    // Find the argument (should be a POWER inside parentheses)
    bool found_power_arg = false;
    for (const auto& child : box.children) {
        if (child.kind == layout::Box_Kind::POWER) {
            found_power_arg = true;
            EXPECT_EQ(child.children[0].text, "4");
            EXPECT_EQ(child.children[1].text, "3");
            // Exponent should be smaller scale
            EXPECT_EQ(child.children[1].scale, 1);
        }
    }
    EXPECT_TRUE(found_power_arg) << "Expected to find POWER as function argument";
}

TEST_F(LayoutTest, complex_fraction_with_function_and_power) {
    auto box = parse_and_layout("sin(4^3)/3");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::FRACTION);
    ASSERT_EQ(box.children.size(), 2u);

    // Numerator: sin(4^3) - should be SEQUENCE containing the function
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::SEQUENCE);

    // Look for the power inside the numerator
    bool found_power_in_num = false;
    for (const auto& child : box.children[0].children) {
        if (child.kind == layout::Box_Kind::POWER) {
            found_power_in_num = true;
            EXPECT_EQ(child.children[0].text, "4");
            EXPECT_EQ(child.children[1].text, "3");
        }
    }
    EXPECT_TRUE(found_power_in_num) << "Expected POWER inside numerator";

    // Denominator: simple 3
    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[1].text, "3");
}

/****************************/
/*   Mixed Expression Tests */
/****************************/
TEST_F(LayoutTest, power_of_fraction) {
    auto box = parse_and_layout("(1/2)^3");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::POWER);
    ASSERT_EQ(box.children.size(), 2u);

    // Base is a fraction
    EXPECT_EQ(box.children[0].kind, layout::Box_Kind::FRACTION);

    // Exponent is 3
    EXPECT_EQ(box.children[1].kind, layout::Box_Kind::ATOM);
    EXPECT_EQ(box.children[1].text, "3");
}

TEST_F(LayoutTest, multiplication_sequence) {
    auto box = parse_and_layout("2*3");
    dump(box);

    EXPECT_EQ(box.kind, layout::Box_Kind::SEQUENCE);
    ASSERT_EQ(box.children.size(), 3u);  // 2 * 3

    EXPECT_EQ(box.children[0].text, "2");
    EXPECT_EQ(box.children[1].text, "*");
    EXPECT_EQ(box.children[2].text, "3");
}

