# Calculator Simulator

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Simulator framework for the custom calculator module.
Two SDL2 windows: a **Keyboard** window (clickable key grid) and a **Calculator LCD** window.

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

## Architecture

```
src/
├── main_sim.cpp                  # Simulator entry point
├── hal/
│   ├── i_display.hpp             # I_Display interface
│   ├── i_input.hpp               # I_Input interface
│   └── sdl/
│       ├── sdl_display.hpp/.cpp  # SDL2 display HAL
│       └── sdl_input.hpp/.cpp    # SDL2 mouse input HAL
└── core/
    ├── keymap.hpp/.cpp           # Layered keycode table
    ├── layer_manager.hpp/.cpp    # Active layer state
    ├── calc_engine.hpp/.cpp      # Expression evaluator (portable)
    └── display_controller.hpp/.cpp # Renders keyboard + LCD
```

## Layers

| Layer | Name        | Contents                        |
|-------|-------------|---------------------------------|
| 0     | Basic       | Standard calculator             |
| 1     | Scientific  | Trig, log, powers, constants    |
| 2     | Programmer  | Hex, bitwise, shift ops         |

Cycle layers with the **▶** key (bottom-right of each layer).

See **[docs/layers.md](docs/layers.md)** for full HTML key tables and key code reference.
