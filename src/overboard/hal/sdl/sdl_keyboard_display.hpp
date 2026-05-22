/**
 * @file    sdl_keyboard_display.hpp
 * @author  Marvin Smith
 * @date    2026-05-20
 *
 * @brief   SDL window owner for the keyboard display
 *
 * Creates and owns an LVGL SDL window for the keyboard. Satisfies
 * I_Display so it can be passed to Display_Controller, but keyboard
 * rendering is delegated entirely to SDL_Keyboard_View widgets.
 * The I_Display draw methods are intentional no-ops; flush() drives
 * lv_timer_handler() to push LVGL widget renders to the screen.
 */
#pragma once

// C++ Standard Libraries
#include <cstdint>
#include <string>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>

// Project Libraries
#include <overboard/hal/i_display.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL window owner implementing I_Display
 *
 * Keyboard rendering is handled by SDL_Keyboard_View widgets attached
 * to screen(). The I_Display interface is satisfied as a stub so
 * Display_Controller can hold a reference without needing a separate
 * display type for the keyboard path.
 */
class SDL_Keyboard_Display : public I_Display {
    public:
        /**
         * @brief Create LVGL keyboard window
         * @param title  Window title
         * @param width  Window width in pixels
         * @param height Window height in pixels
         */
        SDL_Keyboard_Display(const std::string& title, int width, int height);

        /**
         * Destructor
         */
        ~SDL_Keyboard_Display() override = default;

        int    width()     const override;
        int    height()    const override;

        /// @return SDL window ID for event filtering in SDL_Input
        uint32_t window_id() const;

        /// @return LVGL screen object for attaching child widgets
        lv_obj_t* screen() const;

        // ── I_Display stubs (rendering handled by LVGL_Keyboard_View) ─────────
        void clear(Color c = Color::black()) override;
        void draw_pixel(ovb::core::Point<int> pos, Color c) override;
        void draw_rect(ovb::core::Point<int> pos, ovb::core::Point<int> size, Color c, bool filled = true) override;
        void draw_text(ovb::core::Point<int> pos, const std::string& text, Color fg, Color bg, int scale = 1) override;

        /// @brief Drive lv_timer_handler() to flush widget renders to the SDL window
        void flush() override;

    private:
        int           m_width;
        int           m_height;
        lv_display_t* m_display    = nullptr;
        SDL_Window*   m_sdl_window = nullptr;
};

} // namespace ovb::hal::sdl
