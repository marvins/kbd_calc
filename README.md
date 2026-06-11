# Overboard

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Macropad with on-board graphing calculator.

## Documentation

📚 **[Complete Documentation](docs/README.md)** — Architecture, input system, VIA configuration, and more.

**Quick links:**
- [Architecture & Input System](docs/architecture.md) — Text-first input architecture, event routing, VIA macropad setup
- [Custom Fonts](docs/custom_fonts.md) — Font generation for math typesetting

## Dependencies

- CMake 3.16+
- SDL2

```bash
# macOS
brew install sdl2
```

## Build

```bash
mkdir build && cd build
cmake .. -DSIMULATOR=ON
make -j$(nproc)
```

## Run

```bash
./calc_sim
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
