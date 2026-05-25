/**
 * @file    sdl_keyboard_display.cpp
 * @author  Marvin Smith
 * @date    2026-05-20
 *
 * @brief   SDL window owner for the keyboard display
 */
#include <overboard/hal/sdl/sdl_keyboard_display.hpp>

// C++ Standard Libraries
#include <stdexcept>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>

// Project Libraries
#include <overboard/hal/sdl/sdl_keymap.hpp>
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

// Structure to pass data to keyboard callback
struct Keyboard_Data {
    const SDL_Keymap* keymap = nullptr;
    SDL_App* app = nullptr;
};

extern "C" {
    static void key_read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
        (void)indev;
        data->key = 0;
        data->state = LV_INDEV_STATE_RELEASED;

        auto* kb_data = static_cast<Keyboard_Data*>(lv_indev_get_user_data(indev));
        if (!kb_data) return;

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN && ev.key.repeat == 0) {
                if (kb_data->keymap && kb_data->app) {
                    auto key_idx = kb_data->keymap->get_key_index(ev.key.keysym.scancode);
                    if (key_idx.has_value()) {
                        data->key = key_idx.value();
                        data->state = LV_INDEV_STATE_PRESSED;
                        kb_data->app->handle_key(key_idx.value());
                    }
                }
            }
        }
    }
}

/****************************/
/*  Constructor/Destructor  */
/****************************/
SDL_Keyboard_Display::SDL_Keyboard_Display( const std::string& title, int width, int height )
    : m_width(width), m_height(height)
{
    m_display = lv_sdl_window_create(width, height);
    if (!m_display) {
        throw std::runtime_error("SDL_Keyboard_Display: lv_sdl_window_create failed");
    }
    lv_sdl_window_set_title(m_display, title.c_str());

    // Register mouse indev so LVGL tracks cursor position for hover/click
    lv_sdl_mouse_create();

    // Run the timer handler once so the SDL window is actually created
    // before we query its handle
    lv_timer_handler();

    auto* renderer = static_cast<SDL_Renderer*>(lv_sdl_window_get_renderer(m_display));
    m_sdl_window = renderer ? SDL_RenderGetWindow(renderer) : nullptr;

    // Clear default screen padding
    lv_display_set_default(m_display);
    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);
    lv_obj_set_style_border_width(lv_screen_active(), 0, 0);
    lv_obj_set_style_bg_color( lv_screen_active(),
                               lv_color_hex(0x1E1E1E),
                               LV_PART_MAIN );
}

/****************************/
/*     Accessors            */
/****************************/
int    SDL_Keyboard_Display::width()     const { return m_width;  }
int    SDL_Keyboard_Display::height()    const { return m_height; }

uint32_t SDL_Keyboard_Display::window_id() const {
    return m_sdl_window ? SDL_GetWindowID(m_sdl_window) : 0;
}

lv_obj_t* SDL_Keyboard_Display::screen() const {
    lv_display_set_default(m_display);
    return lv_screen_active();
}

/****************************/
/*    I_Display stubs       */
/****************************/
void SDL_Keyboard_Display::clear(Color /*c*/)                                                           {}
void SDL_Keyboard_Display::draw_pixel(ovb::core::Point<int> /*pos*/, Color /*c*/)                       {}
void SDL_Keyboard_Display::draw_rect(ovb::core::Point<int> /*pos*/, ovb::core::Point<int> /*size*/,
                                       Color /*c*/, bool /*filled*/)                                      {}
void SDL_Keyboard_Display::draw_text(ovb::core::Point<int> /*pos*/, const std::string& /*text*/,
                                       Color /*fg*/, Color /*bg*/, int /*scale*/)                         {}

void SDL_Keyboard_Display::flush() {
    lv_timer_handler();
}

} // namespace ovb::hal::sdl
