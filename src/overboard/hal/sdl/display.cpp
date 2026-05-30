/**
 * @file    display.cpp
 * @author  Marvin Smith
 * @date    2026-05-29
 *
 * @brief   SDL display implementation — single window with LCD and keyboard
 */
#include <overboard/hal/sdl/display.hpp>

// C++ Standard Libraries
#include <stdexcept>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#include <lvgl.h>
#include <lvgl/src/drivers/sdl/lv_sdl_window.h>

// Project Libraries
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/lcd_section.hpp>
#include <overboard/hal/display_config.hpp>
#include <overboard/hal/sdl/lvgl_theme.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::hal::sdl {

/**
 * @brief PImpl (Pointer to Implementation) structure for Display
 *
 * Contains all LVGL handles and section managers. Using PImpl allows
 * changes to implementation without recompiling dependent code.
 */
struct Display::Impl {

    /// @brief SDL window handle
    SDL_Window*   sdl_window   = nullptr;

    /// @brief LVGL display handle
    lv_display_t* lv_display   = nullptr;

    /// @brief LCD section manager
    std::unique_ptr<gui::LCD_Section>    lcd;

    /// @brief Keyboard view manager
    std::unique_ptr<gui::Keyboard_View>  keyboard_view_;

    /// @brief LCD container object
    lv_obj_t*                             lcd_container  = nullptr;

    /// @brief Keyboard container object
    lv_obj_t*                             kbd_container  = nullptr;

    /// @brief Calculation engine reference
    const math::Calc_Engine&   engine;

    /// @brief Layer manager reference
    const core::Layer_Manager& layers;

    /// @brief Total window width
    int                        total_width  = 0;

    /// @brief Total window height
    int                        total_height = 0;

    /**
     * @brief Construct Impl with engine and layer references
     * @param e Reference to calculation engine
     * @param l Reference to layer manager
     */
    Impl(const math::Calc_Engine& e, const core::Layer_Manager& l)
        : engine(e), layers(l) {}
};

/***************************/
/*        Constructor        */
/*****************************/
Display::Display( const std::string&            title,
                  int                           width,
                  int                           height,
                  const ovb::core::Grid_Layout& layout,
                  const ovb::math::Calc_Engine& engine,
                  const ovb::core::Layer_Manager& layers )
    : m_impl(std::make_unique<Impl>(engine, layers))
{
    m_impl->total_width  = width;
    m_impl->total_height = height;

    // Create LVGL SDL display
    m_impl->lv_display = lv_sdl_window_create(width, height);
    if (!m_impl->lv_display) {
        throw std::runtime_error("Display: lv_sdl_window_create failed");
    }
    lv_display_set_default(m_impl->lv_display);

    // Get SDL window handle
    m_impl->sdl_window = lv_sdl_window_get_window(m_impl->lv_display);
    SDL_SetWindowTitle(m_impl->sdl_window, title.c_str());

    // Register mouse input device
    lv_sdl_mouse_create();

    // Run timer handler once to ensure window is created
    lv_timer_handler();

    // Get active screen
    lv_obj_t* screen = lv_screen_active();

    // Clear default styling
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_bg_color(screen, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);

    // Create LCD container (top section)
    m_impl->lcd_container = lv_obj_create(screen);
    lv_obj_set_size(m_impl->lcd_container, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_align(m_impl->lcd_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(m_impl->lcd_container, lvgl_color(LVGL_COLOR_BG_SCREEN), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->lcd_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->lcd_container, LV_OBJ_FLAG_SCROLLABLE);

    // Build LCD section
    m_impl->lcd = std::make_unique<gui::LCD_Section>(engine, layers);
    m_impl->lcd->build(m_impl->lcd_container);

    // Create keyboard container (bottom section)
    m_impl->kbd_container = lv_obj_create(screen);
    lv_obj_set_size(m_impl->kbd_container, KBD_WIDTH, KBD_HEIGHT);
    lv_obj_align(m_impl->kbd_container, LV_ALIGN_TOP_LEFT, 0, LCD_HEIGHT);
    lv_obj_set_style_bg_color(m_impl->kbd_container, lvgl_color(LVGL_COLOR_KBD_BG), LV_PART_MAIN);
    lv_obj_set_style_border_width(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(m_impl->kbd_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(m_impl->kbd_container, LV_OBJ_FLAG_SCROLLABLE);

    // Build keyboard view
    m_impl->keyboard_view_ = std::make_unique<gui::Keyboard_View>(
        m_impl->kbd_container, layout, layers, KBD_WIDTH, KBD_HEIGHT);
}

/******************************/
/*         Destructor         */
/******************************/
Display::~Display() = default;

/***********************************/
/*            Window ID            */
/***********************************/
uint32_t Display::window_id() const {
    return m_impl->sdl_window ? SDL_GetWindowID(m_impl->sdl_window) : 0;
}

/*****************************/
/*      Refresh Display      */
/*****************************/
void Display::refresh() {
    m_impl->lcd->refresh();
}

/*****************************/
/*      Update Layer         */
/*****************************/
void Display::update_layer() {
    m_impl->keyboard_view_->update_layer();
}

/*****************************/
/*      Render Display       */
/*****************************/
void Display::render() {
    lv_timer_handler();
}

/*******************************/
/*        Keyboard View        */
/*******************************/
gui::Keyboard_View& Display::keyboard_view() {
    return *m_impl->keyboard_view_;
}

} // namespace ovb::hal::sdl
