/**
 * @file   calc_engine.cpp
 * @author Marvin Smith
 * @date   5/20/2026
 *
 * @brief  Implementation of the Calc_Engine class for handling calculator operations.
 */
#include <overboard/math/calc_engine.hpp>

// C++ Standard Libraries
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// Project Libraries
#include <overboard/math/parser.hpp>

namespace ovb::math {

/****************************/
/*     Constructor/Reset    */
/****************************/
Calc_Engine::Calc_Engine() {
    reset();
}

void Calc_Engine::reset() {
    m_state.expression.clear();
    m_state.display_value = "0";
    m_state.error         = "";
    m_result_shown        = false;
}

const Calc_State& Calc_Engine::state() const {
    return m_state;
}

/****************************/
/*       Key Handling       */
/****************************/
void Calc_Engine::handle_key(Key_Code code) {
    m_state.error = "";

    if (m_result_shown && code != Key_Code::EQUALS) {
        switch (code) {
            case Key_Code::ADD: case Key_Code::SUBTRACT:
            case Key_Code::MULTIPLY: case Key_Code::DIVIDE:
            case Key_Code::POWER_N:
                break;
            default:
                m_state.expression.clear();
                break;
        }
        m_result_shown = false;
    }

    switch (code) {
        case Key_Code::DIGIT_0: case Key_Code::DIGIT_1: case Key_Code::DIGIT_2:
        case Key_Code::DIGIT_3: case Key_Code::DIGIT_4: case Key_Code::DIGIT_5:
        case Key_Code::DIGIT_6: case Key_Code::DIGIT_7: case Key_Code::DIGIT_8:
        case Key_Code::DIGIT_9: case Key_Code::DECIMAL:
        case Key_Code::HEX_A:   case Key_Code::HEX_B:   case Key_Code::HEX_C:
        case Key_Code::HEX_D:   case Key_Code::HEX_E:   case Key_Code::HEX_F:
        case Key_Code::ADD:     case Key_Code::SUBTRACT: case Key_Code::MULTIPLY:
        case Key_Code::DIVIDE:  case Key_Code::POWER_N:  case Key_Code::POWER_2:
        case Key_Code::PERCENT: case Key_Code::FACTORIAL: case Key_Code::RECIPROCAL:
        case Key_Code::PAREN_OPEN: case Key_Code::PAREN_CLOSE:
        case Key_Code::SIN:  case Key_Code::COS:  case Key_Code::TAN:
        case Key_Code::ASIN: case Key_Code::ACOS: case Key_Code::ATAN:
        case Key_Code::LOG:  case Key_Code::LN:   case Key_Code::EXP:
        case Key_Code::SQRT:
        case Key_Code::PI:   case Key_Code::EULER: case Key_Code::PHI: case Key_Code::TAU:
        case Key_Code::BIT_AND: case Key_Code::BIT_OR:  case Key_Code::BIT_XOR:
        case Key_Code::SHIFT_LEFT: case Key_Code::SHIFT_RIGHT:
        case Key_Code::APPROX:
            try_insert(code);
            break;

        case Key_Code::NEGATE:
            if (!m_state.expression.empty()) {
                m_state.expression.insert(Key_Code::SUBTRACT);
                m_state.display_value = m_state.expression.render_string();
            }
            break;

        case Key_Code::CURSOR_LEFT:
            m_state.expression.cursor_left();
            break;

        case Key_Code::CURSOR_RIGHT:
            m_state.expression.cursor_right();
            break;

        case Key_Code::BACKSPACE:
            m_state.expression.backspace();
            m_state.display_value = m_state.expression.empty() ? "0" : m_state.expression.render_string();
            break;

        case Key_Code::CLEAR:
            m_state.expression.clear();
            m_state.display_value = "0";
            break;

        case Key_Code::ALL_CLEAR:
            reset();
            break;

        case Key_Code::MEM_STORE:
            try {
                m_state.memory = Parser(m_state.expression.eval_string()).parse()->eval();
            } catch (...) {}
            break;
        case Key_Code::MEM_RECALL:
            try_insert(Key_Code::EULER); // placeholder — MEM_RECALL inserts numeric string
            break;
        case Key_Code::MEM_ADD:
            try {
                m_state.memory += Parser(m_state.expression.eval_string()).parse()->eval();
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

/****************************/
/*    Expression Insert     */
/****************************/
void Calc_Engine::try_insert(Key_Code code) {
    m_state.expression.insert(code);
    m_state.display_value = m_state.expression.render_string();
}

/****************************/
/*       Evaluation         */
/****************************/
void Calc_Engine::evaluate() {
    if (m_state.expression.empty()) return;
    try {
        std::string expr_str = m_state.expression.eval_string();
        Parser        p(expr_str);
        ovb::ast::Node_Ptr tree      = p.parse();
        ovb::ast::Node_Ptr result    = tree->simplify();

        std::string result_str = result->to_string();
        m_state.display_value = result_str;
        m_state.last_ast      = std::move(tree);
        m_result_shown        = true;

        // Push to history
        m_state.history.push_front({expr_str, result_str});
        if (m_state.history.size() > Calc_State::MAX_HISTORY)
            m_state.history.pop_back();
    } catch (const std::exception& e) {
        m_state.error         = e.what();
        m_state.display_value = "Error";
    }
}

/****************************/
/*     Math Layout Mode     */
/****************************/
void Calc_Engine::toggle_math_layout() {
    m_state.use_math_layout = !m_state.use_math_layout;
}

} // namespace ovb::math

