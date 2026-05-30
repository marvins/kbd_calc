// Project Libraries
#include <overboard/hal/app_factory.hpp>

// Platform-specific implementations
#ifdef TARGET_SDL
    #include <overboard/hal/sdl/app.hpp>
#elif defined(TARGET_RP2350)
    #ifdef BOARD_PICOCALC
        #include <overboard/hal/picocalc/app.hpp>
    #else
        #include <overboard/hal/pico/app.hpp>
    #endif
#else
    #error "No target defined. Define TARGET_SDL or TARGET_RP2350."
#endif

namespace ovb::hal {

/*********************************/
/*      Create App Instance      */
/*********************************/
std::unique_ptr<I_App> App_Factory::create(const core::Grid_Layout& layout,
                                           const std::filesystem::path& layout_path,
                                           const std::filesystem::path& keymap_path,
                                           const std::filesystem::path& layers_path) {
#ifdef TARGET_SDL
    return sdl::SDL_App::create(layout, layout_path, keymap_path, layers_path);
#elif defined(TARGET_RP2350)
    #ifdef BOARD_PICOCALC
        return picocalc::PicoCalc_App::create(layout, layout_path, keymap_path, layers_path);
    #else
        (void)layout; (void)layout_path; (void)keymap_path; (void)layers_path;
        return std::make_unique<pico::Pico_App>(layout);
    #endif
#else
    #error "No target defined"
#endif
}

} // namespace ovb::hal
