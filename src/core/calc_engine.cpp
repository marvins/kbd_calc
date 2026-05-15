#include "calc_engine.hpp"
#include "parser.hpp"
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <iomanip>

Calc_Engine::Calc_Engine() {
    reset();
}

void Calc_Engine::reset() {
    m_state.expression    = "";
    m_state.display_value = "0";
    m_state.error         = "";
    m_state.cursor_pos    = 0;
    m_result_shown        = false;
}

const Calc_State& Calc_Engine::state() const {
    return m_state;
}

void Calc_Engine::handle_key(Key_Code code) {
    m_state.error = "";

    if (m_result_shown && code != Key_Code::EQUALS) {
        switch (code) {
            case Key_Code::ADD: case Key_Code::SUBTRACT:
            case Key_Code::MULTIPLY: case Key_Code::DIVIDE:
            case Key_Code::POWER_N:
                break;
            default:
                m_state.expression = "";
                break;
        }
        m_result_shown = false;
    }

    switch (code) {
        case Key_Code::DIGIT_0: append_to_expression("0"); break;
        case Key_Code::DIGIT_1: append_to_expression("1"); break;
        case Key_Code::DIGIT_2: append_to_expression("2"); break;
        case Key_Code::DIGIT_3: append_to_expression("3"); break;
        case Key_Code::DIGIT_4: append_to_expression("4"); break;
        case Key_Code::DIGIT_5: append_to_expression("5"); break;
        case Key_Code::DIGIT_6: append_to_expression("6"); break;
        case Key_Code::DIGIT_7: append_to_expression("7"); break;
        case Key_Code::DIGIT_8: append_to_expression("8"); break;
        case Key_Code::DIGIT_9: append_to_expression("9"); break;
        case Key_Code::DECIMAL: {
            if (m_state.expression.empty()) {
                m_state.expression = "0";
                m_state.cursor_pos = 1;
            }
            // Find start of current number token by scanning back to last operator
            const std::string& expr = m_state.expression;
            int token_start = m_state.cursor_pos;
            while (token_start > 0) {
                char ch = expr[static_cast<std::size_t>(token_start - 1)];
                if (ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
                    ch == '(' || ch == ')' || ch == '^' || ch == '%')
                    break;
                --token_start;
            }
            std::string token = expr.substr(static_cast<std::size_t>(token_start),
                                            static_cast<std::size_t>(m_state.cursor_pos - token_start));
            if (token.find('.') == std::string::npos)
                append_to_expression(".");
            break;
        }

        case Key_Code::ADD:         append_to_expression("+"); break;
        case Key_Code::SUBTRACT:    append_to_expression("-"); break;
        case Key_Code::MULTIPLY:    append_to_expression("*"); break;
        case Key_Code::DIVIDE:      append_to_expression("/"); break;

        case Key_Code::PAREN_OPEN:  append_to_expression("("); break;
        case Key_Code::PAREN_CLOSE: append_to_expression(")"); break;
        case Key_Code::PERCENT:     append_to_expression("%"); break;

        case Key_Code::SIN:         append_to_expression("sin(");  break;
        case Key_Code::COS:         append_to_expression("cos(");  break;
        case Key_Code::TAN:         append_to_expression("tan(");  break;
        case Key_Code::ASIN:        append_to_expression("asin("); break;
        case Key_Code::ACOS:        append_to_expression("acos("); break;
        case Key_Code::ATAN:        append_to_expression("atan("); break;
        case Key_Code::LOG:         append_to_expression("log(");  break;
        case Key_Code::LN:          append_to_expression("ln(");   break;
        case Key_Code::EXP:         append_to_expression("exp(");  break;
        case Key_Code::SQRT:        append_to_expression("sqrt("); break;
        case Key_Code::POWER_2:     append_to_expression("^2");    break;
        case Key_Code::POWER_N:     append_to_expression("^");     break;
        case Key_Code::FACTORIAL:   append_to_expression("!");     break;
        case Key_Code::PI:          append_to_expression("pi");    break;
        case Key_Code::EULER:       append_to_expression("e");     break;
        case Key_Code::PHI:         append_to_expression("phi");   break;
        case Key_Code::TAU:         append_to_expression("tau");   break;

        case Key_Code::BIT_AND:     append_to_expression("&");  break;
        case Key_Code::BIT_OR:      append_to_expression("|");  break;
        case Key_Code::BIT_XOR:     append_to_expression("^");  break;
        case Key_Code::SHIFT_LEFT:  append_to_expression("<<"); break;
        case Key_Code::SHIFT_RIGHT: append_to_expression(">>"); break;
        case Key_Code::HEX_A:       append_to_expression("A"); break;
        case Key_Code::HEX_B:       append_to_expression("B"); break;
        case Key_Code::HEX_C:       append_to_expression("C"); break;
        case Key_Code::HEX_D:       append_to_expression("D"); break;
        case Key_Code::HEX_E:       append_to_expression("E"); break;
        case Key_Code::HEX_F:       append_to_expression("F"); break;

        case Key_Code::NEGATE:
            if (!m_state.expression.empty()) {
                if (m_state.expression[0] == '-') {
                    m_state.expression = m_state.expression.substr(1);
                    if (m_state.cursor_pos > 0) --m_state.cursor_pos;
                } else {
                    m_state.expression = "-" + m_state.expression;
                    ++m_state.cursor_pos;
                }
                m_state.display_value = m_state.expression;
            }
            break;

        case Key_Code::CURSOR_LEFT:
            if (m_state.cursor_pos > 0) --m_state.cursor_pos;
            break;

        case Key_Code::CURSOR_RIGHT:
            if (m_state.cursor_pos < static_cast<int>(m_state.expression.size()))
                ++m_state.cursor_pos;
            break;

        case Key_Code::BACKSPACE:
            if (m_state.cursor_pos > 0) {
                m_state.expression.erase(static_cast<std::size_t>(m_state.cursor_pos - 1), 1);
                --m_state.cursor_pos;
                m_state.display_value = m_state.expression.empty() ? "0" : m_state.expression;
            }
            break;

        case Key_Code::CLEAR:
            m_state.expression    = "";
            m_state.display_value = "0";
            m_state.cursor_pos    = 0;
            break;

        case Key_Code::ALL_CLEAR:
            reset();
            break;

        case Key_Code::MEM_STORE:
            try {
                m_state.memory = Parser(m_state.expression).parse()->eval();
            } catch (...) {}
            break;
        case Key_Code::MEM_RECALL:
            append_to_expression(std::to_string(m_state.memory));
            break;
        case Key_Code::MEM_ADD:
            try {
                m_state.memory += Parser(m_state.expression).parse()->eval();
            } catch (...) {}
            break;
        case Key_Code::MEM_CLEAR:
            m_state.memory = 0.0;
            break;

        case Key_Code::EQUALS:
            evaluate();
            break;

        default:
            break;
    }
}

void Calc_Engine::append_to_expression(const std::string& token) {
    m_state.expression.insert(static_cast<std::size_t>(m_state.cursor_pos), token);
    m_state.cursor_pos   += static_cast<int>(token.size());
    m_state.display_value = m_state.expression;
}

void Calc_Engine::evaluate() {
    if (m_state.expression.empty()) return;
    try {
        Parser        p(m_state.expression);
        Ast::Node_Ptr tree   = p.parse();
        double        result = tree->eval();

        std::ostringstream oss;
        if (result == std::floor(result) && std::abs(result) < 1e15)
            oss << std::fixed << std::setprecision(0) << result;
        else
            oss << std::setprecision(10) << result;

        m_state.display_value = oss.str();
        m_state.expression    = m_state.display_value;
        m_state.cursor_pos    = static_cast<int>(m_state.expression.size());
        m_state.last_ast      = std::move(tree);
        m_result_shown        = true;
    } catch (const std::exception& e) {
        m_state.error         = e.what();
        m_state.display_value = "Error";
    }
}

