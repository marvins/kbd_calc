# Custom LVGL Fonts

This document describes how to generate and integrate custom LVGL fonts for special characters not available in the default Montserrat font.

## Superscript Font

The superscript font (`lv_font_superscript.c`) provides proper rendering for power button labels (x², x³, xⁿ) and the square root symbol (√).

### Font Generation

The font was generated using `lv_font_conv` from the Geneva system font:

```bash
# Clone lv_font_conv (in parent directory)
cd /Users/marvin/Desktop/Projects
git clone https://github.com/lvgl/lv_font_conv.git
cd lv_font_conv
npm install

# Generate font with superscript glyphs and square root
npx lv_font_conv \
  --size 16 \
  --bpp 8 \
  --format lvgl \
  --font /System/Library/Fonts/Geneva.ttf \
  -r 0x00B2-0x00B3,0x207F,0x78,0x221A \
  -o /Users/marvin/Desktop/Projects/kbd_calc/src/overboard/gui/lv_font_superscript.c \
  --lv-font-name lv_font_superscript \
  --no-compress
```

### Parameters

- `--size 16`: Slightly larger than default Montserrat 14px for thicker appearance
- `--bpp 8`: Higher bit depth for thicker, smoother glyphs (was 4, increased to 8)
- `-r 0x00B2-0x00B3,0x207F,0x78,0x221A`: Unicode ranges for superscript ², ³, ⁿ, base character x, and square root √
- `--no-compress`: Disables RLE compression for simpler integration

### Integration Steps

1. **Font file**: Add generated `.c` file to `src/overboard/gui/`
2. **LVGL config**: Declare in `thirdparty/lv_conf.h` via `LV_FONT_CUSTOM_DECLARE`
3. **Build system**: Add to hardware CMakeLists.txt files (SDL, PicoCalc, RP2350)
4. **Application**: Apply to UI elements using `lv_obj_set_style_text_font()`

### Example: Power Button Labels and Square Root

In `keyboard_view.cpp`, the custom font is applied to power button labels and the square root button:

```cpp
// Use custom superscript font for power buttons and square root
if (key_code == core::Key_Code::POWER_2 ||
    key_code == core::Key_Code::POWER_3 ||
    key_code == core::Key_Code::POWER_N ||
    key_code == core::Key_Code::SQRT) {
    lv_obj_set_style_text_font(lbl, &lv_font_superscript, LV_PART_MAIN);
}
```

The corresponding display text in `keymap.cpp` uses UTF-8 encoded characters:

```cpp
case Key_Code::POWER_2: return "x\xC2\xB2";  // x²
case Key_Code::POWER_3: return "x\xC2\xB3";  // x³
case Key_Code::POWER_N: return "x\xE2\x81\xBF"; // xⁿ
case Key_Code::SQRT:    return "\xE2\x88\x9A"; // √
```

## Adding New Custom Fonts

To add a new custom font:

1. Identify the Unicode code points for the required glyphs
2. Use `lv_font_conv` to generate the font file
3. Follow the integration steps above
4. Update this document with the new font's purpose and generation command
