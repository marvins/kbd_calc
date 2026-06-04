# Unified Architecture Refactor: Expression & Display

This document outlines the comprehensive refactor to transition from token-based expression handling to a unified tree-based architecture with a modular display system, following Epsilon's architecture pattern.

## Overview

The refactor consists of two major components:
1. **Expression Refactor** - Transition from token-based to tree-based (AST) representation
2. **Display Module Refactor** - Build a modular, extensible display system from scratch

## Core Principles

1. **Single Source of Truth** - AST is the only representation of the expression
2. **Interface-Based Design** - Use abstract interfaces for extensibility
3. **Separation of Concerns** - Expression (data) vs Display (presentation)
4. **Platform Agnosticism** - Core logic independent of rendering backend
5. **Testability** - Components can be unit tested in isolation
6. **Type Safety** - Use traits and template metaprogramming for compile-time type checking

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         Application Layer                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐ │
│  │   Input      │─────►│  Expression  │─────►│ Calc_Engine  │ │
│  │   Layer      │      │   (AST)      │      │              │ │
│  └──────────────┘      └──────┬───────┘      └──────┬───────┘ │
│                              │                     │          │
│                              ▼                     ▼          │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                  Display Module                          │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │  ┌──────────────┐      ┌──────────────┐              │ │
│  │  │   Display    │◄─────│   Layout     │              │ │
│  │  │   Manager    │      │   Engine     │              │ │
│  │  └──────┬───────┘      └──────┬───────┘              │ │
│  │         │                     │                          │ │
│  │         ▼                     ▼                          │ │
│  │  ┌──────────────┐      ┌──────────────┐              │ │
│  │  │   Renderer   │─────►│   Canvas     │              │ │
│  │  │   Interface  │      │   Abstraction│              │ │
│  │  └──────────────┘      └──────┬───────┘              │ │
│  │                               │                          │ │
│  │                               ▼                          │ │
│  │  ┌──────────────┐                                      │ │
│  │  │   Platform    │                                      │ │
│  │  │   Renderer    │                                      │ │
│  │  └──────────────┘                                      │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Part 1: UI Refactor

### Overview

Restructure the application UI to support multiple panels, a platform-aware chrome (header/footer bars), and optional keyboard display. The architecture centers on a platform-neutral panel system with per-platform chrome decoration.

```
┌─────────────────────────────────────────────────────┐
│                    Main Window                       │
├────────── Header Bar (PicoCalc only) ───────────────┤
│  [ App Name / Time / Battery / Future Indicators ]  │
├─────────────────────────────────────────────────────┤
│                                                     │
│               Active Panel                          │
│   ┌─────────────────────────────────────────────┐   │
│   │  Status Page  |  Calculator  |  App Menu   │   │
│   └─────────────────────────────────────────────┘   │
│                                                     │
│         (Optional: Keyboard Display)                │
│         SDL only                                    │
│                                                     │
├────────── Footer Bar (PicoCalc only) ───────────────┤
│   [ F1 ]  [ F2 ]  [ F3 ]  [ F4 ]  [ F5 ]          │
└─────────────────────────────────────────────────────┘
```

### Phase 1: Optional Keyboard UI

Goal: Make the keyboard display a build/runtime option. When enabled (SDL only), buttons are fully interactive and reflect the active layer. SDL also accepts normal keyboard input simultaneously, giving two input sources.

#### 1a: Build-time optionality

- [x] Add `SHOW_KEYBOARD_UI` compile-time flag (SDL only, default ON)
- [x] Remove keyboard display from PicoSDL, RP2350, and PICOCALC builds
- [x] Guard `Keyboard_Display` instantiation behind the flag in SDL app
- [x] Check coding standard compliance

#### 1b: Interactive buttons

- [x] Make each key widget clickable (`LV_OBJ_FLAG_CLICKABLE`) and attach an `lv_event_cb` for `LV_EVENT_CLICKED`
- [x] On click, synthesize a `Key_Event` (press + release) into the shared `SDL_Input` event queue — same path as a physical keypress
- [x] Visually highlight the button on press and restore on release (style state `LV_STATE_PRESSED`)
- [x] Check coding standard compliance

#### 1c: Layer-aware display

- [x] Subscribe `Keyboard_Display` to layer-change notifications from `Layer_Manager`
- [x] On layer change, call `update_layer()` to refresh all button labels and fonts
- [ ] Ensure modifier keys (Shift, Fn, etc.) visually toggle their active state
- [ ] Check coding standard compliance

#### 1d: Dual SDL input sources

- [x] SDL physical keyboard events continue to map through `SDL_Input` → `Input_Key` → `SDL_Keymap` as before
- [x] On-screen button clicks generate identical `Key_Event` entries through the same queue — no special-casing in consumers
- [x] Verify both sources produce identical `Action_Code` output for the same logical key
- [ ] Add integration test: click button and press physical key for same action, assert same event emitted
- [ ] Check coding standard compliance

### Phase 2: Panel System

Goal: Replace the single-view app with a panel-based system that supports switching between panels.

- [x] Define `I_Panel` interface (activate, deactivate, handle_input, render, name)
- [x] Implement `Panel_Manager` — owns a stack of panels, routes input/render calls
- [x] Implement `Status_Page` panel — shown on boot, displays system info
- [x] Implement `Calculator_App` panel — current expression/result/history work goes here
- [ ] Implement `App_Menu` panel — lists available panels, allows switching
- [ ] Wire panel switching: App_Menu selects → Panel_Manager pushes new panel
- [x] Set `Status_Page` as the default panel on boot
- [ ] Add unit tests for Panel_Manager (push/pop/route)
- [ ] Check coding standard compliance

### Phase 3: PicoCalc Chrome (Header & Footer Bars)

Goal: PicoCalc-specific window decorations that do not appear on SDL/RP2350.

- [x] Define `I_Header_Bar` interface — `set_app_name()`, `render()`
- [x] Implement `Header_Bar` — shows current panel name
- [x] Define `I_Footer_Bar` interface — `set_label(slot, text)`, `render()`, 5 slots (F1–F5)
- [x] Implement `Footer_Bar` — 5 labelled, clickable function-key slots
- [x] Wire header bar to panel name; footer F1 navigates Status ↔ Calculator
- [ ] Guard both bars behind `TARGET_DEVICE == PICOCALC` in CMake (deferred — useful on SDL too)
- [ ] Check coding standard compliance

### Phase 4: Function-Key Popup System (PicoCalc)

Goal: Pressing F1–F5 on the PicoCalc shows a popup menu anchored to that footer slot.

- [ ] Define `I_Popup` interface — `show()`, `hide()`, `handle_input()`, `render()`
- [ ] Implement `Function_Key_Popup` — generic popup with scrollable item list
- [ ] Map F1–F5 `Action_Code` values to footer slots in `Picocalc_Footer_Bar`
- [ ] On F-key press: instantiate and show the matching popup via `Panel_Manager`
- [ ] Popup dismissal: any non-F-key press or second press of same F-key closes popup
- [ ] Placeholder slot content (items TBD per app)
- [ ] Add unit tests for popup show/hide/dismiss lifecycle
- [ ] Check coding standard compliance

### Phase 5: Integration & Testing

- [ ] Boot flow: `Status_Page` shown → user presses key → navigates to `Calculator_App`
- [ ] App menu accessible from any panel via a designated key (TBD)
- [ ] Keyboard UI visible only in SDL builds
- [ ] Header bar updates when active panel changes
- [ ] Footer bar labels update per active panel context
- [ ] End-to-end test: boot → status → menu → calculator → F1 popup → dismiss
- [ ] Check coding standard compliance

## Part 2: Display Module Refactor


### Phase 3: Platform Renderers

- [ ] Implement LVGL renderer
- [ ] Implement SDL renderer (for testing)
- [ ] Implement LVGL canvas
- [ ] Implement SDL canvas (for testing)
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for platform renderers
- [ ] Check coding standard compliance for renderer implementations

### Phase 4: Display Integration

- [ ] Integrate display manager with Expression (AST input)
- [ ] Integrate display manager with Calc_Engine (result input)
- [ ] Integrate cursor renderer with Expression cursor path
- [ ] Implement display refresh/update mechanism
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for integration points
- [ ] Check coding standard compliance for integration code

### Phase 5: Display Testing

- [ ] Test layout engine with various AST structures
- [ ] Test renderer with mock canvas
- [ ] Test cursor position calculation
- [ ] Test dirty region tracking
- [ ] Test full rendering pipeline
- [ ] Test display refresh with expression changes
- [ ] Visual tests: render sample expressions to images
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for display testing
- [ ] Check coding standard compliance for test code

### Phase 6: Display Cleanup

- [ ] Remove old display code (LCD_Section, math_canvas)
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for cleanup verification
- [ ] Check coding standard compliance for cleanup changes

## Part 3: Unified Integration

### Phase 1: System Integration

- [ ] Update Calc_Engine to work with tree-based Expression
- [ ] Update input layer to work with tree-based Expression
- [ ] Connect all components in main application loop
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for system integration
- [ ] Check coding standard compliance for integration code

### Phase 2: End-to-End Testing

- [ ] Test complete workflow: input → expression → evaluation → display
- [ ] Test complex expressions (nested functions, multiple operators)
- [ ] Test cursor movement through complex expressions
- [ ] Test history management
- [ ] Test result display timing
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for end-to-end testing
- [ ] Check coding standard compliance for test code

### Phase 3: Performance Optimization

- [ ] Implement layout caching for unchanged expressions
- [ ] Optimize dirty region tracking
- [ ] Implement lazy evaluation for layout
- [ ] Add object pooling for layout boxes
- [ ] Implement double buffering if needed
- [ ] Remove unused and unnecessary code from this phase
- [ ] Add unit tests for performance optimizations
- [ ] Check coding standard compliance for optimization code


## Type Safety with Traits and Template Metaprogramming

### Node Type Traits

Define compile-time traits for AST node types to enable type-safe operations:

```cpp
// Base trait for all node types
template<typename Node_Type>
struct Node_Traits {
    static constexpr bool is_leaf = false;
    static constexpr bool is_operator = false;
    static constexpr bool is_function = false;
    static constexpr size_t child_count = 0;
};

// Specializations for specific node types
template<>
struct Node_Traits<Number_Node> {
    static constexpr bool is_leaf = true;
    static constexpr bool is_operator = false;
    static constexpr bool is_function = false;
    static constexpr size_t child_count = 0;
};

template<>
struct Node_Traits<Binary_Op_Node> {
    static constexpr bool is_leaf = false;
    static constexpr bool is_operator = true;
    static constexpr bool is_function = false;
    static constexpr size_t child_count = 2;
};

template<>
struct Node_Traits<Unary_Op_Node> {
    static constexpr bool is_leaf = false;
    static constexpr bool is_operator = true;
    static constexpr bool is_function = false;
    static constexpr size_t child_count = 1;
};

template<>
struct Node_Traits<Function_Node> {
    static constexpr bool is_leaf = false;
    static constexpr bool is_operator = false;
    static constexpr bool is_function = true;
    static constexpr size_t child_count = std::dynamic_extent; // Variable
};
```

### Compile-Time Type Checking

Use concepts (C++20) for compile-time type checking:

```cpp
template<typename T>
concept Leaf_Node = Node_Traits<T>::is_leaf;

template<typename T>
concept Operator_Node = Node_Traits<T>::is_operator;

template<typename T>
concept Function_Node = Node_Traits<T>::is_function;

template<typename T>
concept Has_Fixed_Children = Node_Traits<T>::child_count != std::dynamic_extent;
```

### Type-Safe Tree Operations

Use template metaprogramming to ensure type-safe tree operations:

```cpp
// Type-safe child access with compile-time bounds checking
template<typename Node_Type>
auto get_child_safe(Node_Type& node, size_t index) {
    static_assert(Node_Traits<Node_Type>::child_count != 0,
                  "Node type has no children");
    if constexpr (Node_Traits<Node_Type>::child_count != std::dynamic_extent) {
        static_assert(index < Node_Traits<Node_Type>::child_count,
                      "Child index out of bounds at compile time");
    }
    return node.child_at(index);
}
```

### Cursor Path Type Safety

Define typed cursor paths instead of raw vectors:

```cpp
template<typename... Indices>
struct Cursor_Path {
    std::array<size_t, sizeof...(Indices)> path;

    constexpr Cursor_Path(Indices... indices) : path{indices...} {}

    constexpr size_t size() const { return path.size(); }
    constexpr size_t operator[](size_t i) const { return path[i]; }
};

// Type-safe path construction
using Path = Cursor_Path<0, 1>; // Navigate to child 0, then child 1
```

### Display Type Traits

Define traits for display components:

```cpp
template<typename Renderer>
struct Renderer_Traits {
    static constexpr bool supports_animation = false;
    static constexpr bool supports_themes = false;
    static constexpr bool supports_export = false;
};

template<>
struct Renderer_Traits<LVGL_Renderer> {
    static constexpr bool supports_animation = true;
    static constexpr bool supports_themes = true;
    static constexpr bool supports_export = false;
};
```

### Type-Safe Layout Operations

Use template metaprogramming for layout constraints:

```cpp
template<typename Layout_Type>
struct Layout_Constraints {
    static constexpr bool can_wrap = false;
    static constexpr bool can_scale = false;
    static constexpr bool min_width = 0;
    static constexpr bool max_width = std::numeric_limits<int>::max();
};

template<>
struct Layout_Constraints<Text_Layout> {
    static constexpr bool can_wrap = true;
    static constexpr bool can_scale = false;
    static constexpr int min_width = 10;
};
```

## Type Safety Implementation Checklist

- [ ] Add ATAN2 action code for two-argument arctangent
- [ ] Define Node_Traits template for all AST node types
- [ ] Implement compile-time concepts for node categories (Leaf, Operator, Function)
- [ ] Add type-safe child access functions with compile-time bounds checking
- [ ] Design typed Cursor_Path instead of raw vector
- [ ] Define Renderer_Traits for different renderer implementations
- [ ] Define Layout_Constraints for different layout types
- [ ] Implement type-safe layout operations using template metaprogramming
- [ ] Add static_assert checks for critical invariants
- [ ] Use constexpr where possible for compile-time evaluation
- [ ] Add type traits for display components (Canvas, Renderer, Layout)

## Architecture Details

### Expression (Data Layer)

**Current Design (Token-based):**
- Expression holds vector of Token objects
- Cursor is index in token vector
- AST built from tokens via string parsing
- Simple linear structure

**Target Design (Tree-based):**
- Expression holds AST root directly
- Cursor is path through tree (vector of child indices)
- AST is single source of truth
- Hierarchical structure like Epsilon

**Key Challenges:**
1. **Tree navigation** - Need to expose children from AST nodes
2. **Cursor tracking** - Path-based instead of index-based
3. **Partial editing** - Character-by-character number entry in tree
4. **Tree manipulation** - Insert/delete operations on hierarchical structure

### Display Module (Presentation Layer)

**Responsibilities:**
- Render mathematical expressions from AST
- Display cursor position
- Show evaluation results
- Maintain expression history
- Handle display refresh and updates
- Manage canvas/buffer lifecycle

**What Display Does NOT Do:**
- Expression evaluation (handled by Calc_Engine)
- Expression editing (handled by Expression)
- Input handling (handled by input layer)
- Platform-specific rendering (handled by HAL)

**Component Interfaces:**

```cpp
// Display Manager - Orchestrates display operations
class I_Display_Manager {
    public:
        virtual void render_expression( const ast::Node::ptr_t& ast ) = 0;
        virtual void render_cursor( const std::vector<size_t>& path ) = 0;
        virtual void render_result( const std::string& result ) = 0;
        virtual void add_to_history( const ast::Node::ptr_t& ast,
                                     const std::string& result ) = 0;
        virtual void clear() = 0;
        virtual void refresh() = 0;
};

// Layout Engine - Converts AST to layout boxes
class I_Layout_Engine {
    public:
        virtual Layout_Box::ptr_t build_layout( const ast::Node::ptr_t& ast ) = 0;
        virtual Size measure( const Layout_Box::ptr_t& layout ) = 0;
        virtual void position( Layout_Box::ptr_t& layout, const Rect& bounds ) = 0;
};

// Renderer - Draws layout boxes to canvas
class I_Renderer {
    public:
        virtual void draw_box( const Layout_Box::ptr_t& box, I_Canvas& canvas ) = 0;
        virtual void draw_cursor( const Point& position, I_Canvas& canvas ) = 0;
        virtual void draw_text( const std::string& text, const Point& position,
                             I_Canvas& canvas ) = 0;
        virtual void draw_line( const Point& start, const Point& end,
                             I_Canvas& canvas ) = 0;
};

// Canvas - Platform-agnostic drawing surface
class I_Canvas {
    public:
        virtual Size get_size() const = 0;
        virtual void clear() = 0;
        virtual void set_pixel( const Point& pos, Color color ) = 0;
        virtual void fill_rect( const Rect& rect, Color color ) = 0;
        virtual void invalidate() = 0;
};
```

### File Structure

```
src/overboard/
├── math/
│   ├── expression.hpp/cpp          # Tree-based Expression
│   └── ast/
│       ├── node.hpp                 # Extended with child_count(), child_at()
│       ├── binary_op_node.hpp/cpp   # Implements child methods
│       ├── unary_op_node.hpp/cpp    # Implements child methods
│       └── function_node.hpp/cpp   # Implements child methods
└── display/
    ├── display_manager.hpp/cpp
    ├── layout_engine.hpp/cpp
    ├── renderer/
    │   ├── i_renderer.hpp
    │   ├── lvgl_renderer.hpp/cpp
    │   └── sdl_renderer.hpp/cpp
    ├── canvas/
    │   ├── i_canvas.hpp
    │   ├── lvgl_canvas.hpp/cpp
    │   └── sdl_canvas.hpp/cpp
    ├── cursor_renderer.hpp/cpp
    ├── result_display.hpp/cpp
    ├── history_display.hpp/cpp
    └── types.hpp
```

## Epsilon Reference

- Epsilon uses pool-based memory management (optional for us)
- Tree is the single source of truth
- Direct tree manipulation for editing
- No intermediate token representation
- Clear separation between math engine and display

## Future Extensibility

### Expression Enhancements
- CAS operations (simplification, differentiation)
- Symbolic computation
- Pattern matching

### Display Enhancements
- Animation support (smooth cursor transitions)
- Themes (customizable colors and fonts)
- Zoom/Pan (for large expressions)
- Export (save as images/PDF)
- Accessibility (screen reader support)
- Multi-line display (for complex expressions)

### Extension Points
- Custom renderers for new platforms
- Custom layout engines for different notations
- Custom cursor styles
- Custom result display formats
