/**
 * @file engine.cpp
 * @author Marvin Smith
 * @date 5/20/2026
 *
 * @brief Layout engine implementation
 */
#include <overboard/math/layout/engine.hpp>

// Project Libraries
#include <overboard/core/point.hpp>

// C++ Standard Libraries
#include <algorithm>

using namespace ovb::ast;

namespace ovb::layout {

/****************************/
/*   Layout_Box Factories   */
/****************************/
Layout_Box Layout_Box::atom(std::string t, int scale) {
    int sw = static_cast<int>(t.size()) * 6 * scale;
    int sh = 7 * scale;
    return {Box_Kind::ATOM, core::Point<int>(0, 0), core::Size<int>(sw, sh), sh - 2 * scale, scale, std::move(t), {}};
}

Layout_Box Layout_Box::fraction(Layout_Box num, Layout_Box den, int scale) {
    return { Box_Kind::FRACTION,
             core::Point<int>(0, 0),
             core::Size<int>(0, 0),
             0,
             scale,
             "",
             { std::move(num),
               std::move(den) } };
}

Layout_Box Layout_Box::power(Layout_Box base, Layout_Box exp, int scale) {
    return { Box_Kind::POWER,
             core::Point<int>(0, 0),
             core::Size<int>(0, 0),
             0,
             scale,
             "",
             { std::move(base),
               std::move(exp) } };
}

Layout_Box Layout_Box::sequence(std::vector<Layout_Box> boxes, int scale) {
    return { Box_Kind::SEQUENCE,
             core::Point<int>(0, 0),
             core::Size<int>(0, 0),
             0,
             scale,
             "",
             std::move(boxes)};
}

/****************************************/
/*          Handle Square Root          */
/****************************************/
Layout_Box Layout_Box::sqrt(Layout_Box arg, int scale) {
    return { Box_Kind::SQRT,
             core::Point<int>(0, 0),
             core::Size<int>(0, 0),
             0,
             scale,
             "",
             { std::move(arg) } };
}

/****************************/
/*      Layout Engine       */
/****************************/
Layout_Engine::Layout_Engine(const font::Font_Metrics& metrics, int default_scale)
    : m_metrics(metrics), m_default_scale(default_scale) {}


/****************************/
/*         Build            */
/****************************/
Layout_Box Layout_Engine::build(const ovb::ast::Node* node) {
    return build(node, m_default_scale);
}

/****************************/
/*       Build (scale)      */
/****************************/
Layout_Box Layout_Engine::build(const ovb::ast::Node* node, int scale) {
    switch (node->kind) {
        case Node_Kind::NUMBER:
            return Layout_Box::atom(node->to_string(), scale);

        case Node_Kind::CONSTANT:
            return Layout_Box::atom(node->to_string(), scale);

        case Node_Kind::VARIABLE:
            return Layout_Box::atom(node->to_string(), scale);

        case Node_Kind::BINARY_OP:
            return build_binary_op(static_cast<const Binary_Op_Node*>(node), scale);

        case Node_Kind::UNARY_OP:
            return build_unary_op(static_cast<const Unary_Op_Node*>(node), scale);

        case Node_Kind::FUNCTION:
            return build_function(static_cast<const Function_Node*>(node), scale);

        case Node_Kind::FACTORIAL:
            return Layout_Box::atom(node->to_string(), scale);
    }

    return Layout_Box::atom("?", scale);
}

/****************************/
/*     Build Binary Op      */
/****************************/
Layout_Box Layout_Engine::build_binary_op(const ovb::ast::Binary_Op_Node* node, int scale) {
    auto left = build(node->left.get(), scale);
    auto right = build(node->right.get(), scale);

    switch (node->op) {
        case Binary_Op::DIVIDE: {
            return Layout_Box::fraction(std::move(left), std::move(right), scale);
        }
        case Binary_Op::POWER: {
            return Layout_Box::power(std::move(left), std::move(right), scale);
        }
        case Binary_Op::MULTIPLY: {
            std::vector<Layout_Box> seq;
            seq.reserve(3);
            seq.push_back(std::move(left));
            seq.push_back(Layout_Box::atom("*", scale));
            seq.push_back(std::move(right));
            return Layout_Box::sequence(std::move(seq), scale);
        }
        case Binary_Op::ADD: {
            std::vector<Layout_Box> seq;
            seq.reserve(3);
            seq.push_back(std::move(left));
            seq.push_back(Layout_Box::atom("+", scale));
            seq.push_back(std::move(right));
            return Layout_Box::sequence(std::move(seq), scale);
        }
        case Binary_Op::SUBTRACT: {
            std::vector<Layout_Box> seq;
            seq.reserve(3);
            seq.push_back(std::move(left));
            seq.push_back(Layout_Box::atom("-", scale));
            seq.push_back(std::move(right));
            return Layout_Box::sequence(std::move(seq), scale);
        }
        default:
            // Fall back to string representation for unhandled operators
            return Layout_Box::atom(node->to_string(), scale);
    }
}

/****************************/
/*     Build Unary Op       */
/****************************/
Layout_Box Layout_Engine::build_unary_op(const ovb::ast::Unary_Op_Node* node, int scale) {
    auto operand = build(node->operand.get(), scale);

    if (node->op == Unary_Op::NEGATE) {
        std::vector<Layout_Box> seq;
        seq.reserve(2);
        seq.push_back(Layout_Box::atom("-", scale));
        seq.push_back(std::move(operand));
        return Layout_Box::sequence(std::move(seq), scale);
    }
    return Layout_Box::atom(node->to_string(), scale);
}

/****************************/
/*      Build Function      */
/****************************/
Layout_Box Layout_Engine::build_function(const ovb::ast::Function_Node* node, int scale) {
    // Special case: sqrt function uses proper mathematical notation
    if (node->name == "sqrt" && node->args.size() == 1) {
        return Layout_Box::sqrt(build(node->args[0].get(), scale), scale);
    }

    std::vector<Layout_Box> seq;
    // Reserve: name + ( + args + commas + )
    seq.reserve(node->args.size() * 2 + 3);

    // Function name
    seq.push_back(Layout_Box::atom(node->name, scale));

    // Opening paren
    seq.push_back(Layout_Box::atom("(", scale));

    // Arguments
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (i > 0) {
            seq.push_back(Layout_Box::atom(",", scale));
        }
        seq.push_back(build(node->args[i].get(), scale));
    }

    // Closing paren
    seq.push_back(Layout_Box::atom(")", scale));

    return Layout_Box::sequence(std::move(seq), scale);
}

/****************************/
/*        Measure           */
/****************************/
void Layout_Engine::measure(Layout_Box& box) {
    const int pad = 2;  // Padding around fractions

    switch (box.kind) {
        case Box_Kind::ATOM:
            box.size.x = m_metrics.string_width(box.text) * box.scale;
            box.size.y = m_metrics.line_height() * box.scale;
            box.baseline = m_metrics.ascent * box.scale;
            break;

        case Box_Kind::SEQUENCE: {
            int total_w = 0;
            int max_h = 0;
            int max_baseline = 0;
            for (auto& child : box.children) {
                measure(child);
                total_w += child.size.x + 2;  // 2px spacing
                max_h = std::max(max_h, child.size.y);
                max_baseline = std::max(max_baseline, child.baseline);
            }
            box.size.x = total_w;
            box.size.y = max_h;
            box.baseline = max_baseline;
            break;
        }

        case Box_Kind::FRACTION: {
            auto& num = box.children[0];
            auto& den = box.children[1];
            measure(num);
            measure(den);

            int line_height = box.scale;  // Vbar thickness
            int inner_w = std::max(num.size.x, den.size.x) + 2 * pad;
            int inner_h = num.size.y + line_height + den.size.y + 2 * pad;

            box.size.x = inner_w;
            box.size.y = inner_h;
            box.baseline = num.size.y + pad + line_height / 2;  // Centered on vbar
            break;
        }

        case Box_Kind::POWER: {
            auto& base = box.children[0];
            auto& exp = box.children[1];

            // Exponent is smaller
            exp.scale = std::max(1, base.scale - 1);
            measure(base);
            measure(exp);

            // Exponent sits top-right, raised by ~60% of base height
            int raise = base.size.y * 3 / 5;
            box.size.x = base.size.x + exp.size.x;
            box.size.y = std::max(base.size.y, raise + exp.size.y);
            box.baseline = base.baseline;
            break;
        }

        case Box_Kind::SUPERSCRIPT:
            // TODO: Implement if needed
            break;

        case Box_Kind::SQRT: {
            auto& arg = box.children[0];
            measure(arg);

            // Space for √ symbol (reduced to bring tick closer to text)
            int symbol_width = 2 * box.scale;

            // Vertical padding for horizontal bar above text
            int top_pad = 2 * box.scale;

            // Text offset from bars (2 pixels right and down)
            int text_offset = 2;

            // Total width: symbol + argument + padding + text offset
            box.size.x = symbol_width + arg.size.x + 2 + text_offset;
            box.size.y = arg.size.y + top_pad + text_offset;
            box.baseline = arg.baseline + top_pad + text_offset;  // Shift baseline down
            break;
        }
    }
}

/****************************/
/*         Layout           */
/****************************/
void Layout_Engine::layout(Layout_Box& box, core::Point<int> pos) {
    box.pos = pos;

    const int pad = 2;

    switch (box.kind) {
        case Box_Kind::ATOM:
            // Position is set, nothing else to do
            break;

        case Box_Kind::SEQUENCE: {
            int cx = pos.x;
            for (auto& child : box.children) {
                // Align baselines
                int child_y = pos.y + box.baseline - child.baseline;
                layout(child, core::Point<int>(cx, child_y));
                cx += child.size.x + 2;
            }
            break;
        }

        case Box_Kind::FRACTION: {
            auto& num = box.children[0];
            auto& den = box.children[1];

            int line_height = box.scale;

            // Center numerator above line
            int num_x = pos.x + (box.size.x - num.size.x) / 2;
            layout(num, core::Point<int>(num_x, pos.y));

            // Center denominator below line
            int den_x = pos.x + (box.size.x - den.size.x) / 2;
            int den_y = pos.y + num.size.y + line_height + 2 * pad;
            layout(den, core::Point<int>(den_x, den_y));
            break;
        }

        case Box_Kind::POWER: {
            auto& base = box.children[0];
            auto& exp = box.children[1];

            // Base at normal position
            int base_y = pos.y + box.baseline - base.baseline;
            layout(base, core::Point<int>(pos.x, base_y));

            // Exponent raised above baseline and to the right
            int raise = base.size.y * 4 / 5;  // 80% raise for higher superscript
            int exp_y = pos.y + box.baseline - exp.baseline - raise;
            layout(exp, core::Point<int>(pos.x + base.size.x, exp_y));
            break;
        }

        case Box_Kind::SUPERSCRIPT:
            // TODO: Implement if needed
            break;

        case Box_Kind::SQRT: {
            auto& arg = box.children[0];
            int symbol_width = 2 * box.scale;
            int top_pad = 2 * box.scale;
            int text_offset = 2;

            // Argument positioned to the right of the √ symbol, below top padding, with offset
            layout(arg, core::Point<int>(pos.x + symbol_width + text_offset, pos.y + top_pad + text_offset));
            break;
        }
    }
}

/****************************/
/*        Prepare           */
/****************************/
void Layout_Engine::prepare(Layout_Box& box, core::Point<int> container_size) {
    measure(box);
    int x = (container_size.x - box.size.x) / 2;
    int y = (container_size.y - box.size.y) / 2;
    layout( box, core::Point<int>( x, y ) );
}

} // namespace ovb::layout
