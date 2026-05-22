# Architecture

This document describes the system architecture for the kbd_calc project.

## Current Structure

```
src/overboard/
├── hal/                   # Hardware Abstraction Layer
│   ├── config/            # Build-time target configuration
│   ├── sdl/               # SDL simulator implementation
│   │   ├── lvgl_keyboard.hpp        # LVGL keyboard manager (window + widgets)
│   │   ├── lvgl_keyboard_display.hpp # SDL window owner for LVGL
│   │   ├── lvgl_keyboard_view.hpp   # LVGL button widget tree
│   │   ├── sdl_display.hpp          # Pixel-buffer display (LCD)
│   │   └── sdl_input.hpp            # SDL event handling
│   └── sk30/              # Womier SK30 hardware implementation
├── core/                  # Core portable logic
│   ├── keyboard_layout.hpp     # Physical keyboard grid
│   ├── keymap.hpp         # Key codes, labels, layer definitions
│   ├── layer_manager.hpp  # Keyboard layer state
│   ├── display_controller.hpp  # LCD rendering coordinator
│   ├── point.hpp          # 2D point type
│   └── rect.hpp           # Rectangle type
└── math/                  # Math engine (separate module)
    ├── calc_engine.hpp    # Calculator state machine
    ├── expression.hpp     # Expression string representation
    ├── parser.hpp         # Expression parser
    ├── ast/               # Abstract Syntax Tree nodes
    └── layout/            # Math expression layout/rendering
```

### HAL (Hardware Abstraction Layer)
- **Purpose**: Abstract platform-specific hardware (SDL simulator, embedded devices)
- **Location**: `src/overboard/hal/`
- **Key Components**:
  - `I_App`: Platform application interface
  - `I_Display`: Display interface (pixel-buffer based)
  - `I_Input`: Input interface
  - `App_Factory`: Centralized factory — all `#ifdef` target logic lives here
  - Target config headers (`target_sdl.hpp`, `target_sk30.hpp`)

#### SDL Simulator (`hal/sdl/`)
The SDL simulator uses a dual-display architecture:

| Component | Technology | Purpose |
|-----------|------------|---------|
| `LVGL_Keyboard` | LVGL widgets | Virtual keyboard with button/label widgets |
| `SDL_Display` (LCD) | Pixel buffer (`I_Display`) | Calculator expression/history display |

**Clean Separation**: The keyboard and LCD are independent display hardware in the real device, and this is reflected in the simulator:
- `LVGL_Keyboard` manages its own LVGL window and widget tree — no inheritance from `I_Display`
- `Display_Controller` only handles the LCD via `I_Display`
- `main()` coordinates both managers independently

**Classes**:
- `LVGL_Keyboard`: High-level keyboard manager — owns window + widgets, exposes `window_id()`, `set_pressed()`, `update_layer()`
- `LVGL_Keyboard_Display`: SDL window owner for LVGL — creates SDL window, sets up LVGL display driver
- `LVGL_Keyboard_View`: LVGL widget tree — creates `lv_button`/`lv_label` grid from `Grid_Layout`
- `SDL_Display`: Pixel-buffer display implementing `I_Display` — used for LCD
- `SDL_Input`: Hit testing and event handling — uses window ID from `LVGL_Keyboard`

### Core
- **Purpose**: Platform-independent keyboard types and LCD coordination
- **Location**: `src/overboard/core/`
- **Keyboard**:
  - `Keymap`: Key codes, labels, layer definitions
  - `Layer_Manager`: Active layer state
  - `Grid_Layout`: Physical keyboard grid and cell ownership
- **LCD**:
  - `Display_Controller`: Renders LCD via `I_Display` — math expressions, history, mode indicators
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

**Completed**:
- ✅ LVGL keyboard separated from `I_Display` — `LVGL_Keyboard` is standalone in `hal/sdl/`
- ✅ `Display_Controller` slimmed — only handles LCD, no knowledge of keyboard
- ✅ No forward declarations in `core/` — clean layer separation achieved

**Remaining**:
1. Create `src/overboard/math/` — move `calc_engine`, `expression`, `parser`, `ast/`, `layout/` into it
2. Create `src/overboard/graphics/` — extract drawing helpers from `Display_Controller`
3. Create `src/overboard/ui/` — move `Display_Controller` and add view widgets
4. Slim `src/overboard/core/` to types + keyboard only
5. Update CMake and include paths

## LVGL Integration

### What is LVGL?
[LVGL](https://lvgl.io) is an open-source embedded GUI library providing widgets, layouts, animations, and font rendering. It is well-suited for embedded targets with small displays.

### Implemented Architecture

We use LVGL **only for the keyboard display**, while keeping the LCD on the existing pixel-buffer (`I_Display`) path. This reflects the real hardware where keyboard and LCD are separate display devices.

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                            │
│  ┌──────────────────┐        ┌──────────────────────────┐ │
│  │  LVGL_Keyboard   │        │   Display_Controller     │ │
│  │  (LVGL widgets)    │        │   (I_Display → LCD)      │ │
│  └────────┬───────────┘        └──────────┬───────────────┘ │
│           │                               │                 │
│           ▼                               ▼                 │
│  ┌──────────────────┐        ┌──────────────────────────┐     │
│  │  LVGL_Window     │        │   SDL_Display          │     │
│  │  + Button Grid   │        │   (pixel buffer)       │     │
│  └──────────────────┘        └──────────────────────────┘     │
└─────────────────────────────────────────────────────────────┘
```

**Key Design Decisions**:
1. **LVGL does not implement `I_Display`** — `LVGL_Keyboard` is a standalone manager, not a HAL display
2. **No forward declarations in `core/`** — `Display_Controller` knows nothing about LVGL
3. **Clean separation** — `main()` coordinates both managers; they don't know about each other
4. **Hardware-accurate** — Real device has separate keyboard and LCD displays

### LVGL Keyboard Module (`hal/sdl/`)

| Class | Responsibility |
|-------|--------------|
| `LVGL_Keyboard` | High-level manager — owns display + view, exposes control API |
| `LVGL_Keyboard_Display` | SDL window owner — creates SDL window, LVGL display driver |
| `LVGL_Keyboard_View` | Widget tree — creates/destroys `lv_button`/`lv_label` widgets |

**API**:
```cpp
class LVGL_Keyboard {
    LVGL_Keyboard(title, width, height, Grid_Layout, Layer_Manager);
    uint32_t window_id() const;   // For SDL_Input event filtering
    void set_pressed(int key);    // Highlight key
    void clear_pressed();         // Clear highlight
    void update_layer();          // Refresh labels from current layer
    void render();                // Flush LVGL → SDL window
};
```

### Git Submodule
LVGL is added as a git submodule:
```bash
git submodule add https://github.com/lvgl/lvgl.git thirdparty/lvgl
```

In `CMakeLists.txt`:
```cmake
add_subdirectory(thirdparty/lvgl)
target_link_libraries(calc_sim PRIVATE lvgl)
```

### Future Possibilities
- Migrate LCD to LVGL widgets (unified UI framework)
- Keep dual-path: LVGL for embedded targets, pixel-buffer for minimal SDL sim
- Add more LVGL features: animations, themes, touch gestures

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
