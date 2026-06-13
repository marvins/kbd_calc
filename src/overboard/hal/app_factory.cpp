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
#elif defined(TARGET_ZERO)
    #include <overboard/hal/pi_zero/app.hpp>
#else
    #error "No target defined. Define TARGET_SDL, TARGET_RP2350, or TARGET_ZERO."
#endif

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal {

// Module logger
static ovb::log::Stdout_Logger s_logger(ovb::log::Log_Level::Debug);

/*********************************/
/*      Create App Instance      */
/*********************************/
std::unique_ptr<I_App> App_Factory::create(const core::Grid_Layout& layout,
                                           const std::filesystem::path& layout_path) {
#ifdef TARGET_SDL
    #ifdef TARGET_PICOSDL
        s_logger.info("App_Factory: Creating PicoSDL application (simulator)");
    #else
        s_logger.info("App_Factory: Creating SDL application (simulator)");
    #endif
    return sdl::SDL_App::create(layout, layout_path);
#elif defined(TARGET_RP2350)
    #ifdef BOARD_PICOCALC
        s_logger.info("App_Factory: Creating PicoCalc application (RP2350 hardware)");
        return picocalc::PicoCalc_App::create(layout, layout_path);
    #else
        s_logger.info("App_Factory: Creating Pico application (RP2350 hardware)");
        (void)layout_path;
        return std::make_unique<pico::Pico_App>(layout);
    #endif
#elif defined(TARGET_ZERO)
    s_logger.info("App_Factory: Creating Pi Zero application (Linux embedded)");
    return pi_zero::PiZero_App::create(layout, layout_path);
#else
    #error "No target defined"
#endif
}

} // namespace ovb::hal
