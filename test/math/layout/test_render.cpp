#include <gtest/gtest.h>
#include <cstdint>
#include <string>

// ─── Replicate the UTF-8 decoder from sdl_display.cpp ────────────────────────
// Rather than pulling in SDL headers, we copy the pure-logic decoder here.
// If the implementation in sdl_display.cpp changes, update this copy too.

static uint32_t utf8_decode(const std::string& str, size_t& pos) {
    uint8_t b0 = static_cast<uint8_t>(str[pos]);
    if (b0 < 0x80) { ++pos; return b0; }
    if ((b0 & 0xE0) == 0xC0) {
        if (pos + 1 >= str.size()) { ++pos; return 0xFFFD; }
        uint32_t cp = (static_cast<uint32_t>(b0 & 0x1F) << 6) | static_cast<uint32_t>(static_cast<uint8_t>(str[pos+1]) & 0x3F);
        pos += 2; return cp;
    }
    if ((b0 & 0xF0) == 0xE0) {
        if (pos + 2 >= str.size()) { ++pos; return 0xFFFD; }
        uint32_t cp = (static_cast<uint32_t>(b0 & 0x0F) << 12)
                    | (static_cast<uint32_t>(static_cast<uint8_t>(str[pos+1]) & 0x3F) << 6)
                    |  static_cast<uint32_t>(static_cast<uint8_t>(str[pos+2]) & 0x3F);
        pos += 3; return cp;
    }
    ++pos; return 0xFFFD;
}

// Decode the entire string into a list of codepoints
static std::vector<uint32_t> decode_all(const std::string& s) {
    std::vector<uint32_t> cps;
    size_t pos = 0;
    while (pos < s.size())
        cps.push_back(utf8_decode(s, pos));
    return cps;
}

// ─── Known codepoints for our glyph set ──────────────────────────────────────

struct Unicode_Glyph {
    uint32_t codepoint;
    uint8_t  rows[7];
};

static const Unicode_Glyph UNICODE_GLYPHS[] = {
    { 0x25C0, {0x01,0x03,0x07,0x0F,0x07,0x03,0x01} }, // ◀
    { 0x25B6, {0x10,0x18,0x1C,0x1E,0x1C,0x18,0x10} }, // ▶
    { 0x00F7, {0x00,0x04,0x00,0x1F,0x00,0x04,0x00} }, // ÷
    { 0x00D7, {0x00,0x11,0x0A,0x04,0x0A,0x11,0x00} }, // ×
    { 0x221A, {0x01,0x02,0x02,0x14,0x08,0x08,0x10} }, // √
    { 0x03C0, {0x00,0x1F,0x0A,0x0A,0x0A,0x13,0x00} }, // π
    { 0x00B2, {0x06,0x09,0x02,0x04,0x0F,0x00,0x00} }, // ²
    { 0x02E3, {0x09,0x06,0x04,0x06,0x09,0x00,0x00} }, // ˣ
    { 0x02B8, {0x09,0x09,0x07,0x01,0x0E,0x00,0x00} }, // ʸ
};
static const int GLYPH_COUNT = static_cast<int>(sizeof(UNICODE_GLYPHS)/sizeof(UNICODE_GLYPHS[0]));

static const uint8_t* find_glyph(uint32_t cp) {
    for (int i = 0; i < GLYPH_COUNT; ++i)
        if (UNICODE_GLYPHS[i].codepoint == cp)
            return UNICODE_GLYPHS[i].rows;
    return nullptr;
}

// ─── UTF-8 decoder tests ─────────────────────────────────────────────────────

TEST(Render_Utf8, ASCII_Single_Byte) {
    size_t pos = 0;
    EXPECT_EQ(utf8_decode("A", pos), 0x41u);
    EXPECT_EQ(pos, 1u);
}

TEST(Render_Utf8, Two_Byte_Division) {
    // ÷ = U+00F7 = 0xC3 0xB7
    std::string s = "\xC3\xB7";
    size_t pos = 0;
    EXPECT_EQ(utf8_decode(s, pos), 0x00F7u);
    EXPECT_EQ(pos, 2u);
}

TEST(Render_Utf8, Two_Byte_Multiply) {
    // × = U+00D7 = 0xC3 0x97
    std::string s = "\xC3\x97";
    size_t pos = 0;
    EXPECT_EQ(utf8_decode(s, pos), 0x00D7u);
    EXPECT_EQ(pos, 2u);
}

TEST(Render_Utf8, Two_Byte_Pi) {
    // π = U+03C0 = 0xCF 0x80
    std::string s = "\xCF\x80";
    size_t pos = 0;
    EXPECT_EQ(utf8_decode(s, pos), 0x03C0u);
    EXPECT_EQ(pos, 2u);
}

TEST(Render_Utf8, Three_Byte_Left_Triangle) {
    // ◀ = U+25C0 = 0xE2 0x97 0x80
    std::string s = "\xE2\x97\x80";
    size_t pos = 0;
    EXPECT_EQ(utf8_decode(s, pos), 0x25C0u);
    EXPECT_EQ(pos, 3u);
}

TEST(Render_Utf8, Three_Byte_Right_Triangle) {
    // ▶ = U+25B6 = 0xE2 0x96 0xB6
    std::string s = "\xE2\x96\xB6";
    size_t pos = 0;
    EXPECT_EQ(utf8_decode(s, pos), 0x25B6u);
    EXPECT_EQ(pos, 3u);
}

TEST(Render_Utf8, Three_Byte_Sqrt) {
    // √ = U+221A = 0xE2 0x88 0x9A
    std::string s = "\xE2\x88\x9A";
    size_t pos = 0;
    EXPECT_EQ(utf8_decode(s, pos), 0x221Au);
    EXPECT_EQ(pos, 3u);
}

TEST(Render_Utf8, Mixed_String_Codepoints) {
    // "÷3" — two-byte then ASCII
    std::string s = "\xC3\xB7""3";
    auto cps = decode_all(s);
    ASSERT_EQ(cps.size(), 2u);
    EXPECT_EQ(cps[0], 0x00F7u);
    EXPECT_EQ(cps[1], 0x0033u); // '3'
}

TEST(Render_Utf8, All_Glyph_Labels_Decode) {
    // Verify every label string used in keymap decodes without 0xFFFD
    const std::vector<std::string> labels = {
        "\xE2\x97\x80",     // ◀
        "\xE2\x96\xB6",     // ▶
        "\xC3\xB7",         // ÷
        "\xC3\x97",         // ×
        "\xE2\x88\x9A",     // √
        "\xCF\x80",         // π
        "\xC2\xB2",         // ²
        "\xCB\xA3",         // ˣ
        "\xCA\xB8",         // ʸ
    };
    for (const auto& label : labels) {
        auto cps = decode_all(label);
        for (uint32_t cp : cps)
            EXPECT_NE(cp, 0xFFFDu) << "Replacement char in label: " << label;
    }
}

// ─── Glyph table lookup tests ─────────────────────────────────────────────────

TEST(Render_Glyph, Left_Triangle_Found) {
    EXPECT_NE(find_glyph(0x25C0), nullptr);
}

TEST(Render_Glyph, Right_Triangle_Found) {
    EXPECT_NE(find_glyph(0x25B6), nullptr);
}

TEST(Render_Glyph, Division_Found) {
    EXPECT_NE(find_glyph(0x00F7), nullptr);
}

TEST(Render_Glyph, Multiply_Found) {
    EXPECT_NE(find_glyph(0x00D7), nullptr);
}

TEST(Render_Glyph, Sqrt_Found) {
    EXPECT_NE(find_glyph(0x221A), nullptr);
}

TEST(Render_Glyph, Pi_Found) {
    EXPECT_NE(find_glyph(0x03C0), nullptr);
}

TEST(Render_Glyph, Superscript_2_Found) {
    EXPECT_NE(find_glyph(0x00B2), nullptr);
}

TEST(Render_Glyph, Superscript_X_Found) {
    EXPECT_NE(find_glyph(0x02E3), nullptr);
}

TEST(Render_Glyph, Superscript_Y_Found) {
    EXPECT_NE(find_glyph(0x02B8), nullptr);
}

TEST(Render_Glyph, Unknown_Returns_Null) {
    EXPECT_EQ(find_glyph(0x1234), nullptr);
}

// ─── Glyph bitmap sanity checks ──────────────────────────────────────────────

TEST(Render_Glyph, Division_Middle_Row_Full) {
    // ÷ should have a full-width horizontal bar on row 3 (0x1F = 0b11111)
    const uint8_t* g = find_glyph(0x00F7);
    ASSERT_NE(g, nullptr);
    EXPECT_EQ(g[3], 0x1F);
}

TEST(Render_Glyph, Division_Dots_Symmetric) {
    // Dot rows (1 and 5) should both be 0x04 (centre pixel)
    const uint8_t* g = find_glyph(0x00F7);
    ASSERT_NE(g, nullptr);
    EXPECT_EQ(g[1], 0x04);
    EXPECT_EQ(g[5], 0x04);
}

TEST(Render_Glyph, Left_Triangle_Grows_Then_Shrinks) {
    // ◀: rows should peak at the middle (row 3 widest)
    const uint8_t* g = find_glyph(0x25C0);
    ASSERT_NE(g, nullptr);
    // Row 3 (middle) should be widest (0x0F = 4 bits set)
    EXPECT_EQ(g[3], 0x0F);
    // Row 0 should be narrowest (single pixel)
    EXPECT_EQ(g[0], 0x01);
}

TEST(Render_Glyph, Right_Triangle_Mirror_Of_Left) {
    // ▶ should be the horizontal mirror of ◀
    const uint8_t* l = find_glyph(0x25C0);
    const uint8_t* r = find_glyph(0x25B6);
    ASSERT_NE(l, nullptr);
    ASSERT_NE(r, nullptr);
    for (int row = 0; row < 7; ++row) {
        // Mirror a 5-bit value
        uint8_t lv = l[row];
        uint8_t rv = r[row];
        uint8_t mirrored = 0;
        for (int b = 0; b < 5; ++b)
            if (lv & (1 << b)) mirrored |= (1 << (4 - b));
        EXPECT_EQ(mirrored, rv) << "Row " << row << " not mirrored";
    }
}

TEST(Render_Glyph, Pi_Has_Top_Bar) {
    // π top bar should span full width: row 1 = 0x1F
    const uint8_t* g = find_glyph(0x03C0);
    ASSERT_NE(g, nullptr);
    EXPECT_EQ(g[1], 0x1F);
}

TEST(Render_Glyph, Superscript_Bottom_Rows_Blank) {
    // ², ˣ, ʸ should have blank bottom rows (top-aligned)
    for (uint32_t cp : {0x00B2u, 0x02E3u, 0x02B8u}) {
        const uint8_t* g = find_glyph(cp);
        ASSERT_NE(g, nullptr) << "Glyph not found: " << cp;
        EXPECT_EQ(g[5], 0x00) << "Row 5 not blank for cp " << cp;
        EXPECT_EQ(g[6], 0x00) << "Row 6 not blank for cp " << cp;
    }
}

TEST(Render_Glyph, All_Rows_Within_5_Bits) {
    // No glyph row should use bits beyond the 5-pixel width (mask 0x1F)
    for (int i = 0; i < GLYPH_COUNT; ++i) {
        for (int row = 0; row < 7; ++row) {
            EXPECT_EQ(UNICODE_GLYPHS[i].rows[row] & ~0x1F, 0)
                << "Glyph cp=0x" << std::hex << UNICODE_GLYPHS[i].codepoint
                << " row " << row << " exceeds 5 bits";
        }
    }
}
