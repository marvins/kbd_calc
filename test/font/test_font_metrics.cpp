/**
 * @file    test_font_metrics.cpp
 * @author  Marvin Smith
 * @date    2026-06-09
 *
 * @brief   Unit tests for Font_Metrics::make_from_lv_font()
 *
 * Goal: verify that make_from_lv_font() produces metrics that exactly
 * match what LVGL's font descriptors report, so the layout engine and
 * renderer stay in sync without manual tuning.
 */
#include <gtest/gtest.h>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/font/font_metrics.hpp>

namespace {

class Font_Metrics_From_LV_Font : public ::testing::Test {
    protected:
        ovb::font::Font_Metrics m{ ovb::font::Font_Metrics::make_from_lv_font(&lv_font_montserrat_14) };
};

/************************************************/
/*        Test Line Height Matches Font         */
/************************************************/
TEST_F(Font_Metrics_From_LV_Font, line_height_matches_font) {
    EXPECT_EQ(m.line_height(), static_cast<int>(lv_font_montserrat_14.line_height));
}

/**************************************************************/
/*        Test Ascent Plus Descent Equals Line Height         */
/**************************************************************/
TEST_F(Font_Metrics_From_LV_Font, ascent_plus_descent_equals_line_height) {
    EXPECT_EQ(m.ascent + m.descent, m.line_height());
}

/**************************************************************/
/*            Test Descent Matches Font Base Line             */
/**************************************************************/
TEST_F(Font_Metrics_From_LV_Font, descent_matches_font_base_line) {
    EXPECT_EQ(m.descent, static_cast<int>(lv_font_montserrat_14.base_line));
}

/**************************************************************/
/*            Test Digit Advance Matches LVGL Query           */
/**************************************************************/
TEST_F(Font_Metrics_From_LV_Font, digit_advance_matches_lvgl_query) {
    // For each digit, make_from_lv_font should store exactly what
    // lv_font_get_glyph_dsc reports as adv_w
    lv_font_glyph_dsc_t dsc{};
    for (char c = '0'; c <= '9'; ++c) {
        ASSERT_TRUE(lv_font_get_glyph_dsc(&lv_font_montserrat_14, &dsc,
                                          static_cast<uint32_t>(c), 0))
            << "No glyph for '" << c << "'";
        EXPECT_EQ(m.char_advance(c), static_cast<int>(dsc.adv_w))
            << "Advance mismatch for '" << c << "'";
    }
}

/**************************************************************/
/*            Test Operator Advance Matches LVGL Query        */
/**************************************************************/
TEST_F(Font_Metrics_From_LV_Font, operator_advances_match_lvgl_query) {
    lv_font_glyph_dsc_t dsc{};
    for (char c : {'+', '-', '*', '/', '(', ')', '.', '%', '^'}) {
        ASSERT_TRUE(lv_font_get_glyph_dsc(&lv_font_montserrat_14, &dsc,
                                          static_cast<uint32_t>(c), 0))
            << "No glyph for '" << c << "'";
        EXPECT_EQ(m.char_advance(c), static_cast<int>(dsc.adv_w))
            << "Advance mismatch for '" << c << "'";
    }
}

/**************************************************************/
/*     Regression: adv_w must be stored as whole pixels       */
/**************************************************************/
TEST_F(Font_Metrics_From_LV_Font, adv_w_is_converted_from_sixteenth_pixels) {
    // In LVGL 9.x, adv_w in lv_font_glyph_dsc_t is already in whole pixels.
    // make_from_lv_font must store it directly without dividing.
    lv_font_glyph_dsc_t dsc{};
    ASSERT_TRUE(lv_font_get_glyph_dsc(&lv_font_montserrat_14, &dsc, '0', 0));

    // In LVGL 9.x adv_w is already in whole pixels (uint16_t)
    const int pixel_adv_w = static_cast<int>(dsc.adv_w);
    EXPECT_EQ(m.char_advance('0'), pixel_adv_w)
        << "adv_w=" << pixel_adv_w << " — make_from_lv_font must store it directly";
    EXPECT_GT(m.char_advance('0'), 0)
        << "advance is zero — font glyph query likely failed";
    EXPECT_LT(m.char_advance('0'), lv_font_montserrat_14.line_height * 3)
        << "advance suspiciously large";
}

/**************************************************************/
/*            Test String Width Sums Individual Advances      */
/**************************************************************/
TEST_F(Font_Metrics_From_LV_Font, string_width_sums_individual_advances) {
    // string_width("3-4") should equal advance('3') + advance('-') + advance('4')
    const int expected = m.char_advance('3') + m.char_advance('-') + m.char_advance('4');
    EXPECT_EQ(m.string_width("3-4"), expected);
}

} // namespace
