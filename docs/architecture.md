# Architecture

This document describes the system architecture for the kbd_calc project.

## Current Structure

```
src/overboard/
├── hal/                   # Hardware Abstraction Layer
│   ├── config/            # Build-time target configuration
│   ├── sdl/               # SDL simulator implementation
│   └── sk30/              # Womier SK30 hardware implementation
└── core/                  # Core portable logic (currently mixed concerns)
    ├── ast/               # Abstract Syntax Tree nodes
    ├── layout/            # Math expression layout/rendering engine
    ├── calc_engine.hpp    # Calculator state machine and key handling
    ├── expression.hpp     # Expression string representation
    ├── parser.hpp         # Expression parser (string → AST)
    ├── display_controller.hpp  # UI rendering logic
    ├── keyboard_layout.hpp     # Physical keyboard grid
    ├── keymap.hpp         # Key codes, labels, and layer definitions
    ├── layer_manager.hpp  # Keyboard layer state
    ├── point.hpp          # 2D point type
    └── rect.hpp           # Rectangle type
```

### HAL (Hardware Abstraction Layer)
- **Purpose**: Abstract platform-specific hardware (SDL simulator, embedded devices)
- **Location**: `src/overboard/hal/`
- **Key Components**:
  - `I_App`: Platform application interface
  - `I_Display`: Display interface
  - `I_Input`: Input interface
  - `App_Factory`: Centralized factory — all `#ifdef` target logic lives here
  - Target config headers (`target_sdl.hpp`, `target_sk30.hpp`)

### Core (Currently Mixed)
- **Purpose**: Platform-independent logic — currently mixes concerns
- **Location**: `src/overboard/core/`
- **Math Engine**:
  - `Calc_Engine`: Calculator state machine, key dispatch, history
  - `Expression`: Expression string/cursor representation
  - `Parser`: Recursive-descent parser, string → AST
  - `ast/`: AST node types and tree operations
  - `layout/`: Math layout engine (box model for expression rendering)
- **UI Rendering**:
  - `Display_Controller`: Keyboard and LCD rendering
- **Keyboard**:
  - `Keymap`: Key codes, labels, layer definitions
  - `Layer_Manager`: Active layer state
  - `Grid_Layout`: Physical keyboard grid and cell ownership
- **Shared Types**:
  - `point.hpp`, `rect.hpp`

## Proposed Refactoring (Epsilon-Inspired)

### Epsilon Architecture Lessons
From the NumWorks Epsilon project:
- **ion/** = HAL (platform abstraction)
- **kandinsky/** = Graphics library (low-level drawing)
- **escher/** = UI framework (high-level widgets)
- **poincare/** = Math engine (pure logic, no UI)
- Clear layer separation with no circular dependencies
- Math engine completely independent of UI

### Proposed Module Structure

```
src/overboard/
├── hal/                   # Hardware Abstraction Layer (ion equivalent) — no change
│   ├── config/
│   ├── sdl/
│   └── sk30/
├── core/                  # Shared types and keyboard logic only
│   ├── point.hpp
│   ├── rect.hpp
│   ├── keymap.hpp
│   ├── layer_manager.hpp
│   └── keyboard_layout.hpp
├── math/                  # Math engine (poincare equivalent) — pure logic, no UI
│   ├── calc_engine.hpp
│   ├── expression.hpp
│   ├── parser.hpp
│   ├── ast/               # AST node types
│   ├── layout/            # Math expression layout (box model)
│   └── cas/               # Computer Algebra System (future)
├── graphics/              # Low-level drawing primitives (kandinsky equivalent)
│   ├── color.hpp
│   ├── font.hpp
│   └── drawing.hpp
└── ui/                    # UI framework (escher equivalent)
    ├── display_controller.hpp
    ├── keyboard_view.hpp
    ├── lcd_view.hpp
    └── widgets/
```

### Module Responsibilities

#### `hal/` — Hardware Abstraction Layer
- Platform-specific hardware abstraction (unchanged)
- I_App, I_Display, I_Input interfaces
- App_Factory: all `#ifdef` target logic lives here
- **No dependencies on other overboard modules**

#### `core/` — Shared Types & Keyboard
- Platform-independent types: `Point`, `Rect`
- Keyboard logic: `Keymap`, `Layer_Manager`, `Grid_Layout`
- **No rendering, no math dependencies**

#### `math/` — Math Engine
- `Calc_Engine`: state machine for key handling and history
- `Expression`: string/cursor representation
- `Parser`: recursive-descent parser (string → AST)
- `ast/`: AST node types and evaluation
- `layout/`: Box model engine for rendering math expressions
- `cas/`: Computer Algebra System (symbolic math, future)
- **No UI or graphics dependencies — can be unit-tested in isolation**

#### `graphics/` — Drawing Primitives
- Low-level drawing: colors, fonts, shapes
- Platform-agnostic drawing API backed by `I_Display`
- **No UI widget logic, no math logic**

#### `ui/` — UI Framework
- `Display_Controller`: coordinates keyboard and LCD views
- `Keyboard_View`: renders the keyboard layout widget
- `LCD_View`: renders the calculator expression display
- Depends on `graphics/` and `core/keyboard`
- **No math logic, no HAL dependencies**

### Dependency Graph
```
hal (no deps)
core (no deps)
math → core
graphics → core
ui → graphics, core
main → hal, ui, math
```

### Migration Path
1. **[Next]** Create `src/overboard/math/` — move `calc_engine`, `expression`, `parser`, `ast/`, `layout/` into it
2. Create `src/overboard/graphics/` — extract drawing helpers from `Display_Controller`
3. Create `src/overboard/ui/` — move `Display_Controller` and add view widgets
4. Slim `src/overboard/core/` to types + keyboard only
5. Update CMake and include paths

## LVGL Integration

### What is LVGL?
[LVGL](https://lvgl.io) is an open-source embedded GUI library providing widgets, layouts, animations, and font rendering. It is well-suited for embedded targets with small displays.

### Integration Options

#### Option A: LVGL as UI Backend (Recommended)
- Use LVGL as the rendering backend inside the `ui/` module
- `I_Display` wraps an LVGL canvas or display driver
- LVGL handles widget rendering, fonts, and layout
- Our `ui/` layer uses LVGL widgets instead of manual drawing
- `math/layout/` still owns expression layout logic; renders via LVGL draw calls

#### Option B: LVGL Only for LCD Display
- Use LVGL specifically for the LCD calculator display
- Keep the keyboard display rendering with our own drawing primitives
- Easier migration path — scope-limited

#### Option C: LVGL as HAL Display Driver
- Wrap LVGL's display driver API behind `I_Display`
- LVGL sits below the `ui/` module, above the hardware
- Cleanest layer separation but more plumbing

### Recommended Integration Strategy
**Option A** is the closest match to the Epsilon pattern (where kandinsky provides drawing and escher provides widgets on top of it). LVGL would replace `graphics/` + parts of `ui/`, and our `math/layout/` engine would produce draw commands that LVGL executes.

### Git Submodule Approach
LVGL is best added as a git submodule:
```bash
git submodule add https://github.com/lvgl/lvgl.git extern/lvgl
```
Then in CMake:
```cmake
add_subdirectory(extern/lvgl)
target_link_libraries(calc_sim PRIVATE lvgl)
```
This keeps LVGL pinned to a specific version and avoids vendoring the source directly.

### Pending Decision
- Confirm integration option (A, B, or C)
- Decide LVGL version (v9.x recommended for new projects)
- Add submodule and CMake wiring
- Implement `I_Display` → LVGL driver bridge

## Build System

- CMake 4.0+ for build configuration
- Build script (`scripts/build.sh`) with target selection
- Targets:
  - `calc_sim`: SDL simulator (default)
  - `calc_firmware`: Embedded firmware (SK30)

## Platform Support

### SDL Simulator
- Desktop testing and development
- Uses SDL2 for graphics and input
- Target: `TARGET_SDL`

### Womier SK30
- 30-key macropad hardware
- Embedded target (future: Raspberry Pi Pico)
- Target: `TARGET_SK30`

### Future: PicoCalc
- QWERTY keyboard calculator
- Raspberry Pi Pico-based
- Target: `TARGET_PICO` (planned)
