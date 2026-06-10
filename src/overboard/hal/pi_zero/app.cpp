/**
 * @file    app.cpp
 * @author  Marvin Smith
 * @date    2026-06-05
 *
 * @brief   Pi Zero DRM application implementation
 */
#include <overboard/hal/pi_zero/app.hpp>

// C++ Standard Libraries
#include <array>
#include <iostream>
#include <stdexcept>
#include <thread>

// Third-Party Libraries
#include <lvgl.h>

// Project Libraries
#include <overboard/core/action_code.hpp>
#include <overboard/core/input_key.hpp>
#include <overboard/core/keymap.hpp>
#include <overboard/gui/app_view.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/io/keyboard_config.hpp>
#include <overboard/io/via_layout.hpp>
#include <overboard/log/stdout_logger.hpp>
#ifdef EMBEDDED_JSON
#include <overboard/resources/embedded_json.hpp>
#endif

namespace ovb::hal::pi_zero {

/********************************/
/*          Constructor         */
/********************************/
PiZero_App::PiZero_App(const core::Grid_Layout& layout)
    : m_layout(layout),
      m_layers(m_keymap) {
}

/********************************/
/*          Destructor          */
/********************************/
PiZero_App::~PiZero_App() = default;

/************************************/
/*          Create the app          */
/************************************/
std::unique_ptr<PiZero_App> PiZero_App::create(const core::Grid_Layout& layout,
                                               const std::filesystem::path& layout_path) {
    auto app = std::unique_ptr<PiZero_App>(new PiZero_App(layout));
    app->m_layout_path = layout_path;

    // Load keyboard configuration from keyboard.json
    io::Keyboard_Config keyboard_config;
    try {
        keyboard_config = io::parse_keyboard_config(layout_path.parent_path() / "keyboard.json");
    } catch (const std::exception& e) {
#ifdef EMBEDDED_JSON
        LOG_TRACE("Failed to load keyboard.json from disk, using embedded resource");
        try {
            keyboard_config = io::parse_keyboard_config_string(
                std::string(ovb::resources::embedded_json_data, ovb::resources::embedded_json_size)
            );
        } catch (const std::exception& e2) {
            std::cerr << "Failed to load embedded keyboard config: " << e2.what() << "\n";
            return nullptr;
        }
#else
        std::cerr << "Failed to load keyboard config: " << e.what() << "\n";
        return nullptr;
#endif
    }

    // Convert keyboard_config to Keymap format
    std::array<core::Layer, core::LAYER_COUNT> layers;
    layers.fill(core::Layer{});  // Initialize with empty layers

    for (std::size_t i = 0; i < keyboard_config.layers.size() && i < static_cast<std::size_t>(core::LAYER_COUNT); ++i) {
        const auto& config_layer = keyboard_config.layers[i];
        layers[i].name = config_layer.name;

        // Convert layer keys from string ID map to vector indexed by key ID
        for (const auto& [key_id_str, layer_key] : config_layer.keys) {
            std::size_t key_id = static_cast<std::size_t>(std::stoi(key_id_str));
            // Convert code string to Action_Code
            core::Action_Code code = core::string_to_action_code(layer_key.code);

            // Ensure layer.keys and labels vectors are large enough
            if (layers[i].keys.size() <= key_id) {
                layers[i].keys.resize(key_id + 1, core::Action_Code::NONE);
                layers[i].labels.resize(key_id + 1, "");
            }
            layers[i].keys[key_id]   = code;
            layers[i].labels[key_id] = layer_key.label;
        }
    }

    app->m_keymap = core::Keymap(layers);

    // Build Input_Key -> layout key index map from keyboard_config.keys
    app->m_input_key_map.fill(-1);
    for (const auto& [id_str, key_def] : keyboard_config.keys) {
        if (key_def.input_key.empty()) continue;
        auto input_key = core::string_to_input_key(key_def.input_key);
        if (input_key == core::Input_Key::NONE) continue;
        int key_idx = std::stoi(id_str);
        auto slot = static_cast<std::size_t>(input_key);
        if (slot < app->m_input_key_map.size()) {
            app->m_input_key_map[slot] = key_idx;
        }
    }

    if (!app->init()) {
        return nullptr;
    }
    return app;
}

/****************************/
/*          Init            */
/****************************/
bool PiZero_App::init() {
    LOG_DEBUG("PiZero_App: Initializing...");

    // Initialize LVGL
    lv_init();

    // Create DRM display
    try {
        m_display = std::make_unique<Display_DRM>(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    } catch (const std::exception& e) {
        LOG_ERROR("PiZero_App: Failed to create display: ", e.what());
        return false;
    }

    // Create main application view
    m_view = std::make_unique<gui::App_View>(
        m_display->screen(),
        m_layout,
        m_engine,
        m_layers
    );

    // Create input handler (try common keyboard devices)
    std::vector<std::string> input_devices = {
        "/dev/input/event0",
        "/dev/input/event1",
        "/dev/input/event2",
        "/dev/input/by-path/platform-3f804000.i2c-event",  // Pi Zero I2C keyboard
    };
    m_input = std::make_unique<Linux_Input>(input_devices);

    // Set initial layer
    m_layers.set_layer(0);

    m_initialized = true;
    LOG_DEBUG("PiZero_App: Initialization complete");
    return true;
}

/****************************/
/*          Run             */
/****************************/
void PiZero_App::run() {
    if (!m_initialized) {
        LOG_ERROR("PiZero_App: Not initialized, cannot run");
        return;
    }

    LOG_DEBUG("PiZero_App: Starting main loop");

    constexpr auto FRAME_TIME = std::chrono::milliseconds(16);  // ~60 FPS

    while (!m_should_quit) {
        auto frame_start = std::chrono::steady_clock::now();

        // Pump input events
        m_input->pump();

        // Process input events
        Key_Event event;
        while (m_input->poll(event)) {
            if (event.kind == Key_Event_Kind::Action) {
                if (event.type == Key_Event_Type::Press) {
                    handle_key(event.key_index);
                }
            }
        }

        // Update LVGL
        m_display->refresh();

        // Calculate sleep time to maintain frame rate
        auto frame_end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);
        if (elapsed < FRAME_TIME) {
            std::this_thread::sleep_for(FRAME_TIME - elapsed);
        }
    }

    LOG_DEBUG("PiZero_App: Main loop exiting");
}

/****************************/
/*      Should quit         */
/****************************/
bool PiZero_App::should_quit() const {
    return m_should_quit;
}

/****************************/
/*       Get display        */
/****************************/
I_Display& PiZero_App::get_display() {
    // TODO: Implement I_Display wrapper for Display_DRM
    throw std::runtime_error("get_display not fully implemented for Pi Zero");
}

/****************************/
/*       Get input          */
/****************************/
I_Input& PiZero_App::get_input() {
    return *m_input;
}

/****************************/
/*      Handle key          */
/****************************/
void PiZero_App::handle_key(int key_index) {
    // key_index here is actually a raw Input_Key enum value from Linux_Input::poll.
    // Translate it to the layout key index via m_input_key_map.
    auto slot = static_cast<std::size_t>(key_index);
    int layout_idx = (slot < m_input_key_map.size()) ? m_input_key_map[slot] : -1;
    if (layout_idx < 0) {
        return;  // Unmapped physical key
    }

    // Get the action from the current layer
    core::Action_Code action = m_layers.action_at(layout_idx);

    // Handle layer switching actions
    if (action == core::Action_Code::GO_HOME_LAYER) {
        m_layers.set_layer(0);
        m_view->refresh();
        return;
    }
    if (action == core::Action_Code::NEXT_LAYER) {
        m_layers.next_layer();
        m_view->refresh();
        return;
    }
    if (action == core::Action_Code::PREV_LAYER) {
        m_layers.prev_layer();
        m_view->refresh();
        return;
    }

    // Pass action to the view for handling
    m_view->handle_input(action);
}

/****************************/
/*    On key clicked        */
/****************************/
void PiZero_App::on_key_clicked(int key_index, void* user_data) {
    auto* app = static_cast<PiZero_App*>(user_data);
    app->handle_key(key_index);
}

} // namespace ovb::hal::pi_zero
