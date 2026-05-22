# Architecture

This document describes the system architecture for the kbd_calc project.

## Current Structure

```
src/overboard/
├── hal/                   # Hardware Abstraction Layer
│   ├── config/            # Build configuration
│   ├── sdl/               # SDL simulator
│   │   ├── Input: sdl_input, sdl_keymap
│   │   ├── Keyboard: sdl_keyboard (window, view, display)
│   │   ├── Display: sdl_lcd_display (typeset math)
│   │   └── Theme: lvgl_theme
│   └── sk30/              # KISNT KN34 hardware
├── core/                  # Core portable logic
│   ├── Keyboard: keyboard_layout, keymap, layer_manager
│   ├── Layout: grid_layout
│   └── Types: point, rect
└── math/                  # Math engine
    ├── calc_engine        # Calculator state machine
    ├── expression         # Expression representation
    ├── parser             # String → AST
    ├── ast/               # AST nodes
    └── layout/            # Typesetting engine
```

### HAL (Hardware Abstraction Layer)
- **Purpose**: Abstract platform-specific hardware (SDL simulator, embedded devices)
- **Location**: `src/overboard/hal/`
- **Key Components**:
  - `I_App`: Platform application interface
  - `I_Display`: Display interface (pixel-buffer based)
  - `I_Input`: Input interface
  - `App_Factory`: Centralized factory — all `#ifdef` target logic lives here
  - Target config headers (`target_sdl.hpp`, `target_kn34.hpp`)

#### SDL Simulator (`hal/sdl/`)
The SDL simulator uses a **dual-LVGL-window architecture**:

| Component | Technology | Purpose |
|-----------|------------|---------|
| `SDL_Keyboard` | LVGL widgets | Virtual keyboard with button/label widgets (light theme) |
| `SDL_LCD_Display` | LVGL widgets | Calculator LCD with typeset math, history table (light theme) |

**Architecture**: Both keyboard and LCD are now LVGL-based with matching light themes:
- `SDL_Keyboard` manages the keyboard window with `lv_button` widgets
- `SDL_LCD_Display` manages the LCD window with `lv_table`, `lv_canvas` for math typesetting
- `SDL_Input` handles both mouse (hit-testing) and physical keyboard events
- `lvgl_theme.hpp` provides centralized color/font constants

**Classes**:
| Class | Responsibility |
|-------|--------------|
| `SDL_Keyboard` | High-level keyboard manager — owns window + view |
| `SDL_Keyboard_Display` | SDL window owner for keyboard |
| `SDL_Keyboard_View` | Creates `lv_button`/`lv_label` grid from `Grid_Layout` |
| `SDL_LCD_Display` | LCD window with bezel, history table, math preview canvas |
| `SDL_Input` | Mouse hit-testing + physical keyboard mapping via `SDL_Keymap` |
| `SDL_Keymap` | Maps SDL scancodes (arrows, qwe/asd/zxc, etc.) to key indices |
| `lvgl_theme.hpp` | Centralized light-mode colors and font constants |

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
│   └── kn34/
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


**Remaining**:
1. Create `src/overboard/graphics/` — extract drawing helpers from `Display_Controller`
2. Create `src/overboard/ui/` — move `Display_Controller` and add view widgets
3. Slim `src/overboard/core/` to types + keyboard only (remove any remaining LCD logic)
4. Add theme config loading — runtime theme switching from file

## LVGL Integration

### What is LVGL?
[LVGL](https://lvgl.io) is an open-source embedded GUI library providing widgets, layouts, animations, and font rendering. It is well-suited for embedded targets with small displays.

### Implemented Architecture

We use LVGL **for both keyboard and LCD displays**, replacing the legacy pixel-buffer LCD. This provides:
- Consistent light-mode theming across both displays
- Proper math typesetting via `lv_canvas` with layer-based rendering
- Unified widget framework for future enhancements

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                            │
│                                                             │
│  ┌─────────────────────┐    ┌──────────────────────────┐  │
│  │   LVGL_Keyboard     │    │    LVGL_LCD_Display      │  │
│  │   (LVGL widgets)    │    │    (LVGL widgets)        │  │
│  │                     │    │    - lv_table (history)   │  │
│  │  ┌───────────────┐  │    │    - lv_canvas (math)    │  │
│  │  │ Button Grid   │  │    │    - lv_label (indicators)│ │
│  │  └───────────────┘  │    └──────────┬───────────────┘  │
│  └──────────┬────────────┘               │                  │
│             │                            │                  │
│             ▼                            ▼                  │
│  ┌─────────────────────┐    ┌──────────────────────────┐   │
│  │  SDL Window (Kbd)   │    │  SDL Window (LCD)      │   │
│  │  LVGL SDL Driver    │    │  LVGL SDL Driver       │   │
│  └─────────────────────┘    └──────────────────────────┘   │
│                                                             │
│  SDL_Input: Mouse hit-testing + Physical keyboard mapping   │
│             via SDL_Keymap (arrows, qwe/asd/zxc, etc.)      │
└─────────────────────────────────────────────────────────────┘
```

**Key Design Decisions**:
1. **Dual LVGL windows** — Separate SDL windows for keyboard and LCD, both using LVGL
2. **Centralized theme** — `lvgl_theme.hpp` provides colors/fonts for consistent styling
3. **No forward declarations in `core/`** — Clean layer separation achieved
4. **Math typesetting** — Canvas-based rendering via `lv_draw_label` in `draw_math_to_canvas()`
5. **Hardware-accurate** — Real device has separate keyboard and LCD displays

### SDL Keyboard Mapping (`sdl_keymap`)

Physical keyboard input is mapped to calculator keys via `SDL_Keymap`:

| Physical Key | Calculator Key | Index |
|--------------|----------------|-------|
| ↑ ↓ ← → | Cursor keys | 24, 28, 27, 29 |
| q w e | Top row, first 3 keys | 0, 1, 2 |
| a s d | Second row, first 3 keys | 7, 8, 9 |
| z x c | Third row, first 3 keys | 14, 15, 16 |
| 0-9 | Digit keys | mapped to numpad positions |
| Return | Equals | 23 |
| Backspace | Backspace | 2 |
| Escape | All Clear | 4 |

**Customization**: The keymap can be modified at runtime via `SDL_Input::keymap().bind()`.

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


### Future Possibilities
- Migrate LCD to LVGL widgets (unified UI framework)
- Keep dual-path: LVGL for embedded targets, pixel-buffer for minimal SDL sim
- Add more LVGL features: animations, themes, touch gestures

## Build System

- CMake 4.0+ for build configuration
- Build script (`scripts/build.sh`) with target selection
- Targets:
  - `calc_sim`: SDL simulator (default)
  - `calc_firmware`: Embedded firmware (KN34)

## Platform Support

### SDL Simulator
- Desktop testing and development
- Uses SDL2 for graphics and input
- Target: `TARGET_SDL`

### KISNT KN34
- 30-key macropad hardware
- Embedded target (future: Raspberry Pi Pico)
- Target: `TARGET_KN34`

### Future: PicoCalc
- QWERTY keyboard calculator
- Raspberry Pi Pico-based
- Target: `TARGET_PICO` (planned)
