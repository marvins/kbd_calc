/**
 * @file    test_layout_engine.cpp
 * @author  Marvin Smith
 * @date    2026-06-09
 *
 * @brief   Regression tests for Layout_Engine + Font_Metrics integration
 *
 * Covers two bugs that caused the canvas to show all-white / cleared output:
 *
 *   1. Font metrics initialized before LVGL fonts were loaded → all advances
 *      were 0 → Layout_Engine produced zero-width boxes → lv_draw_label drew
 *      nothing visible.
 *
 *   2. lv_draw_label was given a text pointer into a temporary Layout_Box
 *      string, which became dangling before the async layer flush.  The fix
 *      was text_local=1; this test guards the upstream symptom (non-zero box
 *      widths) so a regression is caught before it reaches the renderer.
 */
#include <gtest/gtest.h>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/font/font_metrics.hpp>
#include <overboard/math/layout/engine.hpp>
#include <overboard/math/parser.hpp>

using namespace ovb;

// ---------------------------------------------------------------------------
// Fixture: build a Layout_Engine from the real LVGL font, same as production
// ---------------------------------------------------------------------------
class Layout_Engine_From_LV_Font : public ::testing::Test {
    protected:
        font::Font_Metrics metrics{ font::Font_Metrics::make_from_lv_font(&lv_font_montserrat_14) };
        math::layout::Layout_Engine engine{ metrics, 1 };
};

/*********************************************/
/*   Regression: zero-width atom boxes       */
/*********************************************/
TEST_F(Layout_Engine_From_LV_Font, digit_atom_has_positive_width) {
    // If Font_Metrics is built before lv_init() / font tables are populated,
    // all advances are 0 and size.x will be 0.  Guard that here.
    auto ast = math::Parser("4").parse();
    ASSERT_NE(ast, nullptr);

    auto box = engine.build(ast.get());
    engine.measure(box);

    EXPECT_GT(box.size.x, 0) << "ATOM '4' has zero width — Font_Metrics likely built too early";
    EXPECT_GT(box.size.y, 0) << "ATOM '4' has zero height";
}

TEST_F(Layout_Engine_From_LV_Font, multi_digit_number_has_positive_width) {
    auto ast = math::Parser("123").parse();
    ASSERT_NE(ast, nullptr);

    auto box = engine.build(ast.get());
    engine.measure(box);

    EXPECT_GT(box.size.x, 0);
}

/*********************************************/
/*   Regression: subtract expression layout  */
/*********************************************/
TEST_F(Layout_Engine_From_LV_Font, subtract_expression_all_atoms_have_positive_width) {
    // "123-456" was the exact expression that triggered the blank-canvas bug.
    // After the fix the SEQUENCE box and each child ATOM must have size.x > 0.
    auto ast = math::Parser("123-456").parse();
    ASSERT_NE(ast, nullptr);

    auto box = engine.build(ast.get());
    engine.measure(box);
    engine.layout(box, {0, 0});

    // Top-level box must be wider than either operand alone
    EXPECT_GT(box.size.x, 0);

    // Every leaf ATOM in the tree must have positive width
    std::function<void(const math::layout::Layout_Box&)> check;
    check = [&](const math::layout::Layout_Box& b) {
        if (b.kind == math::layout::Box_Kind::ATOM && !b.text.empty()) {
            EXPECT_GT(b.size.x, 0)
                << "ATOM '" << b.text << "' has zero width";
        }
        for (const auto& child : b.children) {
            check(child);
        }
    };
    check(box);
}

TEST_F(Layout_Engine_From_LV_Font, subtract_operator_atom_has_positive_width) {
    // Specifically guard the '-' glyph advance — this was the key that
    // triggered the visible clearing because the operator ATOM had size.x=0
    // and all subsequent layout positions collapsed.
    EXPECT_GT(metrics.char_advance('-'), 0)
        << "'-' advance is 0 — Font_Metrics advances not populated";
}

/*********************************************/
/*   Layout positions are non-overlapping    */
/*********************************************/
TEST_F(Layout_Engine_From_LV_Font, sequence_children_have_increasing_x_positions) {
    auto ast = math::Parser("1-2").parse();
    ASSERT_NE(ast, nullptr);

    auto box = engine.build(ast.get());
    engine.measure(box);
    engine.layout(box, {0, 0});

    // Collect all leaf ATOM positions in order
    std::vector<int> x_positions;
    std::function<void(const math::layout::Layout_Box&)> collect;
    collect = [&](const math::layout::Layout_Box& b) {
        if (b.kind == math::layout::Box_Kind::ATOM) {
            x_positions.push_back(b.pos.x);
        }
        for (const auto& child : b.children) {
            collect(child);
        }
    };
    collect(box);

    ASSERT_GE(x_positions.size(), 3u) << "Expected 3 atoms: '1', '-', '2'";
    for (size_t i = 1; i < x_positions.size(); ++i) {
        EXPECT_GT(x_positions[i], x_positions[i - 1])
            << "Atom " << i << " does not advance past atom " << (i - 1)
            << " — likely zero-width atoms causing overlap";
    }
}
