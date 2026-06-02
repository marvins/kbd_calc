/**
 * @file    app.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   SDL simulator application implementation
 */
#include <overboard/hal/sdl/app.hpp>

// C++ Standard Libraries
#include <iostream>
#include <stdexcept>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>

// Project Libraries
#include <overboard/core/keymap.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/sdl/keymap.hpp>
#include <overboard/hal/sdl/input.hpp>
#include <overboard/io/via_layout.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

/********************************/
/*          Constructor         */
/********************************/
SDL_App::SDL_App(const core::Grid_Layout& layout)
    : m_layout(layout),
      m_layers(m_keymap) {
}

/********************************/
/*          Destructor          */
/********************************/
SDL_App::~SDL_App() {
    if (m_initialized) {
        SDL_Quit();
    }
}

/************************************/
/*          Create the app          */
/************************************/
std::unique_ptr<SDL_App> SDL_App::create( const core::Grid_Layout&     layout,
                                          const std::filesystem::path& layout_path,
                                          const std::filesystem::path& keymap_path,
                                          const std::filesystem::path& layers_path ) {
    auto app = std::unique_ptr<SDL_App>(new SDL_App(layout));

    // Load layer assignments from layers JSON
    try {
        // Parse layout to build matrix position -> visual index map
        auto via_layout = io::parse_via_layout(layout_path);
        auto matrix_index_map = io::build_matrix_index_map(via_layout);
        auto layers = core::load_layers_from_json(layers_path.string(), matrix_index_map);
        app->m_keymap = core::Keymap(layers);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load layers from " << layers_path << ": " << e.what() << "\n";
        return nullptr;
    }

    app->m_layout_path  = layout_path;
    app->m_keymap_path  = keymap_path;
    app->m_layers_path  = layers_path;

    if (!app->init()) {
        return nullptr;
    }
    return app;
}

/****************************************************/
/*          Initialize the SDL application          */
/****************************************************/
bool SDL_App::init() {
    // Set up signal handlers early (before LVGL init) to ensure Ctrl-C works
    setup_signal_handlers();

    // Initialise LVGL before SDL so the SDL driver is registered
    lv_init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return false;
    }

    m_initialized = true;

    try {
        // Create SDL window (HAL)
        LOG_TRACE("Creating SDL display");
        m_display = std::make_unique<Display>("Calculator", DISPLAY_WIDTH, DISPLAY_HEIGHT);
        LOG_TRACE("SDL display created successfully");

        // Create GUI view and attach to LVGL screen
        LOG_DEBUG("Creating App_View");
        m_view = std::make_unique<gui::App_View>(
            m_display->screen(), m_layout, m_engine, m_layers);
        LOG_TRACE("App_View created successfully");

        // Load input_key bindings from keymap JSON into the SDL keymap
        if (!m_layout_path.empty() && !m_keymap_path.empty()) {
            LOG_TRACE("Loading input_key bindings from " + m_layout_path.string());
            try {
                auto via_layout = io::parse_via_layout(m_layout_path);
                io::apply_input_keys_from_json(via_layout, m_keymap_path);
                auto key_map = io::build_input_key_index_map(via_layout);
                if (!key_map.empty()) {
                    m_sdl_keymap.load_from_map(key_map);
                }
                LOG_TRACE("Input key bindings loaded successfully");
            } catch (const std::exception& e) {
                std::cerr << "Warning: failed to load input_keys: " << e.what() << "\n";
            }
        }

        // Create SDL input handler for physical keyboard mapping
        LOG_TRACE("Creating SDL input handler");
        m_input = std::make_unique<SDL_Input>();
        m_input->keymap() = m_sdl_keymap;
        LOG_TRACE("SDL input handler created successfully");

#if TARGET_PICOSDL
#if SHOW_KEYBOARD_UI
        // PICOSDL: create separate keyboard window when SHOW_KEYBOARD_UI is enabled
        LOG_TRACE("Creating separate keyboard window");
        m_keyboard_window = std::make_unique<Keyboard_Window>(
            "Keyboard", hal::KBD_WIDTH, hal::KBD_WIN_HEIGHT);
        LOG_TRACE("Keyboard window created successfully");

        // Create keyboard display in separate window
        LOG_TRACE("Creating Keyboard_Display in separate window");
        m_keyboard_display = std::make_unique<gui::Keyboard_Display>(
            m_keyboard_window->screen(), m_layout, m_layers,
            hal::KBD_WIDTH, hal::KBD_WIN_HEIGHT);
        LOG_TRACE("Keyboard_Display created successfully");

        // Wire keyboard button clicks to the same handler as physical keys
        m_keyboard_display->set_click_callback([this](int key_index) {
            on_key_clicked(key_index, this);
        });

        // Subscribe keyboard display to layer changes
        m_layers.on_layer_change([this]([[maybe_unused]] int layer_index) {
            m_keyboard_display->update_layer();
        });
#endif
#else
        // SDL: keyboard is in the same window, wire through App_View
        m_view->set_key_click_callback([this](int key_index) {
            on_key_clicked(key_index, this);
        });
#endif

        LOG_DEBUG("Rendering initial view");
        m_view->render();
        LOG_TRACE("Initial view rendered successfully");

        return true;
    } catch (const std::exception& e) {
        std::cerr << "SDL_App init error: " << e.what() << "\n";
        return false;
    }
}

/************************************/
/*          Run the app             */
/************************************/
void SDL_App::run() {
    LOG_DEBUG("SDL_App::run() started");
    int loop_count = 0;
    while (!m_should_quit && !m_input->should_quit()) {

        // Pump SDL events (handles keyboard and mouse hit-testing)
        m_input->pump();

        // Process keyboard events from SDL_Input
        hal::Key_Event key_event;
        while (m_input->poll(key_event)) {
            if (key_event.type == hal::Key_Event_Type::Press) {
                if (key_event.kind == hal::Key_Event_Kind::Action) {
                    handle_key(key_event.key_index);
                } else {
                    m_view->handle_text(key_event.codepoint);
                    m_view->refresh();
                }
            }
        }

        lv_tick_inc(16);
        m_view->render();

        SDL_Delay(16);

        loop_count++;
    }
    LOG_DEBUG("SDL_App::run() exiting, loop_count=" + std::to_string(loop_count));
}

/************************************/
/*       Key Dispatch               */
/************************************/
void SDL_App::on_key_clicked(int key_index, void* user_data) {
    LOG_DEBUG("LVGL button clicked: key_index=" + std::to_string(key_index));
    static_cast<SDL_App*>(user_data)->handle_key(key_index);
}

/*********************************/
/*        Handle Keypress        */
/*********************************/
void SDL_App::handle_key(int key_index) {
    const core::Action_Code code = m_layers.action_at(key_index);
    LOG_DEBUG("Keypress: key_index=" + std::to_string(key_index) + ", action_code=" + std::to_string(static_cast<int>(code)) + " (" + core::action_code_to_display(code) + ")");
    m_view->handle_input(code);
    m_view->refresh();
}

/*******************************/
/*          Get Display        */
/*******************************/
I_Display& SDL_App::get_display() {
    return *m_view;
}

/****************************************/
/*      Check if the app should quit    */
/****************************************/
bool SDL_App::should_quit() const {
    return m_input ? m_input->should_quit() : m_should_quit;
}

} // namespace ovb::hal::sdl
