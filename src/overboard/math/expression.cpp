/**
 * @file   expression.cpp
 * @author Marvin Smith
 * @date   5/20/2026
 *
 * @brief  Implementation of the Expression class for handling mathematical expressions.
 */
#include <overboard/math/expression.hpp>

// C++ Standard Libraries
#include <stdexcept>

// ── Token factory ─────────────────────────────────────────────────────────────

Token Expression::make_token(Key_Code code) {
    using KC = Key_Code;
    switch (code) {
        // Operators
        case KC::ADD:         return { Token_Type::Operator, "+",     code };
        case KC::SUBTRACT:    return { Token_Type::Operator, "-",     code };
        case KC::MULTIPLY:    return { Token_Type::Operator, "*",     code };
        case KC::DIVIDE:      return { Token_Type::Operator, "/",     code };
        case KC::POWER_N:     return { Token_Type::Operator, "^",     code };
        case KC::PERCENT:     return { Token_Type::Operator, "%",     code };
        case KC::BIT_AND:     return { Token_Type::Operator, "&",     code };
        case KC::BIT_OR:      return { Token_Type::Operator, "|",     code };
        case KC::BIT_XOR:     return { Token_Type::Operator, "^",     code };
        case KC::SHIFT_LEFT:  return { Token_Type::Operator, "<<",    code };
        case KC::SHIFT_RIGHT: return { Token_Type::Operator, ">>",    code };

        // Parens
        case KC::PAREN_OPEN:  return { Token_Type::Paren,    "(",     code };
        case KC::PAREN_CLOSE: return { Token_Type::Paren,    ")",     code };

        // Functions (include opening paren — deleted atomically as one token)
        case KC::SIN:         return { Token_Type::Function, "sin(",  code };
        case KC::COS:         return { Token_Type::Function, "cos(",  code };
        case KC::TAN:         return { Token_Type::Function, "tan(",  code };
        case KC::ASIN:        return { Token_Type::Function, "asin(", code };
        case KC::ACOS:        return { Token_Type::Function, "acos(", code };
        case KC::ATAN:        return { Token_Type::Function, "atan(", code };
        case KC::LOG:         return { Token_Type::Function, "log(",  code };
        case KC::LN:          return { Token_Type::Function, "ln(",   code };
        case KC::EXP:         return { Token_Type::Function, "exp(",  code };
        case KC::SQRT:        return { Token_Type::Function, "sqrt(", code };
        case KC::POWER_2:     return { Token_Type::Function, "^2",    code };
        case KC::FACTORIAL:   return { Token_Type::Operator, "!",     code };
        case KC::RECIPROCAL:  return { Token_Type::Function, "1/",      code };
        case KC::APPROX:      return { Token_Type::Function, "approx(", code };

        // Constants
        case KC::PI:          return { Token_Type::Constant, "pi",    code };
        case KC::EULER:       return { Token_Type::Constant, "e",     code };
        case KC::PHI:         return { Token_Type::Constant, "phi",   code };
        case KC::TAU:         return { Token_Type::Constant, "tau",   code };

        // Digits / decimal — caller handles these as Number tokens
        case KC::DIGIT_0:     return { Token_Type::Number,   "0",     code };
        case KC::DIGIT_1:     return { Token_Type::Number,   "1",     code };
        case KC::DIGIT_2:     return { Token_Type::Number,   "2",     code };
        case KC::DIGIT_3:     return { Token_Type::Number,   "3",     code };
        case KC::DIGIT_4:     return { Token_Type::Number,   "4",     code };
        case KC::DIGIT_5:     return { Token_Type::Number,   "5",     code };
        case KC::DIGIT_6:     return { Token_Type::Number,   "6",     code };
        case KC::DIGIT_7:     return { Token_Type::Number,   "7",     code };
        case KC::DIGIT_8:     return { Token_Type::Number,   "8",     code };
        case KC::DIGIT_9:     return { Token_Type::Number,   "9",     code };
        case KC::DECIMAL:     return { Token_Type::Number,   ".",     code };
        case KC::HEX_A:       return { Token_Type::Number,   "A",     code };
        case KC::HEX_B:       return { Token_Type::Number,   "B",     code };
        case KC::HEX_C:       return { Token_Type::Number,   "C",     code };
        case KC::HEX_D:       return { Token_Type::Number,   "D",     code };
        case KC::HEX_E:       return { Token_Type::Number,   "E",     code };
        case KC::HEX_F:       return { Token_Type::Number,   "F",     code };

        default:
            throw std::invalid_argument("Expression::make_token: non-insertable key");
    }
}

// ── Internal helpers ──────────────────────────────────────────────────────────

bool Expression::cursor_inside_number() const {
    if (cursor_token_ < 0 || cursor_token_ >= static_cast<int>(tokens_.size()))
        return false;
    const Token& t = tokens_[static_cast<std::size_t>(cursor_token_)];
    return t.type == Token_Type::Number && t.char_cursor >= 0;
}

Token& Expression::current_number_token() {
    return tokens_[static_cast<std::size_t>(cursor_token_)];
}

// Count Unicode codepoints in a UTF-8 string.
int Expression::glyph_count(const std::string& s) {
    int count = 0;
    for (std::size_t i = 0; i < s.size(); ) {
        uint8_t b = static_cast<uint8_t>(s[i]);
        if      (b < 0x80)           i += 1;
        else if ((b & 0xE0) == 0xC0) i += 2;
        else if ((b & 0xF0) == 0xE0) i += 3;
        else                         i += 4;
        ++count;
    }
    return count;
}

// ── insert ────────────────────────────────────────────────────────────────────

void Expression::insert(Key_Code code) {
    Token t = make_token(code);

    if (t.type == Token_Type::Number) {
        // ── Digit / decimal / hex character ──────────────────────────────────
        if (cursor_inside_number()) {
            // Append character into the open number token at char_cursor.
            Token& num = current_number_token();

            // Guard: only one decimal point per number token.
            if (t.text == "." && num.text.find('.') != std::string::npos)
                return;

            num.text.insert(static_cast<std::size_t>(num.char_cursor), t.text);
            num.char_cursor += static_cast<int>(t.text.size());
        } else {
            // Check if token to the left is an open number (cursor at its end).
            // cursor_token_ points to the token we are AFTER, so look there.
            bool merged = false;
            if (cursor_token_ >= 0) {
                Token& prev = tokens_[static_cast<std::size_t>(cursor_token_)];
                if (prev.type == Token_Type::Number) {
                    if (t.text == "." && prev.text.find('.') != std::string::npos)
                        return;
                    prev.text += t.text;
                    prev.char_cursor = static_cast<int>(prev.text.size());
                    merged = true;
                }
            }
            if (!merged) {
                // Insert a fresh number token after cursor_token_.
                // Auto-prepend "0" when starting a number with a decimal point.
                if (t.text == ".")
                    t.text = "0.";
                int insert_pos = cursor_token_ + 1;
                t.char_cursor = static_cast<int>(t.text.size());
                tokens_.insert(tokens_.begin() + insert_pos, t);
                cursor_token_ = insert_pos;
            }
        }
    } else {
        // ── Non-number token ─────────────────────────────────────────────────
        // Close any open number first (move char_cursor out).
        if (cursor_inside_number())
            close_number();

        int insert_pos = cursor_token_ + 1;
        tokens_.insert(tokens_.begin() + insert_pos, t);
        cursor_token_ = insert_pos;
    }
}

// ── backspace ─────────────────────────────────────────────────────────────────

void Expression::backspace() {
    if (cursor_inside_number()) {
        Token& num = current_number_token();
        if (num.char_cursor > 0) {
            num.text.erase(static_cast<std::size_t>(num.char_cursor - 1), 1);
            --num.char_cursor;
        }
        if (num.text.empty()) {
            tokens_.erase(tokens_.begin() + cursor_token_);
            --cursor_token_;
        }
        return;
    }

    // Cursor is between tokens — delete the token to the left.
    if (cursor_token_ < 0)
        return;

    tokens_.erase(tokens_.begin() + cursor_token_);
    --cursor_token_;
}

// ── cursor movement ───────────────────────────────────────────────────────────

void Expression::cursor_left() {
    if (cursor_inside_number()) {
        Token& num = current_number_token();
        if (num.char_cursor > 0) {
            --num.char_cursor;
            return;
        }
        // Exit the number on the left side.
        num.char_cursor = -1;
        --cursor_token_;
        return;
    }

    if (cursor_token_ < 0)
        return;

    // Step into a number to the left if it exists.
    Token& left = tokens_[static_cast<std::size_t>(cursor_token_)];
    if (left.type == Token_Type::Number) {
        left.char_cursor = static_cast<int>(left.text.size()) - 1;
    } else {
        --cursor_token_;
    }
}

void Expression::cursor_right() {
    if (cursor_inside_number()) {
        Token& num = current_number_token();
        if (num.char_cursor < static_cast<int>(num.text.size())) {
            ++num.char_cursor;
            return;
        }
        // Exit the number on the right side.
        num.char_cursor = -1;
        return;
    }

    int next = cursor_token_ + 1;
    if (next >= static_cast<int>(tokens_.size()))
        return;

    Token& right = tokens_[static_cast<std::size_t>(next)];
    if (right.type == Token_Type::Number) {
        right.char_cursor = 1;
        cursor_token_ = next;
    } else {
        cursor_token_ = next;
    }
}

// ── clear ─────────────────────────────────────────────────────────────────────

void Expression::clear() {
    tokens_.clear();
    cursor_token_ = -1;
}

// ── close_number ──────────────────────────────────────────────────────────────

void Expression::close_number() {
    if (cursor_inside_number())
        current_number_token().char_cursor = -1;
}

// ── render_string / eval_string ───────────────────────────────────────────────

std::string Expression::eval_string() const {
    std::string out;
    for (const Token& t : tokens_)
        out += t.text;
    return out;
}

std::string Expression::render_string() const {
    return eval_string();
}

// ── cursor_glyph_pos ─────────────────────────────────────────────────────────

int Expression::cursor_glyph_pos() const {
    int glyphs = 0;
    for (int i = 0; i < static_cast<int>(tokens_.size()); ++i) {
        const Token& t = tokens_[static_cast<std::size_t>(i)];
        if (t.type == Token_Type::Number && t.char_cursor >= 0) {
            // Cursor is inside this number — count glyphs up to char_cursor.
            glyphs += glyph_count(t.text.substr(0, static_cast<std::size_t>(t.char_cursor)));
            return glyphs;
        }
        glyphs += glyph_count(t.text);
        if (i == cursor_token_)
            return glyphs;
    }
    return glyphs;
}
