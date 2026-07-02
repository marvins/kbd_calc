# Custom LVGL Fonts

This document describes how to generate and integrate custom LVGL fonts for special characters not available in the default Montserrat font.

## Superscript Font

The superscript font (`lv_font_superscript.c`) provides proper rendering for power button labels (x², x³, xⁿ) and the square root symbol (√).

### Font Generation

The font was generated using `lv_font_conv` from Arial Unicode (the only macOS system font with ∛/∜ glyphs).

Only one size is needed — `lv_font_superscript` is always used alongside `LVGL_FONT_SMALL` (Montserrat 12px), so both must be the same size. If `LVGL_FONT_SMALL` changes, regenerate this font to match.

Use the provided script (run from the repo root):

```bash
bash scripts/generate_fonts.sh
```

The script regenerates both `lv_font_superscript_regular.c` and `lv_font_superscript_bold.c` and handles locating `lv_font_conv` automatically (global install or sibling-directory clone). See the script header for prerequisite details.

### Parameters

- `--size 12`: Matches `LVGL_FONT_SMALL` (Montserrat 12px) so custom and default font items render at the same size
- `--bpp 4`: Standard bit depth for LVGL fonts
- `--font Arial Unicode.ttf`: Switched from Geneva (missing ∛/∜) to Arial Unicode which has all required math glyphs
- `-r 0x0020-0x007E,...`: Full printable ASCII, superscript ², ³, ⁿ, root symbols √, ∛, ∜, and Greek letters π, φ, τ — full ASCII is required so numbered menu prefixes (e.g. `1) π`) render correctly with a single font
- `--no-compress`: Disables RLE compression for simpler integration

### Integration Steps

1. **Font file**: Add generated `.c` file to `src/overboard/gui/`
2. **LVGL config**: Declare in `thirdparty/lv_conf.h` via `LV_FONT_CUSTOM_DECLARE`
3. **Build system**: Add to hardware CMakeLists.txt files (SDL, PicoCalc, RP2350)
4. **Application**: Apply to UI elements using `lv_obj_set_style_text_font()`

### Example: Power Button Labels and Square Root

Font selection is handled by `font::requires_custom_font()` in `src/overboard/font/font_selector.cpp`, which checks display strings for known UTF-8 sequences and applies `lv_font_superscript` automatically. Display strings in `action_code.cpp`:

```cpp
case Action_Code::POWER_2:   return "x\xC2\xB2";     // x²
case Action_Code::POWER_3:   return "x\xC2\xB3";     // x³
case Action_Code::POWER_N:   return "x\xE2\x81\xBF"; // xⁿ
case Action_Code::SQRT:      return "\xE2\x88\x9A";  // √
case Action_Code::CUBE_ROOT: return "\xE2\x88\x9B";  // ∛
```

## Adding New Custom Fonts

To add a new custom font:

1. Identify the Unicode code points for the required glyphs
2. Use `lv_font_conv` to generate the font file
3. Follow the integration steps above
4. Update this document with the new font's purpose and generation command
