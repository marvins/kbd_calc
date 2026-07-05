# Project Tasks

## Phase 0: Math Canvas Cleanup

Goal: Refactor the monolithic draw lambda in `math_canvas.cpp` for better maintainability and performance.

- [ ] Extract per-kind draw functions — split the 120-line lambda into named static helpers (`draw_atom`, `draw_placeholder`, `draw_fraction_bar`, `draw_sqrt`)
- [ ] Add `make_area` inline helper — eliminate repeated `lv_area_t` construction with `static_cast<int32_t>` noise
- [ ] Replace `std::function` with local recursive struct — zero-cost recursion without heap allocation
- [ ] Remove `offset_x / offset_y` parameters — `layout()` already sets absolute positions, offsets are always `0, 0`
- [ ] Derive canvas size internally — query `lv_obj_get_width/height(canvas)` instead of passing `width`/`height` parameters
- [ ] Move result string rendering into layout tree — currently drawn with hardcoded offsets, should be a proper layout region

---

## Phase 1: Calculator — Basic Algebra  ← **NEXT**

Goal: Get the calculator working end-to-end for basic arithmetic and algebra.

**New Operations:**

*Roots & Powers:*
- [x] `factorial(n)` or `n!` — factorial function (already exists)
- [x] `cbrt(x)` — cube root (∛x)
- [x] `nthroot(x, n)` — nth root (ⁿ√x)
- [x] `x^3` — cube power (already exists as POWER_3)
- [x] `x^y` — general power function (POWER_N)

*Absolute Value & Sign:*
- [x] `abs(x)` — absolute value (|x|)
- [ ] `sign(x)` — sign function (returns -1, 0, or 1)

*Rounding & Truncation:*
- [ ] `floor(x)` — largest integer ≤ x
- [ ] `ceil(x)` — smallest integer ≥ x
- [ ] `round(x)` — round to nearest integer
- [ ] `trunc(x)` — truncate to integer (towards zero)
- [ ] `frac(x)` — fractional part (x - floor(x))

*Number Theory:*
- [ ] `gcd(a, b)` — greatest common divisor (Euclidean algorithm)
- [ ] `lcm(a, b)` — least common multiple (a*b / gcd(a,b))
- [ ] `is_prime(n)` — primality test (returns 1 if prime, 0 otherwise)
- [ ] `prime_factors(n)` — returns list/count of prime factors (display as string)
- [ ] `is_even(n)` — returns 1 if even, 0 if odd
- [ ] `is_odd(n)` — returns 1 if odd, 0 if even

*Combinatorics:*
- [ ] `nCr(n, r)` or `binom(n, r)` — binomial coefficient (n choose r)
- [ ] `nPr(n, r)` or `perm(n, r)` — permutations (n!/(n-r)!)

*Min/Max:*
- [ ] `min(a, b)` — minimum of two values
- [ ] `max(a, b)` — maximum of two values
- [ ] `clamp(x, min, max)` — constrain x to [min, max]

*Random Numbers:*
- [ ] `rand()` — random float in [0, 1)
- [ ] `randint(a, b)` — random integer in [a, b]
- [ ] `seed(n)` — set random seed for reproducibility

*Modular Arithmetic:*
- [x] `mod(a, b)` — modulo operation (already exists)
- [ ] `div(a, b)` — integer division (floor(a/b))
- [ ] `rem(a, b)` — remainder (a - b*div(a,b))
- [ ] `mod_pow(base, exp, mod)` — modular exponentiation (base^exp mod m)
- [ ] `mod_inv(a, m)` — modular multiplicative inverse

*Logarithms & Exponentials:*
- [x] `log(x)` — base-10 logarithm (already exists)
- [x] `ln(x)` — natural logarithm (already exists)
- [x] `exp(x)` — e^x (already exists)
- [ ] `log2(x)` — base-2 logarithm
- [ ] `logb(x, b)` — logarithm base b

*Internal Refactoring:*
- [ ] **Function dispatch table** — replace the fixed `if`-chain in `Function_Node::eval()` (and the parallel chains in `to_latex()` and the parser) with a registered function table (see Phase 4 `Function_Registry`). Adding a new function currently requires edits in three separate places.

*Summation & Products:*
- [ ] `sum(list)` — sum of array/list elements (future: requires array support)
- [ ] `prod(list)` — product of array/list elements
- [ ] `mean(list)` — arithmetic mean
- [ ] `geomean(list)` — geometric mean

---

## Phase 2: Status Panel Update

Goal: Refresh the status panel with a clock widget and About popup.

- [ ] Update F2 footer label to "Board"
- [ ] Create Board Stats popup with system info:
  - [x] Define `I_System_Info` HAL interface
- [x] SDL implementation:
  - [x] CPU temperature (Linux `/sys/class/thermal`, macOS mock)
  - [x] Battery (mock - no standard API on dev machines)
  - [x] Storage stats (`statvfs`)
  - [x] USB/BT (mock)
  - [ ] Verify on macOS (current)
  - [ ] Verify on Linux
- [ ] CPU temperature:
  - [ ] PicoCalc: RP2350 internal temperature sensor (ADC)
  - [ ] Pi Zero: `/sys/class/thermal/thermal_zone0/temp` (implemented, needs verify)
- [ ] Battery voltage:
  - [ ] PicoCalc: MAX17048 via I2C
  - [ ] Pi Zero: LiPo HAT via power_supply sysfs
- [ ] Storage stats:
  - [ ] PicoCalc: SD card FAT32
  - [ ] Pi Zero: SD card (implemented, needs verify)
- [ ] USB/BT status:
  - [ ] PicoCalc: TinyUSB + optional BT module
  - [ ] Pi Zero: USB gadget mode, hciconfig for BT


---

## Phase 3: Function-Key Popup System

Goal: F1–F10 on PicoCalc shows a popup anchored to the footer slot, with paging for F6-F10.

**Basic F-Key Support (F1-F5):**
- [ ] Define `I_Popup` interface — `show()`, `hide()`, `handle_input()`, `render()`
- [ ] Implement `Function_Key_Popup` — generic popup with scrollable item list
- [ ] Map F1–F5 `Action_Code` values to footer slots
- [ ] On F-key press: show matching popup via `Panel_Manager`
- [ ] Popup dismissal: any non-F-key press or second press of same F-key closes it
- [ ] Add unit tests for popup lifecycle

**Extended F-Key Support (F6-F10):**
- [ ] Footer paging system:
  - [ ] Add footer page state (page 0: F1-F5, page 1: F6-F10)
  - [ ] PAGE_DOWN key advances to next footer page (F1-F5 → F6-F10)
  - [ ] PAGE_UP key returns to previous footer page (F6-F10 → F1-F5)
  - [ ] Visual indicator showing current footer page (e.g., "Page 1/2")
  - [ ] Wrap-around behavior (optional): PAGE_DOWN on last page returns to first
- [ ] Footer label updates:
  - [ ] Dynamically update footer labels when page changes
  - [ ] Show F6-F10 labels on page 1 (or blank if unassigned)
  - [ ] Handle layer interactions (page state independent of layer state)
- [ ] F6-F10 popup assignments:
  - [ ] Map F6-F10 action codes to popup slots 5-9
  - [ ] Ensure F6-F10 popups work identically to F1-F5
- [ ] Edge cases and robustness:
  - [ ] Closing popup doesn't reset footer page
  - [ ] Layer switching preserves current footer page
  - [ ] ESC or mode change can optionally reset to page 0
- [ ] Add unit tests for:
  - [ ] Footer page navigation (PAGE_UP/PAGE_DOWN)
  - [ ] F6-F10 popup triggering on correct page
  - [ ] Page state persistence across popups and layers

---

## Phase 3.5: F-Key Contexts

Goal: PgUp/PgDn cycles through named contexts, each defining what F1–F5 do and their labels in the footer bar. Each F-key opens a popup menu with context-specific items.

**Core Context Infrastructure:**
- [ ] Define `F_Key_Context` struct with name, 5 footer labels, and 5 popup item lists
- [ ] Add `m_contexts` vector and `m_active_context` index to `Calculator_App`
- [ ] Implement `cycle_context(int delta)` and `apply_context()` in `Calculator_App`
- [ ] Wire PgUp/PgDn to `cycle_context()` while calculator is active
- [ ] Update footer bar to show context name and page indicator (e.g. "Core Math 1/4")

**Context Definitions (Default Layer):**
- [ ] Core Math context: F1=Alg, F2=Trig, F3=Const, F4=Log, F5=Round
- [ ] Number Theory context: F1=Factor, F2=Divis, F3=Mod, F4=Base, F5=Comb
- [ ] Statistics context: F1=Avg, F2=Spread, F3=Dist, F4=Fit, F5=Misc
- [ ] Units & Conversions context: F1=Len, F2=Mass, F3=Temp, F4=Area, F5=Speed

**Popup Item Implementation:**
- [ ] Core Math popups: Logarithms and Rounding items
- [ ] Number Theory popups: factorize, is_prime, GCD, LCM, mod, base conversion, nCr, nPr
- [ ] Statistics popups: mean, median, std dev, normal CDF, regression, sum/min/max
- [ ] Units & Conversions popups: length, mass, temperature, area, speed conversions

**Layer 2 (F6–F10):**
- [ ] Define second-bank contexts for F6–F10 using the same physical keys on Layer 2
- [ ] Ensure context switching works on Layer 2 independently or as a separate bank

**UI / Polish:**
- [ ] Footer labels update when context changes
- [ ] Popup state resets safely when switching contexts
- [ ] Add unit tests for context cycling and popup assignment

---

## Phase 4: Catalog System

Goal: Universal catalog of all built-in functions, constants, and variables, accessible as both a standalone app and a calculator popup.

**Function Registry:**
- [ ] `Function_Registry` class — central repository for all math functions
  - [ ] Register function metadata: name, symbol, description, operand count, category
  - [ ] Categories: Arithmetic, Trigonometry, Hyperbolic, Logarithmic, Statistics, Linear Algebra, Constants, Variables
  - [ ] Support registration at initialization (static registration pattern)
  - [ ] Query interface: search by name, filter by category, get all functions
- [ ] Auto-register existing functions:
  - [ ] All function operators (`Sin`, `Cos`, `Sqrt`, `Log`, etc.)
  - [ ] Constants (`Pi`, `E`, `Phi`, `Tau`)
  - [ ] Future: User-defined functions and variables
- [ ] Function metadata structure:
  - [ ] Name (display string)
  - [ ] Action code (for insertion)
  - [ ] Description (help text)
  - [ ] Category (for filtering)
  - [ ] Operand count and signature (e.g., "sin(x)", "atan2(y, x)")

**Catalog App (Standalone):**
- [ ] `Catalog_App` class — browsable list of all registered functions
  - [ ] Scrollable list view with search/filter
  - [ ] Category tabs or dropdown for filtering
  - [ ] Display: function name, signature, brief description
  - [ ] Select function to view detailed help (syntax, examples, domain/range)
  - [ ] Action: Copy function signature to clipboard (or return to calc with function inserted)
- [ ] Register catalog app in app menu:
  - [ ] Icon: `LV_SYMBOL_LIST` or custom catalog icon
  - [ ] Hotkey: 'f' (for functions) or 't' (for tools)
  - [ ] Menu priority: place after Calculator, before Settings
- [ ] Search functionality:
  - [ ] Text input field for filtering functions by name
  - [ ] Incremental search (update list as user types)
  - [ ] Search across name, description, and category

**Catalog Popup (In Calculator):**
- [ ] Add catalog popup to calculator F-key menu (e.g., F5 or F9)
  - [ ] Reuse `Function_Registry` for data
  - [ ] Compact view: show only function names, no descriptions
  - [ ] Numbered shortcuts (1-9) for quick selection
  - [ ] Selecting function inserts it into expression at cursor
- [ ] Integration with calculator:
  - [ ] Insert function with placeholder arguments (e.g., `sin(□)`)
  - [ ] Position cursor on first placeholder for immediate entry
  - [ ] Close popup after insertion
- [ ] Category filtering in popup:
  - [ ] Arrow keys or tabs to switch categories
  - [ ] Show category name in popup title (e.g., "Catalog: Trig")

**Help System Integration:**
- [ ] Function help viewer:
  - [ ] Display syntax, parameters, description
  - [ ] Show examples with expected results
  - [ ] Include domain/range restrictions (e.g., "asin: -1 ≤ x ≤ 1")
  - [ ] Link to related functions (e.g., sin → asin, sinh)
- [ ] Access help from catalog:
  - [ ] Long-press or secondary action on function shows help
  - [ ] Help displayed in overlay or separate panel

**Unit Tests:**
- [ ] Function registry:
  - [ ] Registration and query
  - [ ] Category filtering
  - [ ] Search functionality
- [ ] Catalog app:
  - [ ] List rendering
  - [ ] Search and filter
  - [ ] Function selection
- [ ] Catalog popup:
  - [ ] Integration with calculator
  - [ ] Function insertion with placeholders

---

## Phase 5: Variable Support

Goal: Add user-defined variables with storage, recall, and management through the catalog system.

**Variable Infrastructure:**
- [ ] Variable storage in `Calc_State`:
  - [ ] `std::map<std::string, double> variables` — symbol table
  - [ ] Reserve system variable names (e.g., `ans`, `pi`, `e`)
  - [ ] Variable name validation (alphanumeric, starts with letter, case-sensitive)
- [ ] `Variable_Node` evaluation context:
  - [ ] Extend `eval()` to accept evaluation context with variable bindings
  - [ ] Return variable value from context, or NaN if undefined
  - [ ] Update all node `eval()` signatures to pass context through recursion
- [ ] Variable assignment syntax:
  - [ ] `x = 5` creates/updates variable `x` with value 5
  - [ ] `x = sin(pi/4)` evaluates RHS and stores result
  - [ ] Assignment returns the assigned value (for display)

**Variable UI:**
- [ ] Variable menu/popup:
  - [ ] List all defined variables with current values
  - [ ] Show as "Vars" in F-key menu (e.g., F6 or dedicated key)
  - [ ] Numbered shortcuts for quick recall
  - [ ] Selecting variable inserts its name into expression
- [ ] Variable management actions:
  - [ ] Delete variable: select and press DELETE
  - [ ] Clear all variables: confirmation dialog
  - [ ] Rename variable (optional): edit name in-place
- [ ] Variable assignment workflow:
  - [ ] Type variable name (letter keys when not in number entry mode)
  - [ ] Press `=` to enter assignment mode
  - [ ] Enter expression for RHS
  - [ ] Press EVAL to complete assignment and display result

**Catalog Integration:**
- [ ] Register variables in `Function_Registry`:
  - [ ] Variables appear in "Variables" category
  - [ ] Show variable name and current value
  - [ ] Update catalog dynamically when variables are added/deleted
- [ ] Variable recall from catalog:
  - [ ] Select variable from catalog inserts name into expression
  - [ ] Show current value in preview/description area
- [ ] System vs. user variables:
  - [ ] System variables (`ans`, `pi`, `e`) marked read-only
  - [ ] User variables shown with edit/delete options

**Special Variables:**
- [ ] `ans` — stores result of last evaluation
  - [ ] Auto-updated after each EVAL
  - [ ] Read-only (cannot be assigned by user)
  - [ ] Available immediately for use in next expression
- [ ] Reserved constants:
  - [ ] `pi`, `e`, `phi`, `tau` treated as read-only variables
  - [ ] Attempting assignment shows error "Cannot assign to constant"

**Persistence (Optional):**
- [ ] Save variables to settings/config file
- [ ] Restore variables on app startup
- [ ] Export variables to file (CSV or JSON)
- [ ] Import variables from file

**Unit Tests:**
- [ ] Variable storage and retrieval
- [ ] Assignment parsing and execution
- [ ] Variable evaluation in expressions
- [ ] Special variable behavior (`ans`)
- [ ] Catalog integration (registration, filtering)
- [ ] Edge cases:
  - [ ] Undefined variable returns NaN
  - [ ] Reassignment updates value
  - [ ] Reserved name rejection

---

## Phase 6: History Navigation and Management

Goal: TI-Nspire style history navigation allowing selective recall of expressions or results, plus history management.

**History Navigation (TI-Nspire Style):**
- [ ] Add history navigation state to `Calc_Engine`:
  - [ ] Current history index (which entry is selected)
  - [ ] Navigation mode flag (browsing vs. editing)
  - [ ] Selected content type (expression vs. result)
- [ ] **UP arrow**: Navigate to previous history entry (move back in time)
- [ ] **DOWN arrow**: Navigate to next history entry (move forward in time), or exit history mode if at newest
- [ ] **LEFT arrow**: Insert the **expression** from current history entry into editor
- [ ] **RIGHT arrow**: Insert the **result** from current history entry into editor
- [ ] Visual feedback:
  - [ ] Highlight selected history entry in the history table
  - [ ] Show "History Mode" indicator in status area
  - [ ] Preview expression/result that will be inserted
- [ ] Clear history navigation mode on:
  - [ ] Any edit action (typing, delete, etc.)
  - [ ] ESC key
  - [ ] Successful insertion (LEFT or RIGHT arrow)
- [ ] Add unit tests for:
  - [ ] History navigation state transitions
  - [ ] Expression vs. result insertion
  - [ ] Mode clearing on edits

**History Management:**
- [ ] Delete individual history entries:
  - [ ] While in history navigation mode, DELETE key removes selected entry
  - [ ] Confirm deletion with visual feedback (fade out animation)
  - [ ] Update history indices after deletion
- [ ] Clear all history:
  - [ ] Add "Clear History" action code
  - [ ] Add confirmation dialog for clearing all entries
  - [ ] Preserve current expression when clearing history
- [ ] History size management:
  - [ ] Enforce MAX_HISTORY limit (currently 20)
  - [ ] Auto-remove oldest entries when limit reached
  - [ ] Optional: Make MAX_HISTORY user-configurable via settings
- [ ] Add unit tests for:
  - [ ] Individual entry deletion
  - [ ] History clearing
  - [ ] Size limit enforcement

---

## Phase 7: Array and Matrix Support

Goal: Add first-class support for 1D arrays (vectors) and ND arrays (matrices) with basic linear algebra operations.

**Data Structures:**
- [ ] `Array_Node` AST node — holds dimensions, data storage, and shape metadata
- [ ] `Matrix` value type — stores elements in row-major order with dimension tracking
- [ ] Array literal syntax — `[1, 2, 3]` for vectors, `[[1,2], [3,4]]` for matrices
- [ ] Placeholder templates — `[□, □, □]` for quick vector entry, matrix templates for common sizes

**Construction & Access:**
- [ ] Array constructor functions:
  - [ ] `zeros(n)` / `zeros(m, n)` — create zero-filled vector/matrix
  - [ ] `ones(n)` / `ones(m, n)` — create ones-filled vector/matrix
  - [ ] `eye(n)` — identity matrix
  - [ ] `diag([v1, v2, ...])` — diagonal matrix from vector
  - [ ] `linspace(start, stop, n)` — linearly spaced vector
  - [ ] `arange(start, stop, step)` — range vector with step size
- [ ] Element access — `A[i, j]` notation for matrices, `v[i]` for vectors
- [ ] Submatrix extraction — `A[1:3, 2:4]` slice syntax
- [ ] Shape queries — `size(A)`, `rows(A)`, `cols(A)`, `length(v)`

**Basic Operations:**
- [ ] Element-wise arithmetic — `A + B`, `A - B`, `A .* B`, `A ./ B` (broadcasting rules)
- [ ] Scalar operations — `k * A`, `A / k`, `A + k`
- [ ] Matrix multiplication — `A * B` (standard matrix product)
- [ ] Transpose — `A'` or `transpose(A)`
- [ ] Concatenation — `hcat(A, B)`, `vcat(A, B)` for horizontal/vertical stacking
- [ ] Reshape — `reshape(A, m, n)` to change dimensions while preserving data

**Reduction Operations:**
- [ ] `sum(A)` — sum of all elements, or along dimension
- [ ] `prod(A)` — product of all elements
- [ ] `mean(A)` — arithmetic mean
- [ ] `std(A)` — standard deviation
- [ ] `min(A)` / `max(A)` — extrema
- [ ] `trace(A)` — sum of diagonal elements
- [ ] `det(A)` — determinant (2×2 and 3×3 direct formulas)
- [ ] `norm(v)` — Euclidean norm of vector

**Display & Rendering:**
- [ ] Matrix layout in typeset view — grid rendering with proper alignment
- [ ] Compact notation for large matrices — show corners with ellipsis
- [ ] Vector display — horizontal `[1, 2, 3]` or vertical column format

**Unit Tests:**
- [ ] Array construction and initialization
- [ ] Element access and slicing
- [ ] Arithmetic operations with dimension checks
- [ ] Matrix multiplication correctness
- [ ] Reduction operations accuracy

---

## Phase 8: Advanced Linear Algebra

Goal: Implement matrix factorizations, system solvers, and advanced decomposition methods for scientific computing.

**Matrix Factorizations:**
- [ ] **LU Decomposition** — `[L, U, P] = lu(A)` with partial pivoting
  - [ ] Solve `Ax = b` via forward/backward substitution
  - [ ] Compute determinant via diagonal product of U
- [ ] **QR Decomposition** — `[Q, R] = qr(A)` using Householder reflections or Gram-Schmidt
  - [ ] Least squares solver `x = qr_solve(A, b)`
  - [ ] Orthonormalization of column space
- [ ] **Cholesky Decomposition** — `L = chol(A)` for symmetric positive-definite matrices
  - [ ] Efficient solver for symmetric systems
  - [ ] Numerical stability checks

**Eigenvalue Problems:**
- [ ] **Power iteration** — dominant eigenvalue and eigenvector
- [ ] **QR algorithm** — compute all eigenvalues iteratively
- [ ] **Characteristic polynomial** — symbolic or numeric roots for small matrices
- [ ] `eig(A)` function — return eigenvalues and eigenvectors
- [ ] `eigvals(A)` — eigenvalues only

**Singular Value Decomposition (SVD):**
- [ ] **SVD factorization** — `[U, S, V] = svd(A)` where `A = U * S * V'`
- [ ] Golub-Reinsch algorithm for numerical SVD
- [ ] Truncated SVD for low-rank approximation
- [ ] Condition number — `cond(A) = σ_max / σ_min`
- [ ] Matrix rank — `rank(A)` via singular value threshold

**Linear System Solvers:**
- [ ] **Direct solver** — `x = A \ b` using LU decomposition
- [ ] **Least squares** — `x = lsq(A, b)` for overdetermined systems (QR or SVD)
- [ ] **Pseudo-inverse** — `A_pinv = pinv(A)` via SVD
- [ ] **Iterative solvers** (optional):
  - [ ] Conjugate gradient for symmetric positive-definite systems
  - [ ] GMRES for general systems
  - [ ] Preconditioners for improved convergence

**Matrix Analysis:**
- [ ] Spectral radius — `ρ(A) = max(|λ_i|)`
- [ ] Matrix norms — Frobenius, operator norms (1, 2, ∞)
- [ ] Null space basis — `null(A)` via SVD
- [ ] Range (column space) basis — `range(A)` via QR or SVD

**Advanced Operations:**
- [ ] Matrix exponential — `expm(A)` via Padé approximation or eigendecomposition
- [ ] Matrix power — `A^k` for integer k
- [ ] Kronecker product — `kron(A, B)`
- [ ] Vectorization — `vec(A)` to stack columns

**Numerical Robustness:**
- [ ] Pivoting strategies for LU (partial, full)
- [ ] Singular matrix detection and error handling
- [ ] Ill-conditioned matrix warnings
- [ ] Iterative refinement for improved accuracy

**Unit Tests:**
- [ ] Factorization correctness (reconstruction check: `A ≈ L*U`)
- [ ] System solver validation against known solutions
- [ ] SVD properties (orthogonality of U and V)
- [ ] Pseudo-inverse uniqueness and Moore-Penrose properties
- [ ] Numerical stability on ill-conditioned matrices

---

## Future / Backlog

### Core Features

- [ ] **Export C** — button to export the session history as a Python script (each entry as a commented expression + assignment) or a LaTeX document (formatted as an `align` environment)
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
