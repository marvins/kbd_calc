/**
 * @file    keylogger.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   Keylogger utility for mapping physical keys
 *
 * Captures SDL keyboard events and logs scancodes/keycodes.
 * Useful for determining key mappings on real hardware.
 */

// C++ Standard Libraries
#include <cstdio>
#include <string>

// Third-Party Libraries
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

/**
 * @brief Get human-readable name for SDL scancode
 */
static std::string scancode_name(SDL_Scancode sc) {
    const char* name = SDL_GetScancodeName(sc);
    if (name && name[0]) return std::string(name);
    return "SC_" + std::to_string(static_cast<int>(sc));
}

/**
 * @brief Main keylogger loop
 */
int main(int /*argc*/, char* /*argv*/[]) {
    ovb::log::Stdout_Logger logger(ovb::log::Log_Level::Info);

    logger.info("=== Keylogger Starting ===");
    logger.info("Press keys to see their SDL scancodes");
    logger.info("Press ESC or close window to exit");
    logger.info("===========================");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create a small window to receive keyboard events
    SDL_Window* window = SDL_CreateWindow(
        "Keylogger - Press keys to log",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 200,
        SDL_WINDOW_SHOWN);

    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN: {
                    SDL_Scancode sc = event.key.keysym.scancode;
                    SDL_Keycode key = event.key.keysym.sym;
                    Uint16 mod = event.key.keysym.mod;

                    std::string msg = "KEYDOWN: scancode=" + std::to_string(sc) +
                                      " (" + scancode_name(sc) + ")" +
                                      " keycode=" + std::to_string(key) +
                                      " mod=" + std::to_string(mod);
                    logger.info(msg);

                    // Exit on ESC
                    if (sc == SDL_SCANCODE_ESCAPE) {
                        running = false;
                    }
                    break;
                }

                case SDL_KEYUP: {
                    SDL_Scancode sc = event.key.keysym.scancode;
                    std::string msg = "KEYUP:   scancode=" + std::to_string(sc) +
                                      " (" + scancode_name(sc) + ")";
                    logger.info(msg);
                    break;
                }
            }
        }

        SDL_Delay(10);  // Small delay to prevent busy-waiting
    }

    logger.info("=== Keylogger Exiting ===");

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
