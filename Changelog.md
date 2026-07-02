# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [0.10.0] - 2026-07-02

### Added
- **Stats popup** in Status page (F2): displays CPU temperature, storage usage, battery, and USB/BT status via `I_System_Info`
- **`SDL_System_Info`** implementation: Linux thermal zones, `statvfs` storage stats, mock battery/USB on macOS
- **`build_popup_overlay()`** helper on `Calculator_App`: centralises digit-key overlay construction, eliminating duplication between the action-code and `Input_Key` F-key paths
- **Custom keymap label for ANS key**: shows "Ans" in the calculator keymap overlay while retaining "Home" as the physical key label in `keyboard.json`
- **Font refactor**: replaced `lv_font_superscript.c` (Geneva 16px) with `lv_font_superscript_bold.c` (SF-Pro-Text-Bold 12px, bpp4) and `lv_font_superscript_regular.c` (Arial Unicode 12px, bpp4); added `scripts/generate_fonts.sh`

### Fixed
- **Factorial evaluation** (`8!`): `Factorial_Node::simplify()` now folds to `Number_Node` when the operand is numeric, matching the `Binary_Op_Node` constant-folding pattern; previously returned the expression string unchanged
- **Keymap overlay during popups**: `Key_Mapping_Info::apply_top_overlay()` now resets all key labels to their regular layer values before applying overlay keys (passthrough), preventing the keymap display from going blank when a popup is open

### Tests
- Added `Ast_Simplify` tests for `Factorial_Node::simplify()`: numeric fold, zero, and constant-operand truncation cases

## [0.9.0] - 2026-06-28

### Added
- **ANS key**: Home key remapped to ANS in the calculator; inserts the previous result AST at the cursor position.
- **Cursor highlighting**: the focused node in the expression canvas is drawn with a configurable background highlight color (`LVGL_COLOR_CURSOR_HIGHLIGHT`, default light blue-gray `0xD8E8F8`)
- `ui.cursor_highlight_color` setting in `settings.toml` for runtime color configuration
- `Expression::insert_node()` for pasting an arbitrary AST subtree at the cursor

### Fixed
- **Cursor left navigation**: left arrow now correctly traverses the full expression tree — every node (including fractions, functions, and other internal nodes) is a visitable cursor stop; previously the cursor would cycle within a sub-expression and never escape
- **Cursor right navigation** (committed): fixed `clone()` on `Binary_Op_Node` and `Function_Node` so ANS paste and copy operations produce correct deep copies

## [0.8.0] - 2026-06-24

### Added
- Status page: analog clock, digital clock, and solar info widgets (sunrise/sunset/solar noon)
- `Location_Provider` and `Solar_Calc` supporting the solar widget
- `Settings_Manager` API with type-safe access, dirty-state tracking, and observer notifications
- Settings app now displays all registered settings fields

### Fixed
- Calculator: `simplify()` folds integer-exact results (e.g. `2+3`, `sqrt(4)`) but keeps irrationals symbolic (e.g. `sqrt(2)/4`) until `approx()` is called
- Calculator: arrow key now correctly exits function/group nodes (e.g. `sqrt(4)→+4` works as expected)

### Tests
- 377 tests passing; added suites for `Settings_Manager`, AST constant folding, and `Number_Node::to_string()` epsilon behaviour


## [0.7.2] - 2026-06-21

### Added
- `Layout_Engine::find_node_position()` to locate AST nodes in rendered layout
- Node pointer tracking in `Layout_Box` for AST-to-layout mapping
- Test for cursor navigation from exponent to parent expression

### Changed
- Parser error handling: returns nullptr/0.0 instead of throwing on invalid input
- LVGL memory size: 128KB for RP2350 target, 4MB for SDL simulator

### Fixed
- Cursor navigation now correctly exits exponent context before adding operators to parent


## [0.7.1] - 2026-06-14

### Changed
- TH33 keyboard config cleaned up.

### Fixed
- Pi Zero: keys with `NONE` action code now route as raw `Input_Key` to `handle_input_key`, enabling F-key popups on hardware
- Calculator: switching between F-key popups now correctly closes the previous popup before opening the new one


## [0.7.0] - 2026-06-13

### Added
- TH33 keyboard configuration for EpoMaker 33-key numpad with knob and arrow keys
- `TARGET_TH33` build target for TH33 keyboard support

### Changed
- Default keyboard configuration changed from MF34 to TH33 for SDL, Pi Zero, and RP2350 targets
- App menu now uses key 27 as "Sel" (Enter key on TH33 layout)

### Fixed
- SDL virtual keyboard routing now prioritizes actions over text, ensuring EVAL/Enter works correctly in menus
- Key display labels use icons instead of text for cursor arrows and multiply operator


## [0.6.0] - 2026-06-13

### Added
- `Settings_Tree` property tree API for hierarchical settings (`get<T>("display.brightness")`, `set<T>("calculator.angle_mode")`)
- `I_Settings_Store` HAL interface for persistent settings storage
- SDL/Pi Zero implementation using host filesystem TOML storage
- Stub implementations for Pico (LittleFS) and PicoCalc (SD/FAT32)

### Changed
- **Project reorganization**: `apps/` renamed to `tools/`, new `apps/` created for user-facing applications
  - Tools: `ovt_layout_util`, `test_math_render` (development utilities)
  - Apps: `calculator/`, `status/`, `settings/` (user-facing applications)


## [0.5.0] - 2026-06-10

### Added
- `I_System_Info` HAL interface for board statistics (CPU temp, battery, storage, USB/BT)
- SDL implementation of system info (Linux thermal zones, `statvfs` for storage)
- Stub implementations for PicoCalc (no battery, RP2350 temp sensor) and Pi Zero
- Cursor navigation in expression editor (`cursor_left`, `cursor_right`, `backspace`, `delete_right`)
- `Group_Node` AST node for parenthesized expressions with proper rendering
- `Expression::set_number()` for continuing calculations from previous result
- Post-evaluation continuation - result becomes starting point for next operation

### Fixed
- Exponent layout bounds fixed.  Adding an exponent will shift the equation if it's near an edge.
- Operator insertion after grouped expressions now works correctly
- Parenthesis handling in evaluation and AST navigation

### Changed
- HAL structure now organized by platform (sdl/, pico/, picocalc/, pi_zero/)


## [0.4.0] - 2026-06-10

### Added
- Math expression typesetting on LCD canvas with layout engine
- `Font_Metrics` class for measuring glyph advances from LVGL fonts
- Unit tests for `Layout_Engine` with real LVGL font metrics
- ESCAPE key handling for standard keyboards (QWERTY) in all panels
- Error handling action item in tasks.md for future error policy design

### Changed
- MF keyboard config: removed `input_key` mappings from keys 0, 1, 2, 13 (ESCAPE, TAB, DELETE, MINUS)
  - Prevents double-dispatch of key events for QWERTY keys that also generate TEXTINPUT
  - MF macropad now uses "text-first" pass-through architecture
- Input architecture updated: text input is primary, mapped actions are secondary
- `Calculator_App::handle_input_key` now handles ESCAPE, RETURN, NUMPAD_ENTER, F1-F10
- `Status_Page` now implements `handle_input_key` for ESCAPE dismissal

### Fixed
- **Display rendering fixes** (critical):
  - Font metrics now initialized after LVGL fonts are ready (lazy initialization via `std::optional`)
  - Fixed `adv_w` interpretation: LVGL 9.5 provides advances in whole pixels, not 1/16th pixels
  - Fixed canvas clearing: changed `preview_canvas` background opacity from `LV_OPA_COVER` to `LV_OPA_TRANSP`
  - Fixed invisible text: set `label_dsc.text_local = 1` to ensure LVGL copies strings for async rendering
  - Layout engine now produces non-zero box widths for all atoms

## [0.3.0] - 2026-06-08

### Added
- `Font_Selector` for runtime font selection per target
- `Function_Menu_Popup` system (renamed from `Function_Key_Popup`) for F-key driven function menus
- `docs/README.md` with comprehensive project documentation
- `docs/math_canvas_refactor_ideas.md` with design improvement proposals

### Changed
- Renamed `Function_Key_Popup` to `Function_Menu_Popup` for clarity
- Documentation: `keyboard_architecture.md` merged into `architecture.md`

### Removed
- Legacy `data/configs/picocalc/layers.json` (migrated to unified `keyboard.json`)
- `docs/keyboard_architecture.md` (superseded by updated `architecture.md`)

## [0.2.0] - 2026-06-03

### Added
- Unified keyboard configuration format (`keyboard.json`) combining keys, positions, and layers
- Target-specific default config paths (SDL/RP2350→MF, PICOSDL/PICOCALC→picocalc)
- Embedded JSON resource support for SDL target (keyboard.json compiled into binary)
- JSON schema validation for keyboard configs (`data/schemas/keyboard.json.schema`)
- Python validation script (`scripts/validate_keyboard_config.py`) for offline config testing
- `parse_keyboard_config_string()` for parsing JSON from embedded resources

### Changed
- SDL target now only supports `keyboard.json` format (legacy VIA layout removed)
- Command-line arguments simplified: `--layout` now points to `keyboard.json` only
- Removed `--keymap` and `--layers` CLI options (no longer needed with unified format)
- `App_Factory::create()` signature simplified to `(layout, layout_path)`
- `SDL_App::create()` signature simplified to `(layout, layout_path)`
- `PicoCalc_App::create()` signature simplified to `(layout, layout_path)`
- Config class removed `keymap_path()` and `layers_path()` methods

### Fixed
- MF keyboard layout adjustments (up arrow position, key 32 alignment, vertical gaps)
- SDL build errors related to missing keyboard_window.cpp source file
- Config parsing now correctly prioritizes `keyboard.json` over legacy `main.json`

## [0.1.1] - 2026-05-30

### Added
- PICOSDL build variant - SDL simulator with 320x320 screen configuration for PicoCalc
- Render layout tool moved to `src/overboard/apps/ovt_layout_util/` with dedicated Config class for CLI parsing
- Support for empty objects `{}` as 1x1 spacers in VIA layout JSON (matching Python key mapper behavior)

### Fixed
- VIA layout parsing now correctly handles empty objects as spacers that advance the cursor
- Key positioning in render tool now matches Python key mapper's visual representation

## [0.1.0] - 2026-05-29

### Added
- Initial PicoCalc HAL implementation (`hal/picocalc/`)
  - ILI9488 SPI display driver (320×320)
  - I2C keyboard driver (STM32 controller)
  - `PicoCalc_App` lifecycle
- Renamed `hal/kn34/` to `hal/pico/` (RP2350 stub)
- Added `BOARD` selection under `TARGET_RP2350` (picocalc/pico)
- Added `TARGET_DEVICE=PICOCALC` convenience option
- Moved MF34 configs to `data/configs/mf/`
- Added PicoCalc VIA layout configs to `data/configs/picocalc/`
- Converted `display_config.hpp` to CMake template (`templates/display_config.hpp.in`)
- Per-target display dimensions (SDL: 400×800, PicoCalc: 320×320)
- Updated architecture documentation with Mermaid diagrams
- Added brace-init style for `constexpr` constants to coding standards
