#pragma once

#include <overboard/hal/i_display.hpp>
#include <SDL2/SDL.h>
#include <string>

namespace ovb::hal::sdl {

/**
 * @brief SDL2 implementation of I_Display interface
 *
 * Renders to an SDL2 window using hardware-accelerated 2D rendering.
 * Supports the full I_Display API including Point-based drawing methods.
 */
class SDL_Display : public I_Display {
    public:
        /**
         * @brief Create SDL display window
         * @param title Window title bar text
         * @param pos Window position on screen
         * @param size Window dimensions (width, height)
         */
        SDL_Display( const std::string& title,
                     core::Point2i      pos,
                     core::Point2i      size );

        /// Destructor cleans up SDL window and renderer
        ~SDL_Display() override;

        /// @return Window width in pixels
        int  width()  const override;

        /// @return Window height in pixels
        int  height() const override;

        /// @copydoc I_Display::clear()
        void clear(Color c = Color::black()) override;

        /// @copydoc I_Display::draw_pixel()
        void draw_pixel(::ovb::core::Point2i pos, Color c) override;

        /// @copydoc I_Display::draw_rect()
        void draw_rect( core::Point2i pos,
                        core::Point2i size,
                        Color c,
                        bool filled = true ) override;

        /// @copydoc I_Display::draw_text()
        void draw_text( core::Point2i      pos,
                        const std::string& text,
                        Color              fg,
                        Color              bg,
                        int                scale = 1 ) override;

        /// @copydoc I_Display::flush()
        void flush() override;

        /// @return Underlying SDL window handle
        SDL_Window* window() const;

        /// @brief Bring window to front and set input focus
        void raise_to_front();

    private:
        SDL_Window*   m_window   = nullptr;   ///< SDL window handle
        SDL_Renderer* m_renderer = nullptr;   ///< SDL 2D renderer
        int           m_width;                ///< Cached window width
        int           m_height;               ///< Cached window height

        /// @brief Set SDL renderer draw color
        void set_color(Color c);
};

} // namespace ovb::hal::sdl
