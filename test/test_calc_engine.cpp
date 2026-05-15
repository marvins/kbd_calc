#include <gtest/gtest.h>
#include "core/calc_engine.hpp"

static void press(Calc_Engine& eng, std::initializer_list<Key_Code> keys) {
    for (Key_Code k : keys)
        eng.handle_key(k);
}

// ─── Decimal guard ───────────────────────────────────────────────────────────

TEST(Calc_Engine_Decimal, Single_Decimal_Allowed) {
    Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3 });
    EXPECT_EQ(eng.state().expression, "3.3");
}

TEST(Calc_Engine_Decimal, Second_Decimal_Ignored) {
    Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3, Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression, "3.3");
}

TEST(Calc_Engine_Decimal, Third_Decimal_Also_Ignored) {
    Calc_Engine eng;
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3,
                 Key_Code::DECIMAL, Key_Code::DIGIT_3, Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression, "3.33");
}

TEST(Calc_Engine_Decimal, New_Token_After_Operator_Allows_Decimal) {
    Calc_Engine eng;
    // 3.3 + 1.1 — each token may have its own decimal
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3,
                 Key_Code::ADD,
                 Key_Code::DIGIT_1, Key_Code::DECIMAL, Key_Code::DIGIT_1 });
    EXPECT_EQ(eng.state().expression, "3.3+1.1");
}

TEST(Calc_Engine_Decimal, Second_Decimal_After_Operator_Ignored) {
    Calc_Engine eng;
    // 3.3 + 1.1. — second dot in second token should be blocked
    press(eng, { Key_Code::DIGIT_3, Key_Code::DECIMAL, Key_Code::DIGIT_3,
                 Key_Code::ADD,
                 Key_Code::DIGIT_1, Key_Code::DECIMAL, Key_Code::DIGIT_1,
                 Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression, "3.3+1.1");
}

TEST(Calc_Engine_Decimal, Leading_Decimal_Prepends_Zero) {
    Calc_Engine eng;
    press(eng, { Key_Code::DECIMAL });
    EXPECT_EQ(eng.state().expression, "0.");
}
