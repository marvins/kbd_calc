/**
 * @file    calculator_app.cpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Calculator application panel implementation
 */
#include <overboard/gui/calculator_app.hpp>

// Project Libraries
#include <overboard/gui/lcd_section.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::gui {

/*******************************/
/*            Impl             */
/*******************************/
struct Calculator_App::Impl {
    math::Calc_Engine&           engine;
    core::Layer_Manager&         layers;
    Back_Cb                      on_back;
    std::unique_ptr<LCD_Section> lcd;
    std::unique_ptr<Header_Bar>  header;
    std::unique_ptr<Footer_Bar>  footer;
    lv_obj_t*                    container = nullptr;

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
Calculator_App::~Calculator_App() = default;

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

    m_impl->lcd = std::make_unique<LCD_Section>(m_impl->engine, m_impl->layers);
    m_impl->lcd->build(lcd_parent);

    // Footer bar — decorative only, navigation via ESCAPE
    m_impl->footer = std::make_unique<Footer_Bar>(m_impl->container, width);
}

/*******************************/
/*          Deactivate         */
/*******************************/
void Calculator_App::deactivate() {
    LOG_DEBUG("Calculator_App: deactivating");
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
/*           Refresh           */
/*******************************/
void Calculator_App::refresh() {
    if (m_impl->lcd) {
        m_impl->lcd->refresh();
    }
}

/*******************************/
/*            Name             */
/*******************************/
std::string Calculator_App::name() const {
    return "Calculator";
}

/*******************************/
/*      Get Custom Label       */
/*******************************/
std::string Calculator_App::get_custom_label(int key_index) const {
    // In calculator context, keys 3-6 act as F1-F4 function keys
    switch (key_index) {
        case 3: return "F1";
        case 4: return "F2";
        case 5: return "F3";
        case 6: return "F4";
        default: return ""; // Use default from keyboard.json
    }
}

} // namespace ovb::gui
