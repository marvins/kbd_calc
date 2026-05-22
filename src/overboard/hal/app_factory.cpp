// Project Libraries
#include <overboard/hal/app_factory.hpp>

// Platform-specific implementations
#ifdef TARGET_SDL
    #include <overboard/hal/sdl/sdl_app.hpp>
#elif defined(TARGET_KN34)
    #include <overboard/hal/kn34/kn34_app.hpp>
#else
    #error "No target defined. Define TARGET_SDL or TARGET_KN34."
#endif

namespace ovb::hal {

/*********************************/
/*      Create App Instance      */
/*********************************/
std::unique_ptr<I_App> App_Factory::create(const core::Grid_Layout& layout) {
#ifdef TARGET_SDL
    return sdl::SDL_App::create(layout);
#elif defined(TARGET_KN34)
    return std::make_unique<kn34::KN34_App>(layout);
#else
    #error "No target defined"
#endif
}

} // namespace ovb::hal
