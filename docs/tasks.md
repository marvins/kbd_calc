# Project Tasks

## Phase 1: Calculator — Basic Algebra  ← **NEXT**

Goal: Get the calculator working end-to-end for basic arithmetic and algebra.

- [ ] **Error handling strategy** — currently `draw_math_to_canvas` silently swallows render exceptions to `std::cerr`. Define a proper policy
- [ ] Digits, operators (`+`, `-`, `*`, `/`), parentheses all route correctly through `Calculator_App`
- [x] `EVAL` key evaluates the expression and displays result
- [ ] `BACKSPACE` deletes last token
- [ ] Cursor keys (`←`, `→`, `↑`, `↓`) move through expression
- [ ] Negate (`+/-`) works on the current token
- [ ] `DECIMAL` inserts `.` correctly
- [ ] Result history renders below active expression
- [ ] Clear / reset works
- [ ] Add unit tests for common expression patterns (arithmetic, nested parens)

---

## Phase 2: Function-Key Popup System

Goal: F1–F5 on PicoCalc shows a popup anchored to the footer slot.

- [ ] Define `I_Popup` interface — `show()`, `hide()`, `handle_input()`, `render()`
- [ ] Implement `Function_Key_Popup` — generic popup with scrollable item list
- [ ] Map F1–F5 `Action_Code` values to footer slots
- [ ] On F-key press: show matching popup via `Panel_Manager`
- [ ] Popup dismissal: any non-F-key press or second press of same F-key closes it
- [ ] Add unit tests for popup lifecycle

---

## Phase 3: PicoCalc Migration to keyboard.json

Goal: Migrate picocalc config from legacy VIA format to unified `keyboard.json`.

- [ ] Create unified `keyboard.json` for picocalc (consolidating `main.json` + `layers.json`)
- [ ] Update `PicoCalc_App::create` to use `parse_keyboard_config` instead of `parse_via_layout`
- [ ] Add embedded JSON resource support for PICOCALC target
- [ ] Remove legacy `data/configs/picocalc/main.json` and `layers.json`
- [ ] Validate new picocalc `keyboard.json` with schema

---

## Future / Backlog

### Core Features

- [ ] **Calculation history** — record each evaluated expression and its result in a scrollable history list on the LCD
- [ ] **Export history** — button to export the session history as a Python script (each entry as a commented expression + assignment) or a LaTeX document (formatted as an `align` environment)
- [ ] **Symbol layer** — dedicated layer for inserting named symbols: Greek alphabet (α β γ δ … ω), uppercase variants, and Latin letter variables (a–z)

### Math Rendering

- [ ] **Math-view rendering** — typeset display of evaluated expressions using the AST, shown after `=` is pressed (TI-Nspire style)
  - [ ] `Math_Box` layout model — width, height, baseline offset per node; layout pass separate from paint pass
  - [ ] Phase 1: numbers, binary `+`/`−`, fractions (`/` → stacked numerator/denominator with rule)
  - [ ] Phase 2: `^` superscript (recursive scale parameter), `sqrt` with dynamic vinculum
  - [ ] Phase 3: functions (`sin`, `cos`, etc.) with parenthesised arguments
  - [ ] Hook `Math_Renderer` into `Display_Controller::render_lcd()` post-equals, using `last_ast`

### Symbolic Math (CAS)

- [ ] **Exact / approximate mode** — NSpire-style: all evaluation is symbolic (`simplify()`); pressing `≈` wraps the expression in `approx(…)` which force-evaluates to a decimal; `Node::simplify()` / `Node::clone()` scaffolding in place
  - [ ] `Rational` type for exact integer arithmetic (`1/3 + 1/6 = 1/2`)
  - [ ] Symbolic radicals — keep `sqrt(2)` unevaluated unless inside `approx()`
  - [ ] Variable binding — symbol table in `Calc_State`, variable tokens via the symbol layer
  - [ ] Differentiation — recursive AST transform `Node::differentiate(var)`

### System Improvements

- **Remove exceptions for embedded builds** — ARM toolchain disables exceptions by default; replace all `throw` statements with error codes, assertions, or std::optional returns throughout codebase
  - **Establish error handling policy** — define how to handle unrecoverable errors (e.g., placeholder_node::eval(), division by zero) without exceptions: assertions for programming errors, error codes for I/O failures, special return values (NaN) for math errors
- **AST display refactor** — render expressions using layout engine (fractions, superscripts, etc.)
- **ATAN2** action code for two-argument arctangent
- **Animation** — smooth cursor transitions in LVGL
- **Themes** — customisable colours and fonts
