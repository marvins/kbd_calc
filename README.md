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
| 1     | Trig        | Trig and inverse trig functions |
| 2     | Constants   | π, e, φ, τ                      |
| 3     | Programmer  | Hex, bitwise, shift ops         |

Cycle layers with the **Trig** / **CST** keys. Return with **BACK**.

See **[docs/layers.md](docs/layers.md)** for full HTML key tables and key code reference.

---

## TODO

- [ ] **Calculation history** — record each evaluated expression and its result in a scrollable history list on the LCD
- [ ] **Export history** — button to export the session history as a Python script (each entry as a commented expression + assignment) or a LaTeX document (formatted as an `align` environment)
- [ ] **Symbol layer** — dedicated layer for inserting named symbols: Greek alphabet (α β γ δ … ω), uppercase variants, and Latin letter variables (a–z)
- [ ] **Math-view rendering** — typeset display of evaluated expressions using the AST, shown after `=` is pressed (TI-Nspire style)
  - [ ] `Math_Box` layout model — width, height, baseline offset per node; layout pass separate from paint pass
  - [ ] Phase 1: numbers, binary `+`/`−`, fractions (`/` → stacked numerator/denominator with rule)
  - [ ] Phase 2: `^` superscript (recursive scale parameter), `sqrt` with dynamic vinculum
  - [ ] Phase 3: functions (`sin`, `cos`, etc.) with parenthesised arguments
  - [ ] Hook `Math_Renderer` into `Display_Controller::render_lcd()` post-equals, using `last_ast`
- [ ] **Exact / approximate mode** — NSpire-style: all evaluation is symbolic (`simplify()`); pressing `≈` wraps the expression in `approx(…)` which force-evaluates to a decimal; `Node::simplify()` / `Node::clone()` scaffolding in place
  - [ ] `Rational` type for exact integer arithmetic (`1/3 + 1/6 = 1/2`)
  - [ ] Symbolic radicals — keep `sqrt(2)` unevaluated unless inside `approx()`
  - [ ] Variable binding — symbol table in `Calc_State`, variable tokens via the symbol layer
  - [ ] Differentiation — recursive AST transform `Node::differentiate(var)`
