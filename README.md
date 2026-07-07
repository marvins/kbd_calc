# Overboard

[![Build Status](https://github.com/Terminus-Geospatial/overboard/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/Terminus-Geospatial/overboard/actions/workflows/build-and-test.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Macropad with on-board graphing calculator.

## Documentation

📚 **[Complete Documentation](docs/README.md)** — Architecture, input system, VIA configuration, and more.

**Quick links:**
- [Architecture & Input System](docs/architecture.md) — Text-first input architecture, event routing, VIA macropad setup
- [Custom Fonts](docs/custom_fonts.md) — Font generation for math typesetting

## Dependencies

- CMake 4.0+
- SDL2
- (Optional) Emscripten SDK for the WASM target

```bash
# macOS
brew install sdl2

# Emscripten (for WASM)
# Follow https://emscripten.org/docs/getting_started/downloads.html, then run:
# ./emsdk install latest && ./emsdk activate latest
```

## Build

### Native SDL simulator

```bash
mkdir build && cd build
cmake .. -DSIMULATOR=ON
make -j$(nproc)
```

### WASM/Emscripten

```bash
# Activate Emscripten SDK in this shell
source /path/to/emsdk/emsdk_env.sh

mkdir build-wasm && cd build-wasm
emcmake cmake .. -DTARGET_DEVICE=WASM
emmake make -j$(nproc)

# Launch in browser
emrun calc_sim.html
```

## Run

```bash
./calc_sim
```

For WASM:

```bash
./go_macropad.sh --wasm
```

## Testing

Build and run the unit-test suite:

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
./calc_tests
```

Filter to specific test suites (e.g., layout tests only):

```bash
./calc_tests --gtest_filter="LayoutTest*"
```

---

## Roadmap

See **[docs/tasks.md](docs/tasks.md)** for current development phases, active tasks, and future feature backlog.
