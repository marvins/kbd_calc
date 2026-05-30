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

namespace ovb::math {

// ── Token factory ─────────────────────────────────────────────────────────────

Token Expression::make_token(core::Key_Code code) {
    using KC = core::Key_Code;
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

void Expression::insert(core::Key_Code code) {
    Token t = make_token(code);

    if (t.type == Token_Type::Number) {
        // ── Digit / decimal / hex character ──────────────────────────────────

        // Check if cursor is at a placeholder — replace it with the number
        int next_idx = cursor_token_ + 1;
        if (next_idx < static_cast<int>(tokens_.size()) &&
            tokens_[static_cast<std::size_t>(next_idx)].type == Token_Type::Placeholder) {
            // Replace placeholder with number token
            if (t.text == ".")
                t.text = "0.";
            t.char_cursor = static_cast<int>(t.text.size());
            tokens_[static_cast<std::size_t>(next_idx)] = t;
            cursor_token_ = next_idx;
            return;
        }

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
    } else if ( code == core::Key_Code::PAREN_OPEN ) {
        // ── Auto-balanced parentheses ────────────────────────────────────────
        // Insert "()" with cursor between them
        if (cursor_inside_number())
            close_number();

        Token open_paren = make_token( core::Key_Code::PAREN_OPEN) ;
        Token close_paren = make_token(core::Key_Code::PAREN_CLOSE );

        int insert_pos = cursor_token_ + 1;
        tokens_.insert(tokens_.begin() + insert_pos, open_paren);
        tokens_.insert(tokens_.begin() + insert_pos + 1, close_paren);
        cursor_token_ = insert_pos;  // Cursor is between ( and )
    } else {
        // ── Non-number token (Operator, Function, Constant, Paren) ─────────
        // Close any open number first (move char_cursor out).
        if (cursor_inside_number())
            close_number();

        // Check if cursor is at a placeholder — replace it (unless this is also an operator)
        int next_idx = cursor_token_ + 1;
        if (t.type != Token_Type::Operator &&
            next_idx < static_cast<int>(tokens_.size()) &&
            tokens_[static_cast<std::size_t>(next_idx)].type == Token_Type::Placeholder) {
            // Replace placeholder with this token
            tokens_[static_cast<std::size_t>(next_idx)] = t;
            cursor_token_ = next_idx;
        } else {
            int insert_pos = cursor_token_ + 1;
            tokens_.insert(tokens_.begin() + insert_pos, t);
            cursor_token_ = insert_pos;
        }

        // ── Auto-insert placeholder after binary operators that need a right-hand operand ─
        if (t.type == Token_Type::Operator) {
            // Check if we need a placeholder (at end, or before operator/close paren)
            bool needs_placeholder = false;
            int ph_idx = cursor_token_ + 1;
            if (ph_idx >= static_cast<int>(tokens_.size())) {
                // At end of expression
                needs_placeholder = true;
            } else {
                Token& next = tokens_[static_cast<std::size_t>(ph_idx)];
                if (next.type == Token_Type::Operator ||
                    (next.type == Token_Type::Paren && next.text == ")") ||
                    next.type == Token_Type::Constant) {
                    needs_placeholder = true;
                }
            }

            if (needs_placeholder) {
                Token placeholder{ Token_Type::Placeholder, "", core::Key_Code::NONE, -1 };
                tokens_.insert(tokens_.begin() + ph_idx, placeholder);
            }
        }
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

    // Check for balanced () pair — delete both atomically
    if (cursor_token_ + 1 < static_cast<int>(tokens_.size())) {
        const Token& left = tokens_[static_cast<std::size_t>(cursor_token_)];
        const Token& right = tokens_[static_cast<std::size_t>(cursor_token_ + 1)];
        if (left.type == Token_Type::Paren && left.text == "(" &&
            right.type == Token_Type::Paren && right.text == ")") {
            // Atomic delete of () pair
            tokens_.erase(tokens_.begin() + cursor_token_,
                          tokens_.begin() + cursor_token_ + 2);
            --cursor_token_;
            return;
        }
    }

    // Check for operator followed by placeholder — delete both atomically
    if (cursor_token_ + 1 < static_cast<int>(tokens_.size())) {
        const Token& left = tokens_[static_cast<std::size_t>(cursor_token_)];
        const Token& right = tokens_[static_cast<std::size_t>(cursor_token_ + 1)];
        if ((left.type == Token_Type::Operator ||
             left.type == Token_Type::Function ||
             left.type == Token_Type::Constant) &&
            right.type == Token_Type::Placeholder) {
            // Atomic delete of operator+placeholder pair
            tokens_.erase(tokens_.begin() + cursor_token_,
                          tokens_.begin() + cursor_token_ + 2);
            --cursor_token_;
            return;
        }
    }

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
        // Skip over any placeholders to the left.
        while (cursor_token_ >= 0 &&
               tokens_[static_cast<std::size_t>(cursor_token_)].type == Token_Type::Placeholder) {
            --cursor_token_;
        }
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
        // Skip over any placeholders to the left.
        while (cursor_token_ >= 0 &&
               tokens_[static_cast<std::size_t>(cursor_token_)].type == Token_Type::Placeholder) {
            --cursor_token_;
        }
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
        // Fall through to move to next token (skipping placeholders)
    }

    int next = cursor_token_ + 1;
    // Skip over any placeholders.
    while (next < static_cast<int>(tokens_.size()) &&
           tokens_[static_cast<std::size_t>(next)].type == Token_Type::Placeholder) {
        ++next;
    }
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

// ── placeholder helpers ───────────────────────────────────────────────────────

std::string Expression::placeholder_text_for_eval() {
    return "0";  // Makes expression parsable: "8+0" is valid
}

std::string Expression::placeholder_text_for_render() {
    return "";   // Invisible in display
}

// ── has_placeholder ────────────────────────────────────────────────────────────

bool Expression::has_placeholder() const {
    for (const Token& t : tokens_) {
        if (t.type == Token_Type::Placeholder)
            return true;
    }
    return false;
}

// ── render_string / eval_string ───────────────────────────────────────────────

std::string Expression::eval_string() const {
    std::string out;
    for (const Token& t : tokens_) {
        if (t.type == Token_Type::Placeholder)
            out += placeholder_text_for_eval();
        else
            out += t.text;
    }
    return out;
}

std::string Expression::render_string() const {
    std::string out;
    for (const Token& t : tokens_) {
        if (t.type == Token_Type::Placeholder)
            out += placeholder_text_for_render();
        else
            out += t.text;
    }
    return out;
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

} // namespace ovb::math
