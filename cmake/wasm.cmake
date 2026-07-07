# Toolchain file for Emscripten/WASM builds
# Usage: emcmake cmake -B build-wasm -DCMAKE_TOOLCHAIN_FILE=cmake/wasm.cmake

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR wasm32)

set(CMAKE_C_COMPILER   emcc)
set(CMAKE_CXX_COMPILER em++)
set(CMAKE_AR           emar)
set(CMAKE_RANLIB       emranlib)
set(CMAKE_LINKER       em++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Emscripten-specific settings
set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_C_FLAGS_INIT   "-sUSE_SDL=2")
set(CMAKE_CXX_FLAGS_INIT "-sUSE_SDL=2")
