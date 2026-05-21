// Project Libraries
#include <overboard/hal/app_factory.hpp>

// Platform-specific implementations
#ifdef TARGET_SDL
    #include <overboard/hal/sdl/sdl_app.hpp>
#elif defined(TARGET_SK30)
    #include <overboard/hal/sk30/sk30_app.hpp>
#else
    #error "No target defined. Define TARGET_SDL or TARGET_SK30."
#endif

namespace ovb::hal {

/*********************************/
/*      Create App Instance      */
/*********************************/
std::unique_ptr<I_App> App_Factory::create(const core::Grid_Layout& layout) {
#ifdef TARGET_SDL
    return std::make_unique<sdl::SDL_App>(layout);
#elif defined(TARGET_SK30)
    return std::make_unique<sk30::SK30_App>(layout);
#else
    #error "No target defined"
#endif
}

} // namespace ovb::hal
