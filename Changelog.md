# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


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
