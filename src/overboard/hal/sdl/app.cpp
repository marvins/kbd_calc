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
#include <overboard/core/action_code.hpp>
#include <overboard/core/input_key.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/sdl/keymap.hpp>
#include <overboard/hal/sdl/input.hpp>
#include <overboard/io/keyboard_config.hpp>
#include <overboard/log/stdout_logger.hpp>
#include <overboard/resources/embedded_json.hpp>

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
                                          const std::filesystem::path& layout_path ) {
    auto app = std::unique_ptr<SDL_App>(new SDL_App(layout));

    // Load keyboard configuration with embedded fallback
    try {
        io::Keyboard_Config keyboard_config = io::load_keyboard_config(layout_path);
        app->m_keymap = io::config_to_keymap(keyboard_config);
        app->m_layout_path = layout_path;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load keyboard config: " << e.what() << "\n";
        return nullptr;
    }

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

        // Build input_key -> key_index mapping from keyboard.json
        LOG_TRACE("Building input_key mapping from keyboard.json");
        try {
            auto keyboard_config = io::load_keyboard_config(m_layout_path);
            int bindings_count = 0;
            for (const auto& [key_id_str, key_def] : keyboard_config.keys) {
                int key_id = std::stoi(key_id_str);
                core::Input_Key input_key = core::string_to_input_key(key_def.input_key);
                if (input_key != core::Input_Key::NONE) {
                    m_sdl_keymap.bind(input_key, key_id);
                    bindings_count++;
                }
            }
            LOG_TRACE("Input key mapping loaded: " + std::to_string(bindings_count) + " keys");
        } catch (const std::exception& e) {
            std::cerr << "Warning: failed to build input_key mapping: " << e.what() << "\n";
        }

        // Create SDL input handler for physical keyboard mapping
        LOG_TRACE("Creating SDL input handler");
        m_input = std::make_unique<SDL_Input>();
        m_input->keymap() = m_sdl_keymap;
        LOG_TRACE("SDL input handler created successfully");

#if SHOW_KEYBOARD_UI
        // Create separate keyboard window when SHOW_KEYBOARD_UI is enabled
        LOG_TRACE("Creating separate keyboard window");
        m_keyboard_window = std::make_unique<Keyboard_Window>(
            "Keyboard", hal::KBD_WIDTH, hal::KBD_WIN_HEIGHT);
        LOG_TRACE("Keyboard window created successfully");

        // Create keyboard display in separate window (interactive buttons)
        LOG_TRACE("Creating Keyboard_Display in separate window");
        m_keyboard_display = std::make_unique<gui::Keyboard_Display>(
            m_keyboard_window->screen(), m_layout, m_layers,
            hal::KBD_WIDTH, hal::KBD_WIN_HEIGHT);
        LOG_TRACE("Keyboard_Display created successfully");

        // Wire keyboard button clicks to the same handler as physical keys
        m_keyboard_display->set_click_callback([this](int key_index) {
            on_key_clicked(key_index, this);
        });

        // Create key mapping info panel only when the main window has space for it
        // (KBD_HEIGHT > 0 means a keyboard region was reserved below the LCD)
        if constexpr (hal::KBD_HEIGHT > 0) {
            LOG_TRACE("Creating Key_Mapping_Info panel in main window");
            m_key_mapping_info = std::make_unique<gui::Key_Mapping_Info>(
                m_display->screen(), m_layout, m_layers,
                hal::FULL_WIDTH, hal::KBD_WIN_HEIGHT,
                [this](int key_index) { return m_view->get_active_panel_label(key_index); });
            lv_obj_set_pos(m_key_mapping_info->container(), 0, hal::LCD_HEIGHT);
            LOG_TRACE("Key_Mapping_Info panel created successfully");
        }

        // Subscribe both displays to layer changes
        m_layers.on_layer_change([this]([[maybe_unused]] int layer_index) {
            if (m_keyboard_display) {
                m_keyboard_display->update_layer();
            }
            if (m_key_mapping_info) {
                m_key_mapping_info->update_layer();
            }
        });

        // Subscribe to panel changes to update Key_Mapping_Info labels
        m_view->set_panel_change_callback([this]([[maybe_unused]] gui::I_Panel* panel) {
            LOG_DEBUG("SDL_App: panel change callback, panel='", panel ? panel->name() : "null", "'");
            if (m_key_mapping_info) {
                LOG_DEBUG("SDL_App: calling update_layer");
                m_key_mapping_info->update_layer();
                LOG_DEBUG("SDL_App: update_layer done");
            }
            LOG_DEBUG("SDL_App: panel change callback complete");
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
                    // Macropad: key_index -> action via layers
                    handle_key(key_event.key_index);
                } else if (key_event.kind == hal::Key_Event_Kind::Direct_Action) {
                    // Standard keyboard: Input_Key -> Action_Code directly
                    handle_direct_action(key_event.input_key);
                } else {
                    // Text input - panel handles refresh if consumed
                    if (!m_view->handle_text(key_event.codepoint)) {
                        m_view->refresh();
                    }
                }
            }
        }

        // Track shift state for future mode system
        // Note: Hardcoded Shift layer switching removed.
        // Layer switching will be handled by the app mode system.
        [[maybe_unused]] int current_shift_state = m_input->is_shift_pressed() ? 1 : 0;

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
    // Virtual keyboard buttons: emit text from label if printable, else action
    const std::string label = m_layers.label_at(key_index);

    // Check if label is a single printable character
    if (label.length() == 1) {
        char32_t text_char = static_cast<char32_t>(label[0]);
        LOG_DEBUG("Keypress: key_index=" + std::to_string(key_index) + ", label='" + label + "' -> text");
        m_view->handle_text(text_char);
        return;
    }

    // Not printable text - handle as action
    const core::Action_Code code = m_layers.action_at(key_index);
    LOG_DEBUG("Keypress: key_index=" + std::to_string(key_index) + ", action_code=" + std::to_string(static_cast<int>(code)) + " (" + core::action_code_to_display(code) + ")");
    m_view->handle_input(code);
}

/****************************************/
/*   Handle Direct Action (standard kbd) */
/****************************************/
void SDL_App::handle_direct_action(core::Input_Key key) {
    LOG_DEBUG("Standard keyboard: input_key=" + core::input_key_to_string(key));
    // Pass Input_Key directly to GUI - panels decide what it means
    m_view->handle_input_key(key);
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
