#!/usr/bin/env bash
# generate_fonts.sh — Regenerate all custom LVGL fonts for the kbd_calc project.
#
# Run from the repo root:
#   bash scripts/generate_fonts.sh
#
# Prerequisites:
#   lv_font_conv must be available. Either:
#     a) npm install -g lv_font_conv   (global)
#     b) Clone + npm install alongside this repo:
#          git clone https://github.com/lvgl/lv_font_conv.git ../lv_font_conv
#          cd ../lv_font_conv && npm install
#
# The script tries 'lv_font_conv' on PATH first, then falls back to the
# sibling-directory clone at ../lv_font_conv/lv_font_conv.js via node.

set -euo pipefail

# ── Locate lv_font_conv ────────────────────────────────────────────────────────
if command -v lv_font_conv &>/dev/null; then
    CONV="lv_font_conv"
elif [[ -f "../lv_font_conv/lv_font_conv.js" ]]; then
    CONV="node ../lv_font_conv/lv_font_conv.js"
else
    echo "ERROR: lv_font_conv not found." >&2
    echo "  Install globally:  npm install -g lv_font_conv" >&2
    echo "  Or clone locally:  git clone https://github.com/lvgl/lv_font_conv.git ../lv_font_conv && cd ../lv_font_conv && npm install" >&2
    exit 1
fi

# ── Common glyph ranges ────────────────────────────────────────────────────────
# 0x0020-0x007E  Full printable ASCII (required for "1) label" prefixes)
# 0x00B2-0x00B3  ² ³ (superscript 2/3)
# 0x207F         ⁿ (superscript n)
# 0x221A-0x221C  √ ∛ ∜ (square/cube/fourth root)
# 0x03C0         π
# 0x03C6         φ
# 0x03C4         τ
RANGES="0x0020-0x007E,0x00B2-0x00B3,0x207F,0x221A-0x221C,0x03C0,0x03C6,0x03C4"
SIZE=12
BPP=4
OUT_DIR="src/overboard/gui"

# ── Font sources ───────────────────────────────────────────────────────────────
# Arial Unicode: only macOS system font with ∛/∜ glyphs — used for regular
FONT_REGULAR="/Library/Fonts/Arial Unicode.ttf"
# SF Pro Text Bold: bold variant with matching math glyphs — used for UI labels
FONT_BOLD="/Library/Fonts/SF-Pro-Text-Bold.otf"

echo "Generating fonts (size=${SIZE}px, bpp=${BPP})..."

# ── Regular ───────────────────────────────────────────────────────────────────
echo "  [1/2] lv_font_superscript_regular  (Arial Unicode, regular)"
$CONV \
    --size "$SIZE" \
    --bpp  "$BPP"  \
    --format lvgl  \
    --font "$FONT_REGULAR" \
    -r "$RANGES" \
    -o "${OUT_DIR}/lv_font_superscript_regular.c" \
    --lv-font-name lv_font_superscript_regular \
    --no-compress

# ── Bold ──────────────────────────────────────────────────────────────────────
echo "  [2/2] lv_font_superscript_bold     (SF Pro Text Bold)"
$CONV \
    --size "$SIZE" \
    --bpp  "$BPP"  \
    --format lvgl  \
    --font "$FONT_BOLD" \
    -r "$RANGES" \
    -o "${OUT_DIR}/lv_font_superscript_bold.c" \
    --lv-font-name lv_font_superscript_bold \
    --no-compress

echo "Done. Rebuild the project to pick up changes."
