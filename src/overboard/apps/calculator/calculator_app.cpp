/**
 * @file    calculator_app.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Calculator application panel implementation
 */
#include <overboard/apps/calculator/calculator_app.hpp>

// C++ Standard Libraries
#include <array>

// Project Libraries
#include <overboard/gui/function_menu_popup.hpp>
#include <overboard/gui/lcd_section.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Calculator_App::Impl {

    /// @brief Calculator engine
    math::Calc_Engine&                      engine;

    /// @brief Layer manager
    core::Layer_Manager&                    layers;

    /// @brief Back callback
    Back_Cb                                 on_back;

    /// @brief LCD section
    std::unique_ptr<LCD_Section>            lcd;

    /// @brief Header bar
    std::unique_ptr<Header_Bar>             header;

    /// @brief Footer bar
    std::unique_ptr<Footer_Bar>             footer;

    /// @brief F-key popups
    std::array<std::unique_ptr<Function_Menu_Popup>, F_KEY_POPUP_COUNT> f_key_popups;

    /// @brief Currently visible popup
    Function_Menu_Popup*                    active_popup = nullptr;

    /// @brief Container object
    lv_obj_t*                               container = nullptr;

    /**
     * @brief Constructor
     * @param e Calculator engine
     * @param l Layer manager
     * @param cb Back callback
     */
    Impl(math::Calc_Engine& e, core::Layer_Manager& l, Back_Cb cb)
        : engine(e), layers(l), on_back(std::move(cb)) {}
};

/*******************************/
/*          Constructor        */
/*******************************/
Calculator_App::Calculator_App(math::Calc_Engine& engine,
                               core::Layer_Manager& layers,
                               Back_Cb on_back)
    : m_impl(std::make_unique<Impl>(engine, layers, std::move(on_back))) {}

/*******************************/
/*          Destructor         */
/*******************************/
Calculator_App::~Calculator_App() {
    LOG_DEBUG("Calculator_App: destructor");
}

/*******************************/
/*           Activate          */
/*******************************/
void Calculator_App::activate(lv_obj_t* parent) {
    LOG_DEBUG("Calculator_App: activating");

    m_impl->container = lv_obj_create(parent);
    lv_obj_set_size(m_impl->container, lv_pct(100), lv_pct(100));
    lv_obj_align(m_impl->container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_pad_all(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->container, LV_OBJ_FLAG_SCROLLABLE);

    const int width = lv_obj_get_width(parent);

    // Header bar
    m_impl->header = std::make_unique<Header_Bar>(m_impl->container, width);
    m_impl->header->set_app_name("Calculator");

    // LCD section (between header and footer)
    lv_obj_t* lcd_parent = lv_obj_create(m_impl->container);
    const int lcd_h = lv_obj_get_height(parent) - Header_Bar::HEIGHT - Footer_Bar::HEIGHT;
    lv_obj_set_size(lcd_parent, width, lcd_h);
    lv_obj_align(lcd_parent, LV_ALIGN_TOP_LEFT, 0, Header_Bar::HEIGHT);
    lv_obj_set_style_pad_all(lcd_parent, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(lcd_parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(lcd_parent, 0, LV_PART_MAIN);
    lv_obj_clear_flag(lcd_parent, LV_OBJ_FLAG_SCROLLABLE);

    LOG_DEBUG("Calculator_App: creating LCD_Section");
    m_impl->lcd = std::make_unique<LCD_Section>(m_impl->engine, m_impl->layers);
    LOG_DEBUG("Calculator_App: building LCD_Section");
    m_impl->lcd->build(lcd_parent, width, lcd_h);
    LOG_DEBUG("Calculator_App: LCD_Section built");

    // Footer bar — decorative only, navigation via ESCAPE
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);

    // Callback for all function menu selections
    auto menu_callback = [this](core::Action_Code action) {
        m_impl->active_popup = nullptr;  // Clear active popup
        m_impl->engine.handle_key(action);
        refresh();
    };

    // F1: Alg menu
    std::vector<Function_Menu_Item> alg_items = {
        {"1/x",   core::Action_Code::RECIPROCAL},
        {"x^2",   core::Action_Code::POWER_2},
        {"x^y",   core::Action_Code::POWER_N},
        {"sqrt",  core::Action_Code::SQRT},
        {"nroot", core::Action_Code::NONE}
    };
    LOG_DEBUG("Calculator_App: alg_items.size()=" + std::to_string(alg_items.size()) + " before creating popup");
    LOG_DEBUG("Calculator_App: creating Alg popup");
    m_impl->f_key_popups[static_cast<int>(Popup_Menu::Alg)] =
        std::make_unique<Function_Menu_Popup>(m_impl->container, "Alg", alg_items, menu_callback);
    LOG_DEBUG("Calculator_App: Alg popup created");

    // F2: Trig menu
    std::vector<Function_Menu_Item> trig_items = {
        {"Sin",   core::Action_Code::SIN},
        {"Cos",   core::Action_Code::COS},
        {"Tan",   core::Action_Code::TAN},
        {"Atan2", core::Action_Code::NONE},
        {"ASin",  core::Action_Code::ASIN},
        {"ACos",  core::Action_Code::ACOS},
        {"ATan",  core::Action_Code::ATAN},
        {"Sec",   core::Action_Code::NONE},
        {"Cosec", core::Action_Code::NONE}
    };
    LOG_DEBUG("Calculator_App: trig_items.size()=" + std::to_string(trig_items.size()) + " before creating popup");
    LOG_DEBUG("Calculator_App: creating Trig popup");
    m_impl->f_key_popups[static_cast<int>(Popup_Menu::Trig)] =
        std::make_unique<Function_Menu_Popup>(m_impl->container, "Trig", trig_items, menu_callback);
    LOG_DEBUG("Calculator_App: Trig popup created");

    // F3-F10: Available for future menus
    LOG_DEBUG("Calculator_App: activate complete");
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Calculator_App::deactivate() {
    LOG_DEBUG("Calculator_App: deactivating");
    m_impl->active_popup = nullptr;
    for (auto& popup : m_impl->f_key_popups) {
        popup.reset();
    }
    m_impl->footer.reset();
    m_impl->header.reset();
    m_impl->lcd.reset();
    if (m_impl->container) {
        lv_obj_del(m_impl->container);
        m_impl->container = nullptr;
    }
}

/*******************************/
/*        Handle Input         */
/*******************************/
bool Calculator_App::handle_input(core::Action_Code action) {
    switch (action) {
        case core::Action_Code::ESCAPE:
            if (m_impl->on_back) { m_impl->on_back(); }
            return true;
        case core::Action_Code::NEXT_LAYER:
            m_impl->layers.next_layer();
            return true;
        case core::Action_Code::PREV_LAYER:
            m_impl->layers.prev_layer();
            return true;
        case core::Action_Code::GO_HOME_LAYER:
            m_impl->layers.set_layer(0);
            return true;
        case core::Action_Code::GO_CONST_LAYER:
            m_impl->layers.set_layer(2);
            return true;
        case core::Action_Code::GO_ALG_LAYER:
            m_impl->layers.set_layer(4);
            return true;
        case core::Action_Code::TOGGLE_MATH_LAYOUT:
            m_impl->engine.toggle_math_layout();
            return true;
        default:
            m_impl->engine.handle_key(action);
            refresh();
            return true;
    }
}

/*******************************/
/*      Handle Input Key       */
/*******************************/
bool Calculator_App::handle_input_key(core::Input_Key key) {
    // If a popup is active, route input to it first
    if (m_impl->active_popup) {
        bool handled = m_impl->active_popup->handle_input(key);
        // Clear active popup if ESC was pressed
        if (key == core::Input_Key::ESCAPE) {
            m_impl->active_popup = nullptr;
        }
        if (handled) return true;
    }

    switch (key) {
        case core::Input_Key::ESCAPE:
            // Back/navigate out of calculator
            if (m_impl->on_back) { m_impl->on_back(); }
            return true;

        case core::Input_Key::RETURN:
        case core::Input_Key::NUMPAD_ENTER:
            // In calculator context, Enter/Return evaluates the expression
            m_impl->engine.handle_key(core::Action_Code::EVAL);
            refresh();
            return true;

        case core::Input_Key::LEFT:
            // Move cursor left in the expression
            m_impl->engine.state().expression.cursor_left();
            refresh();
            return true;

        case core::Input_Key::RIGHT:
            // Move cursor right in the expression
            m_impl->engine.state().expression.cursor_right();
            refresh();
            return true;

        case core::Input_Key::BACKSPACE:
            // Delete node to the left of cursor
            m_impl->engine.state().expression.backspace();
            refresh();
            return true;

        case core::Input_Key::DELETE:
            // Delete node to the right of cursor
            m_impl->engine.state().expression.delete_right();
            refresh();
            return true;

        case core::Input_Key::F1:
        case core::Input_Key::F2:
        case core::Input_Key::F3:
        case core::Input_Key::F4:
        case core::Input_Key::F5:
        case core::Input_Key::F6:
        case core::Input_Key::F7:
        case core::Input_Key::F8:
        case core::Input_Key::F9:
        case core::Input_Key::F10: {
            // Map F-key to array index (F1=0, F2=1, ...)
            int popup_index = static_cast<int>(key) - static_cast<int>(core::Input_Key::F1);
            if (popup_index >= 0 && popup_index < F_KEY_POPUP_COUNT) {
                auto idx = static_cast<size_t>(popup_index);
                if (auto& popup = m_impl->f_key_popups[idx]) {
                    m_impl->active_popup = popup.get();
                    popup->show();
                    return true;
                }
            }
            // F-key without assigned popup - ignore
            LOG_DEBUG("Calculator_App: F-key without popup (F", std::to_string(popup_index + 1), ")");
            return true;
        }

        default:
            return false;
    }
}

/*******************************/
/*        Handle Text          */
/*******************************/
bool Calculator_App::handle_text(char32_t codepoint) {
    // Handle digit and operator input from standard keyboard
    LOG_DEBUG("Calculator received text: " + std::to_string(static_cast<uint32_t>(codepoint)));

    core::Action_Code action = core::Action_Code::NONE;

    // Map text input to action codes
    switch (codepoint) {
        // Digits
        case U'0': action = core::Action_Code::DIGIT_0; break;
        case U'1': action = core::Action_Code::DIGIT_1; break;
        case U'2': action = core::Action_Code::DIGIT_2; break;
        case U'3': action = core::Action_Code::DIGIT_3; break;
        case U'4': action = core::Action_Code::DIGIT_4; break;
        case U'5': action = core::Action_Code::DIGIT_5; break;
        case U'6': action = core::Action_Code::DIGIT_6; break;
        case U'7': action = core::Action_Code::DIGIT_7; break;
        case U'8': action = core::Action_Code::DIGIT_8; break;
        case U'9': action = core::Action_Code::DIGIT_9; break;

        // Decimal point
        case U'.': action = core::Action_Code::DECIMAL; break;

        // Operators
        case U'+': action = core::Action_Code::ADD; break;
        case U'-': action = core::Action_Code::SUBTRACT; break;
        case U'*': action = core::Action_Code::MULTIPLY; break;
        case U'/': action = core::Action_Code::DIVIDE; break;
        case U'^': action = core::Action_Code::POWER_N; break;

        // Parentheses
        case U'(': action = core::Action_Code::PAREN_OPEN; break;
        case U')': action = core::Action_Code::PAREN_CLOSE; break;

        // Enter/Equals
        case U'=': action = core::Action_Code::EVAL; break;

        default:
            return false; // Unhandled text
    }

    if (action != core::Action_Code::NONE) {
        m_impl->engine.handle_key(action);
        refresh();
        return true;
    }

    return false;
}

/*******************************/
/*           Refresh           */
/*******************************/
void Calculator_App::refresh() {
    if (m_impl->lcd) {
        m_impl->lcd->refresh();
    }
}

/*******************************/
/*      Get Custom Label       */
/*******************************/
std::string Calculator_App::get_custom_label(int key_index) const {
    // In calculator context, keys 3-6 act as function menu keys
    switch (key_index) {
        case 3: return "Alg";
        case 4: return "Trig";
        case 5: return "F3";
        case 6: return "F4";
        default: return ""; // Use default from keyboard.json
    }
}

} // namespace ovb::gui
