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

    /// @brief Settings manager
    std::shared_ptr<core::Settings_Manager>  m_settings;

    /// @brief System info provider
    hal::I_System_Info&                     system_info;

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

    /// @brief Overlay push callback
    I_Panel::Overlay_Push_Cb                on_overlay_push;

    /// @brief Overlay pop callback
    I_Panel::Overlay_Pop_Cb                 on_overlay_pop;

    /// @brief Container object
    lv_obj_t*                               container = nullptr;

    /**
     * @brief Constructor
     * @param e  Calculator engine
     * @param l  Layer manager
     * @param cb Back callback
     * @param s  Settings manager
     */
    Impl( math::Calc_Engine& e,
         core::Layer_Manager& l,
         hal::I_System_Info& si,
         Back_Cb cb,
         std::shared_ptr<core::Settings_Manager> s)
        : engine(e),
          layers(l),
          m_settings(std::move(s)),
          system_info(si),
          on_back(std::move(cb)) {}
};

/*******************************/
/*          Constructor        */
/*******************************/
Calculator_App::Calculator_App(math::Calc_Engine&                      engine,
                               core::Layer_Manager&                    layers,
                               hal::I_System_Info&                     system_info,
                               Back_Cb                                 on_back,
                               std::shared_ptr<core::Settings_Manager> settings)
    : m_impl(std::make_unique<Impl>( engine,
                                     layers,
                                     system_info,
                                     std::move(on_back),
                                     std::move(settings) ) ) {}

/*******************************/
/*     Set Overlay Callbacks   */
/*******************************/
void Calculator_App::set_overlay_callbacks(Overlay_Push_Cb push, Overlay_Pop_Cb pop) {
    m_impl->on_overlay_push = std::move(push);
    m_impl->on_overlay_pop  = std::move(pop);
}


/*******************************/
/*          Destructor         */
/*******************************/
Calculator_App::~Calculator_App() {
    LOG_DEBUG("Calculator_App: destructor");
    if (m_impl->container) {
        deactivate();
    }
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
    lv_obj_set_scrollable(m_impl->container, false);

    const int width = lv_obj_get_width(parent);

    // Header bar
    m_impl->header = std::make_unique<Header_Bar>( m_impl->container,
                                                   width,
                                                   m_impl->system_info );
    m_impl->header->set_app_name("Calculator");

    // LCD section (between header and footer)
    lv_obj_t* lcd_parent = lv_obj_create(m_impl->container);
    const int lcd_h = lv_obj_get_height(parent) - Header_Bar::HEIGHT - Footer_Bar::HEIGHT;
    lv_obj_set_size(lcd_parent, width, lcd_h);
    lv_obj_align(lcd_parent, LV_ALIGN_TOP_LEFT, 0, Header_Bar::HEIGHT);
    lv_obj_set_style_pad_all(lcd_parent, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(lcd_parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(lcd_parent, 0, LV_PART_MAIN);
    lv_obj_set_scrollable(lcd_parent, false);

    LOG_DEBUG("Calculator_App: creating LCD_Section");
    m_impl->lcd = std::make_unique<LCD_Section>( m_impl->engine,
                                                 m_impl->layers,
                                                 m_impl->m_settings );
    LOG_DEBUG("Calculator_App: building LCD_Section");
    m_impl->lcd->build(lcd_parent, width, lcd_h);
    LOG_DEBUG("Calculator_App: LCD_Section built");

    // Footer bar
    m_impl->footer = std::make_unique<Footer_Bar>( m_impl->container,
                                                   width );
    m_impl->footer->set_label(0, "Alg");
    m_impl->footer->set_label(1, "Trig");
    m_impl->footer->set_label(2, "Const");

    // Callback for all function menu selections
    auto menu_callback = [this](core::Action_Code action) {
        m_impl->active_popup = nullptr;  // Clear active popup
        if (m_impl->on_overlay_pop) m_impl->on_overlay_pop();
        m_impl->engine.handle_key(action);
        refresh();
    };

    // F1: Alg menu
    std::vector<Function_Menu_Item> alg_items = {
        {"1/x",           "Inverse",       core::Action_Code::RECIPROCAL},
        {"x\xC2\xB2",     "Square",        core::Action_Code::POWER_2},
        {"x\xC2\xB3",     "Cube",          core::Action_Code::POWER_3},
        {"x^y",           "Power",         core::Action_Code::POWER_N},
        {"abs",           "Absolute Value",core::Action_Code::ABS},
        {"n!",            "Factorial",     core::Action_Code::FACTORIAL},
        {"\xE2\x88\x9A",  "Square Root",   core::Action_Code::SQRT},
        {"\xE2\x88\x9B",  "Cube Root",     core::Action_Code::CUBE_ROOT},
        {"nroot",         "Nth Root",      core::Action_Code::NTH_ROOT}
    };
    LOG_DEBUG("Calculator_App: alg_items.size()=" + std::to_string(alg_items.size()) + " before creating popup");
    LOG_DEBUG("Calculator_App: creating Alg popup");
    m_impl->f_key_popups[static_cast<int>(Popup_Menu::Alg)] =
        std::make_unique<Function_Menu_Popup>(m_impl->container, "Alg", alg_items, menu_callback);
    LOG_DEBUG("Calculator_App: Alg popup created");

    // F2: Trig menu (includes hyperbolic functions)
    std::vector<Function_Menu_Item> trig_items = {
        {"Sin",   "Sine",             core::Action_Code::SIN},
        {"Cos",   "Cosine",           core::Action_Code::COS},
        {"Tan",   "Tangent",          core::Action_Code::TAN},
        {"Cot",   "Cotangent",        core::Action_Code::COT},
        {"Sec",   "Secant",           core::Action_Code::SEC},
        {"Csc",   "Cosecant",         core::Action_Code::CSC},
        {"Sinh",  "Hyp. Sine",        core::Action_Code::SINH},
        {"Cosh",  "Hyp. Cosine",      core::Action_Code::COSH},
        {"Tanh",  "Hyp. Tangent",     core::Action_Code::TANH},
        {"Coth",  "Hyp. Cotangent",   core::Action_Code::COTH},
        {"Sech",  "Hyp. Secant",      core::Action_Code::SECH},
        {"Csch",  "Hyp. Cosecant",    core::Action_Code::CSCH},
        {"ASin",  "Inverse Sine",     core::Action_Code::ASIN},
        {"ACos",  "Inverse Cosine",   core::Action_Code::ACOS},
        {"ATan",  "Inverse Tangent",  core::Action_Code::ATAN},
        {"ACot",  "Inverse Cotangent",core::Action_Code::ACOT},
        {"Atan2", "Angle (y, x)",     core::Action_Code::ATAN2},
        {"ASinh", "Inv. Hyp. Sine",   core::Action_Code::ASINH},
        {"ACosh", "Inv. Hyp. Cosine", core::Action_Code::ACOSH},
        {"ATanh", "Inv. Hyp. Tangent",core::Action_Code::ATANH}
    };

    LOG_DEBUG("Calculator_App: creating Trig popup");
    m_impl->f_key_popups[static_cast<int>(Popup_Menu::Trig)] =
        std::make_unique<Function_Menu_Popup>( m_impl->container,
                                               "Trig",
                                               trig_items,
                                               menu_callback );
    LOG_DEBUG("Calculator_App: Trig popup created");

    // F3: Constants menu
    std::vector<Function_Menu_Item> const_items = {
        {"\u03c0", "Pi",          core::Action_Code::PI},
        {"e",      "Euler",       core::Action_Code::EULER},
        {"\u03c6", "Golden Ratio",core::Action_Code::PHI},
        {"\u03c4", "Tau",         core::Action_Code::TAU},
    };
    LOG_DEBUG("Calculator_App: creating Const popup");
    m_impl->f_key_popups[static_cast<int>(Popup_Menu::Const)] =
        std::make_unique<Function_Menu_Popup>( m_impl->container,
                                               "Const",
                                               const_items,
                                               menu_callback );
    LOG_DEBUG("Calculator_App: Const popup created");

    // F4-F10: Available for future menus
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
    // If a popup is active, route action codes to it first
    if (m_impl->active_popup) {
        bool handled = m_impl->active_popup->handle_input(action);
        // Clear active popup if ESC was pressed
        if (action == core::Action_Code::ESCAPE) {
            m_impl->active_popup = nullptr;
        }
        if (handled) return true;
    }

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
        case core::Action_Code::FUNC_1:
        case core::Action_Code::FUNC_2:
        case core::Action_Code::FUNC_3: {
            const int popup_index = static_cast<int>(action) - static_cast<int>(core::Action_Code::FUNC_1);
            auto& popup = m_impl->f_key_popups[static_cast<std::size_t>(popup_index)];
            if (popup) {
                m_impl->active_popup = popup.get();
                popup->show();
                if (m_impl->on_overlay_push) {
                    m_impl->on_overlay_push(popup->title(), build_popup_overlay(*popup));
                }
            }
            return true;
        }
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
            if (m_impl->on_overlay_pop) m_impl->on_overlay_pop();
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

        case core::Input_Key::HOME:
            m_impl->engine.handle_key(core::Action_Code::ANS);
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
            // Hide any currently active popup and pop its overlay
            if (m_impl->active_popup) {
                m_impl->active_popup->hide();
                m_impl->active_popup = nullptr;
                if (m_impl->on_overlay_pop) m_impl->on_overlay_pop();
            }

            // Map F-key to array index (F1=0, F2=1, ...)
            int popup_index = static_cast<int>(key) - static_cast<int>(core::Input_Key::F1);
            if (popup_index >= 0 && popup_index < F_KEY_POPUP_COUNT) {
                auto idx = static_cast<size_t>(popup_index);
                if (auto& popup = m_impl->f_key_popups[idx]) {
                    m_impl->active_popup = popup.get();
                    popup->show();

                    // Push keyboard overlay with numbered shortcuts
                    if (m_impl->on_overlay_push) {
                        m_impl->on_overlay_push( popup->title(),
                                                 build_popup_overlay( *popup ) );
                    }
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
    // If a popup is active, route digit keys as shortcut selections
    if (m_impl->active_popup && codepoint >= U'1' && codepoint <= U'9') {
        int index = static_cast<int>(codepoint - U'1');  // '1' -> 0, '2' -> 1, etc.
        auto* popup = dynamic_cast<Function_Menu_Popup*>(m_impl->active_popup);
        if (popup && popup->select_by_index(index)) {
            m_impl->active_popup = nullptr;
            if (m_impl->on_overlay_pop) m_impl->on_overlay_pop();
            refresh();
            return true;
        }
    }

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
    // Resolve the action code for this physical key in the base layer (layer 0),
    // then return the calculator-specific popup name.  This is keyboard-JSON-agnostic
    // — it doesn't matter which key index F1 lives on in the JSON.
    const auto& layer = m_impl->layers.current_layer();
    if (key_index < 0 || static_cast<std::size_t>(key_index) >= layer.keys.size()) {
        return "";
    }
    switch (layer.keys[static_cast<std::size_t>(key_index)]) {
        case core::Action_Code::ANS:    return "Ans";
        case core::Action_Code::FUNC_1: return "Alg";
        case core::Action_Code::FUNC_2: return "Trig";
        case core::Action_Code::FUNC_3: return "Const";
        case core::Action_Code::FUNC_4: return "F4";
        case core::Action_Code::FUNC_5: return "F5";
        default: return "";
    }
}

/******************************************/
/*        Build Popup Overlay             */
/******************************************/
std::vector<I_Panel::Overlay_Key_Desc>
Calculator_App::build_popup_overlay(const Function_Menu_Popup& popup) const {
    static constexpr std::array<core::Action_Code, 9> DIGIT_ACTIONS {
        core::Action_Code::DIGIT_1, core::Action_Code::DIGIT_2,
        core::Action_Code::DIGIT_3, core::Action_Code::DIGIT_4,
        core::Action_Code::DIGIT_5, core::Action_Code::DIGIT_6,
        core::Action_Code::DIGIT_7, core::Action_Code::DIGIT_8,
        core::Action_Code::DIGIT_9,
    };
    const auto& layer_keys = m_impl->layers.current_layer().keys;
    const auto& items      = popup.items();
    std::vector<Overlay_Key_Desc> overlay_keys;
    for (size_t i = 0; i < items.size() && i < DIGIT_ACTIONS.size(); ++i) {
        for (size_t k = 0; k < layer_keys.size(); ++k) {
            if (layer_keys[k] == DIGIT_ACTIONS[i]) {
                overlay_keys.push_back({
                    static_cast<int>(k),
                    std::to_string(i + 1) + ") " + items[i].label,
                    items[i].action
                });
                break;
            }
        }
    }
    return overlay_keys;
}

} // namespace ovb::gui
