/**
 * @file    app.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   PicoCalc hardware application implementation
 */
#include <overboard/hal/picocalc/app.hpp>

// C++ Standard Libraries
#include <iostream>
#include <stdexcept>

// Third-Party Libraries
#include <lvgl.h>
#ifdef TARGET_RP2350
#include <pico/stdlib.h>
#endif

// Project Libraries
#include <overboard/core/keymap.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/io/via_layout.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::picocalc {

/****************************/
/*       Constructor        */
/****************************/
PicoCalc_App::PicoCalc_App(const core::Grid_Layout& layout)
    : m_layout(layout),
      m_layers(m_keymap)
{}

/****************************/
/*        Destructor        */
/****************************/
PicoCalc_App::~PicoCalc_App() = default;

/****************************/
/*          Create          */
/****************************/
std::unique_ptr<PicoCalc_App> PicoCalc_App::create(
    const core::Grid_Layout&     layout,
    const std::filesystem::path& layout_path,
    const std::filesystem::path& keymap_path,
    const std::filesystem::path& layers_path )
{
    auto app = std::unique_ptr<PicoCalc_App>(new PicoCalc_App(layout));

    // Load layer assignments
    try {
        // Parse layout to build matrix position -> visual index map
        auto via_layout = io::parse_via_layout(layout_path);
        auto matrix_index_map = io::build_matrix_index_map(via_layout);
        auto layers = core::load_layers_from_json(layers_path.string(), matrix_index_map);
        app->m_keymap = core::Keymap(layers);
    } catch (const std::exception& e) {
        std::cerr << "PicoCalc_App: failed to load layers: " << e.what() << "\n";
        return nullptr;
    }

    app->m_layout_path = layout_path;
    app->m_keymap_path = keymap_path;
    app->m_layers_path = layers_path;

    if (!app->init()) {
        return nullptr;
    }
    return app;
}

/****************************/
/*           Init           */
/****************************/
bool PicoCalc_App::init() {
    lv_init();

    try {
        // Create the hardware display (initialises SPI + ILI9488 + LVGL driver)
        m_display = std::make_unique<PicoCalc_Display>();

        // Create the GUI view on the LVGL screen
        m_view = std::make_unique<gui::App_View>(
            m_display->screen(), m_layout, m_engine, m_layers);

        // Create the I2C keyboard input driver
        m_input = std::make_unique<PicoCalc_Input>(m_layout);

        // Load scancode → key index bindings from VIA JSON if available
        if (!m_layout_path.empty() && !m_keymap_path.empty()) {
            try {
                auto via_layout = io::parse_via_layout(m_layout_path);
                io::apply_scancodes_from_json(via_layout, m_keymap_path);
                auto sc_map = io::build_scancode_index_map(via_layout);
                for (const auto& [scancode, key_index] : sc_map) {
                    // Scancodes in VIA JSON are stored as uint; cast to uint8_t
                    // for the I2C key code space (0x00–0xFF)
                    if (scancode <= 0xFF) {
                        m_input->bind(static_cast<uint8_t>(scancode), key_index);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "PicoCalc_App: warning: failed to load scancodes: "
                          << e.what() << "\n";
            }
        }

        // Wire LVGL button click callback
        m_view->set_key_callback(on_key_clicked, this);

        m_view->render();
        m_initialized = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "PicoCalc_App init error: " << e.what() << "\n";
        return false;
    }
}

/****************************/
/*            Run           */
/****************************/
void PicoCalc_App::run() {
    while (!m_should_quit && !m_input->should_quit()) {
        m_input->pump();

        hal::Key_Event event;
        while (m_input->poll(event)) {
            if (event.type == hal::Key_Event_Type::Press) {
                handle_key(event.key_index);
            }
        }

        lv_timer_handler();
        lv_tick_inc(20);

#ifdef TARGET_RP2350
        sleep_ms(5);
#endif
    }
}

/****************************/
/*       On Key Clicked     */
/****************************/
void PicoCalc_App::on_key_clicked(int key_index, void* user_data) {
    static_cast<PicoCalc_App*>(user_data)->handle_key(key_index);
}

/****************************/
/*       Handle Key         */
/****************************/
void PicoCalc_App::handle_key(int key_index) {
    const core::Key_Code code = m_layers.key_at(key_index);

    switch (code) {
        case core::Key_Code::LAYER_NEXT:
            m_layers.next_layer();
            m_view->update_layer();
            break;
        case core::Key_Code::LAYER_PREV:
            m_layers.prev_layer();
            m_view->update_layer();
            break;
        case core::Key_Code::LAYER_CONST:
            m_layers.set_layer(2);
            m_view->update_layer();
            break;
        case core::Key_Code::LAYER_ALG:
            m_layers.set_layer(4);
            m_view->update_layer();
            break;
        case core::Key_Code::LAYER_HOME:
            m_layers.set_layer(0);
            m_view->update_layer();
            break;
        case core::Key_Code::TOGGLE_MATH_LAYOUT:
            m_engine.toggle_math_layout();
            break;
        default:
            m_engine.handle_key(code);
            break;
    }
    m_view->refresh();
}

/****************************/
/*        Get Display       */
/****************************/
I_Display& PicoCalc_App::get_display() {
    return *m_view;
}

/****************************/
/*       Should Quit        */
/****************************/
bool PicoCalc_App::should_quit() const {
    return m_input ? m_input->should_quit() : m_should_quit;
}

} // namespace ovb::hal::picocalc
