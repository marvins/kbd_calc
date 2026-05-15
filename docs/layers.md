# Key Layers

Each layer occupies a **5-column × 6-row** physical grid (30 keys total).  
The bottom-right key always cycles to the next layer.

---

## Layer 0 — Basic

Standard numpad layout with memory and utility keys.

<table>
  <thead>
    <tr>
      <th>Col 1</th>
      <th>Col 2</th>
      <th>Col 3</th>
      <th>Col 4</th>
      <th>Col 5</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><kbd>AC</kbd></td>
      <td><kbd>(</kbd></td>
      <td><kbd>)</kbd></td>
      <td><kbd>%</kbd></td>
      <td><kbd>DEL</kbd></td>
    </tr>
    <tr>
      <td><kbd>MR</kbd></td>
      <td><kbd>MS</kbd></td>
      <td><kbd>M+</kbd></td>
      <td><kbd>MC</kbd></td>
      <td><kbd>+/-</kbd></td>
    </tr>
    <tr>
      <td><kbd>7</kbd></td>
      <td><kbd>8</kbd></td>
      <td><kbd>9</kbd></td>
      <td><kbd>÷</kbd></td>
      <td><kbd>×</kbd></td>
    </tr>
    <tr>
      <td><kbd>4</kbd></td>
      <td><kbd>5</kbd></td>
      <td><kbd>6</kbd></td>
      <td><kbd>-</kbd></td>
      <td><kbd>+</kbd></td>
    </tr>
    <tr>
      <td><kbd>1</kbd></td>
      <td><kbd>2</kbd></td>
      <td><kbd>3</kbd></td>
      <td rowspan="2"><kbd>=</kbd></td>
      <td rowspan="2"><kbd>SCI ▶</kbd></td>
    </tr>
    <tr>
      <td colspan="2"><kbd>0</kbd></td>
      <td><kbd>.</kbd></td>
    </tr>
  </tbody>
</table>

---

## Layer 1 — Scientific

Trigonometry, logarithms, powers, and mathematical constants.  
Digit keys retained on lower rows.

<table>
  <thead>
    <tr>
      <th>Col 1</th>
      <th>Col 2</th>
      <th>Col 3</th>
      <th>Col 4</th>
      <th>Col 5</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><kbd>AC</kbd></td>
      <td><kbd>π</kbd></td>
      <td><kbd>e</kbd></td>
      <td><kbd>√</kbd></td>
      <td><kbd>DEL</kbd></td>
    </tr>
    <tr>
      <td><kbd>sin</kbd></td>
      <td><kbd>cos</kbd></td>
      <td><kbd>tan</kbd></td>
      <td><kbd>x²</kbd></td>
      <td><kbd>xʸ</kbd></td>
    </tr>
    <tr>
      <td><kbd>asin</kbd></td>
      <td><kbd>acos</kbd></td>
      <td><kbd>atan</kbd></td>
      <td><kbd>log</kbd></td>
      <td><kbd>ln</kbd></td>
    </tr>
    <tr>
      <td><kbd>7</kbd></td>
      <td><kbd>8</kbd></td>
      <td><kbd>9</kbd></td>
      <td><kbd>÷</kbd></td>
      <td><kbd>×</kbd></td>
    </tr>
    <tr>
      <td><kbd>4</kbd></td>
      <td><kbd>5</kbd></td>
      <td><kbd>6</kbd></td>
      <td><kbd>-</kbd></td>
      <td><kbd>+</kbd></td>
    </tr>
    <tr>
      <td><kbd>1</kbd></td>
      <td><kbd>2</kbd></td>
      <td><kbd>3</kbd></td>
      <td><kbd>0</kbd></td>
      <td><kbd>PRG ▶</kbd></td>
    </tr>
  </tbody>
</table>

---

## Layer 2 — Programmer

Hexadecimal digits, bitwise operations, and bit-shift keys.

<table>
  <thead>
    <tr>
      <th>Col 1</th>
      <th>Col 2</th>
      <th>Col 3</th>
      <th>Col 4</th>
      <th>Col 5</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><kbd>AC</kbd></td>
      <td><kbd>AND</kbd></td>
      <td><kbd>OR</kbd></td>
      <td><kbd>XOR</kbd></td>
      <td><kbd>DEL</kbd></td>
    </tr>
    <tr>
      <td><kbd>NOT</kbd></td>
      <td><kbd>&lt;&lt;</kbd></td>
      <td><kbd>&gt;&gt;</kbd></td>
      <td><kbd>(</kbd></td>
      <td><kbd>)</kbd></td>
    </tr>
    <tr>
      <td><kbd>A</kbd></td>
      <td><kbd>B</kbd></td>
      <td><kbd>C</kbd></td>
      <td><kbd>÷</kbd></td>
      <td><kbd>×</kbd></td>
    </tr>
    <tr>
      <td><kbd>D</kbd></td>
      <td><kbd>E</kbd></td>
      <td><kbd>F</kbd></td>
      <td><kbd>-</kbd></td>
      <td><kbd>+</kbd></td>
    </tr>
    <tr>
      <td><kbd>7</kbd></td>
      <td><kbd>8</kbd></td>
      <td><kbd>9</kbd></td>
      <td><kbd>4</kbd></td>
      <td><kbd>5</kbd></td>
    </tr>
    <tr>
      <td><kbd>6</kbd></td>
      <td><kbd>1</kbd></td>
      <td><kbd>2</kbd></td>
      <td><kbd>3</kbd></td>
      <td><kbd>BAS ▶</kbd></td>
    </tr>
  </tbody>
</table>

---

## Key Code Reference

| Symbol | Key_Code | Notes |
|--------|----------|-------|
| `AC` | `All_Clear` | Clears expression and resets state |
| `DEL` | `Backspace` | Deletes last character |
| `MR` / `MS` / `M+` / `MC` | `Mem_Recall` / `Mem_Store` / `Mem_Add` / `Mem_Clear` | Memory registers |
| `+/-` | `Negate` | Flips sign of current value |
| `SCI ▶` / `PRG ▶` / `BAS ▶` | `Layer_Next` | Cycles to next layer |
| `π` | `Pi` | Inserts 3.14159… |
| `e` | `Euler` | Inserts 2.71828… |
| `xʸ` | `Power_N` | Raises x to power y |
| `AND` / `OR` / `XOR` / `NOT` | `Bit_And` / `Bit_Or` / `Bit_Xor` / `Bit_Not` | Bitwise ops |
| `<<` / `>>` | `Shift_Left` / `Shift_Right` | Bit shift |
