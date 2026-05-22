# Key Layers

<style>
kbd {
  border: 1px solid #ccc;
  border-radius: 3px;
  padding: 2px 6px;
  background-color: #f8f8f8;
  font-family: monospace;
  display: inline-block;
  min-width: 20px;
  text-align: center;
}
kbd.double-width {
  grid-column: span 2;
}
kbd.double-height {
  grid-row: span 2;
}
.keyboard-layout {
  display: grid;
  grid-template-columns: auto auto;
  gap: 20px;
  margin: 10px 0;
  border: 3px solid #888 !important;
  border-radius: 4px;
  padding: 10px;
  background-color: transparent;
}
.left-section {
  display: flex;
  flex-direction: column;
  gap: 10px;
}
.right-section-wrapper {
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.right-section {
  display: grid;
  grid-template-columns: repeat(4, auto);
  grid-template-rows: repeat(5, auto);
  gap: 4px;
  border: 3px solid #888 !important;
  border-radius: 4px;
  padding: 4px;
  background-color: transparent;
}
.key-group {
  border: 3px solid #888 !important;
  border-radius: 4px;
  padding: 4px;
  background-color: transparent;
  display: inline-grid;
  grid-template-columns: repeat(3, auto);
  gap: 4px;
  align-items: center;
}
.key-group-3x2 {
  border: 3px solid #888 !important;
  border-radius: 4px;
  padding: 4px;
  background-color: transparent;
  display: inline-grid;
  grid-template-columns: repeat(3, auto);
  gap: 4px;
}
.key-group-4x1 {
  border: 3px solid #888 !important;
  border-radius: 4px;
  padding: 4px;
  background-color: transparent;
  display: inline-grid;
  grid-template-columns: repeat(4, auto);
  gap: 4px;
}
.arrow-group {
  border: 3px solid #888 !important;
  border-radius: 4px;
  padding: 4px;
  background-color: transparent;
  display: inline-grid;
  grid-template-columns: repeat(3, auto);
  grid-template-rows: auto auto;
  gap: 4px;
  grid-template-areas:
    ". up ."
    "left down right";
}
</style>

Each layer occupies an **8-column × 7-row** asymmetric grid (42 cells, 34 keys with gaps).
This layout matches the **KISNT KN34** macropad physical arrangement:
- Split sections with gaps between logical key groups
- 7 keys in top rows, 6 in middle, 4-3 keys in bottom rows
- Use **Home** key to return to the Basic layer

**Keyboard Mapping** (for SDL simulator):
- Arrow keys (←→↑↓) map to cursor keys
- `qwe` / `asd` / `zxc` map to top-left 3×3 grid
- Number keys `0-9` map to calculator digits
- `Return` = Equals, `Backspace` = Backspace, `Escape` = All Clear

---

## Layer 0 — Basic

<div class="keyboard-layout">
  <div class="left-section">
    <div class="key-group"><kbd>Home</kbd> <kbd>&nbsp;</kbd> <kbd>BSP</kbd></div>
    <div class="key-group-3x2"><kbd>TRG</kbd><kbd>CST</kbd><kbd>PgUp</kbd><kbd>ALG</kbd><kbd>Var</kbd><kbd>PgDn</kbd></div>
    <div class="arrow-group">
      <kbd style="grid-area: up;">↑</kbd>
      <kbd style="grid-area: left;">←</kbd>
      <kbd style="grid-area: down;">↓</kbd>
      <kbd style="grid-area: right;">→</kbd>
    </div>
  </div>
  <div class="right-section-wrapper">
    <div class="key-group-4x1"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="right-section">
      <kbd>(</kbd> <kbd>)</kbd> <kbd>=</kbd> <kbd>+/-</kbd>
      <kbd>7</kbd> <kbd>8</kbd> <kbd>9</kbd> <kbd style="grid-row: span 2;">Aprx</kbd>
      <kbd>4</kbd> <kbd>5</kbd> <kbd>6</kbd>
      <kbd>1</kbd> <kbd>2</kbd> <kbd>3</kbd> <kbd style="grid-row: span 2;">Eval</kbd>
      <kbd style="grid-column: span 2;">0</kbd> <kbd>.</kbd>
    </div>
  </div>
</div>

---

## Layer 1 — Trig

Trigonometric and inverse trigonometric functions.

<div class="keyboard-layout">
  <div class="left-section">
    <div class="key-group"><kbd>Home</kbd> <kbd>&nbsp;</kbd> <kbd>BSP</kbd></div>
    <div class="key-group-3x2"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="arrow-group">
      <kbd style="grid-area: up;">↑</kbd>
      <kbd style="grid-area: left;">←</kbd>
      <kbd style="grid-area: down;">↓</kbd>
      <kbd style="grid-area: right;">→</kbd>
    </div>
  </div>
  <div class="right-section-wrapper">
    <div class="key-group-4x1"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="right-section">
      <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd>
      <kbd>sin</kbd> <kbd>cos</kbd> <kbd>tan</kbd> <kbd style="grid-row: span 2;">Aprx</kbd>
      <kbd>asin</kbd> <kbd>acos</kbd> <kbd>atan</kbd>
      <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd style="grid-row: span 2;">Eval</kbd>
      <kbd style="grid-column: span 2;">&nbsp;</kbd> <kbd>&nbsp;</kbd>
    </div>
  </div>
</div>

---

## Layer 2 — Constants

Mathematical constants.

<div class="keyboard-layout">
  <div class="left-section">
    <div class="key-group"><kbd>Home</kbd> <kbd>&nbsp;</kbd> <kbd>BSP</kbd></div>
    <div class="key-group-3x2"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="arrow-group">
      <kbd style="grid-area: up;">↑</kbd>
      <kbd style="grid-area: left;">←</kbd>
      <kbd style="grid-area: down;">↓</kbd>
      <kbd style="grid-area: right;">→</kbd>
    </div>
  </div>
  <div class="right-section-wrapper">
    <div class="key-group-4x1"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="right-section">
      <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd>
      <kbd>π</kbd> <kbd>e</kbd> <kbd>φ</kbd> <kbd style="grid-row: span 2;">Aprx</kbd>
      <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd>
      <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd style="grid-row: span 2;">Eval</kbd>
      <kbd style="grid-column: span 2;">&nbsp;</kbd> <kbd>&nbsp;</kbd>
    </div>
  </div>
</div>

---

## Layer 3 — Programmer

Hexadecimal digits, bitwise operations, and bit-shift keys.

<div class="keyboard-layout">
  <div class="left-section">
    <div class="key-group"><kbd>Home</kbd> <kbd>&nbsp;</kbd> <kbd>BSP</kbd></div>
    <div class="key-group-3x2"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="arrow-group">
      <kbd style="grid-area: up;">↑</kbd>
      <kbd style="grid-area: left;">←</kbd>
      <kbd style="grid-area: down;">↓</kbd>
      <kbd style="grid-area: right;">→</kbd>
    </div>
  </div>
  <div class="right-section-wrapper">
    <div class="key-group-4x1"><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd><kbd>&nbsp;</kbd></div>
    <div class="right-section">
      <kbd>AC</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd> <kbd>&nbsp;</kbd>
      <kbd>AND</kbd> <kbd>OR</kbd> <kbd>XOR</kbd> <kbd style="grid-row: span 2;">Aprx</kbd>
      <kbd>A</kbd> <kbd>B</kbd> <kbd>C</kbd>
      <kbd>D</kbd> <kbd>E</kbd> <kbd>F</kbd> <kbd style="grid-row: span 2;">Eval</kbd>
      <kbd style="grid-column: span 2;">0</kbd> <kbd>.</kbd>
    </div>
  </div>
</div>

---

## Key Code Reference

| Symbol | Key_Code | Notes |
|--------|----------|-------|
| `BSP` | `BACKSPACE` | Deletes last character |
| `AC` | `ALL_CLEAR` | Clears expression and resets state |
| `+/-` | `NEGATE` | Flips sign of current value |
| `◀` / `▶` | `CURSOR_LEFT` / `CURSOR_RIGHT` | Move cursor in expression |
| `Trig` | `LAYER_NEXT` | Enter Trig layer |
| `CST` | `CONST_LAYER` | Enter Constants layer |
| `BACK` | `LAYER_PREV` / `LAYER_HOME` | Return to Basic layer |
| `x²` | `POWER_2` | Squares current value |
| `xʸ` | `POWER_N` | Raises x to power y |
| `√` | `SQRT` | Square root |
| `π` | `PI` | Inserts 3.14159… |
| `e` | `EULER` | Inserts 2.71828… |
| `φ` | `PHI` | Inserts 1.61803… |
| `τ` | `TAU` | Inserts 6.28318… |
| `AND` / `OR` / `XOR` / `NOT` | `BIT_AND` / `BIT_OR` / `BIT_XOR` / `BIT_NOT` | Bitwise ops |
| `<<` / `>>` | `SHIFT_LEFT` / `SHIFT_RIGHT` | Bit shift |
