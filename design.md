# Design Notes

## Project Overview

A custom mechanical keyboard that replaces the traditional numpad with a dedicated hardware calculator module. The calculator is a self-contained unit — it has its own microcontroller, display, and key switches — and occupies the same physical footprint as a standard numpad.

---

## Keyboard (Main Unit)

### Layout
- **Base layout**: TKL (tenkeyless) + calculator module side by side, giving a full-width board without a traditional numpad
- Standard ANSI layout: alphas, mods, function row, navigation cluster

### Key Switches & Keycaps
- MX-compatible PCB footprints (5-pin/3-pin)
- Standard keycap compatibility (Cherry profile, SA, GMK, etc.)

### Controller
- **RP2040** — QMK-compatible
- **USB-C** connector on the keyboard body (standardized)

### PCB
- Custom PCB with per-key RGB (SK6812 MINI-E or similar) optional
- ESD protection on USB lines
- BOOT/RESET buttons accessible from bottom of case

---

## Calculator Module

### Form Factor
- Occupies the standard numpad footprint (~80 × 130 mm)
- **Detachable** — snaps or slides into a rail/bracket on the right side of the keyboard case
- Mechanical attachment: dovetail slot or keyed bracket + captive thumb screw
- Electrical connection when docked: **pogo-pin connector** (power + USB D+/D−) to keyboard's internal USB hub, OR
- When undocked: **USB-C port** on the module for standalone use
- Both modes supported simultaneously via a USB hub IC on the calculator PCB (e.g. CH334/GL850G)

### Display

**Module footprint context**: standard numpad is ~86mm wide × ~105mm tall (4 columns × 5 rows at 19.05mm MX pitch + bezels). The display sits at the top of the module, tilted upward at ~15–30°, with the key grid below it.

| Display | Diagonal | Active area (mm) | PCB width | Notes |
|---------|----------|-----------------|-----------|-------|
| ST7735 1.8" | 1.8" | 28.8 × 35.0 | ~46mm | Compact, fits with wide margins |
| **ILI9341 2.4"** | **2.4"** | **36.7 × 48.9** | **~57mm** | **Recommended — readable, fits well** |
| ILI9341 2.8" | 2.8" | 43.2 × 57.6 | ~65mm | Larger, tight fit (~10mm margin each side) |

- Interface: **SPI** (4-wire)
- **Tilted mount**: display angled back 15–30° at top of module; hinged or fixed-angle bracket in case
- Resolution: 240×320 (2.4") gives comfortable digit/expression rendering

### Keys
- **17–19 keys** (standard numpad layout + extra function keys)
- **6×6mm tactile switches** (through-hole, top-push) — specific series TBD
- Keycaps: TBD (snap-on caps, 3D-printed SLS nylon, or laser-cut acrylic)
- Key pitch: ~11–12 mm center-to-center
- Dedicated keys: digits 0–9, `+` `-` `×` `÷`, `=`, `C` / `AC`, `.`, optional `(` `)`, `%`, `√`, `M+` / `MR`

### Microcontroller
- **RP2040** — dual-core, native USB HID, inexpensive
- **Firmware language**: MicroPython or C++
- **USB-C** port on the module (standardized; also used for the pogo-pin docked path via internal hub IC)

### Firmware Responsibilities
1. Scan key matrix (using PIO on RP2040 or standard GPIO)
2. Evaluate arithmetic expressions (floating point, operator precedence, memory registers)
3. Drive display over SPI or I²C
4. **USB HID mode**: optionally act as a USB numpad — send keystrokes to host when `=` is pressed, type the result into whatever application is focused

---

## Integration Options

| Mode | Description |
|------|-------------|
| **Standalone** | Calculator has its own USB-C; fully independent of keyboard |
| **Pass-through** | Calculator connects to keyboard's internal USB hub; one cable to PC |
| **Tethered I²C/UART** | Calculator talks to keyboard MCU; keyboard firmware relays HID events |

**Recommended for v1**: Standalone USB-C — simplest firmware, no dependency on keyboard MCU, can be used detached.

> When docked, the pogo-pin connector can carry USB 2.0 signals to the keyboard's internal hub, so only one cable runs to the PC. The calculator module auto-detects which power/data path is active.

---

## Physical Design

### Dimensions
- **Module footprint**: ~86mm wide × ~105mm tall (matches standard numpad)
- **Key zone**: lower ~50–55mm; 4-column × 4–5 row grid at 11–12mm pitch
- **Display zone**: upper ~45–50mm; 2.4" ILI9341 (57mm wide PCB) centered, tilted 15–30° rearward

### Construction
- Calculator module is **fully detachable** — slides into a keyed channel on the right side of the keyboard top case
- Retention: spring-loaded latch or captive M2 thumb screw; no tools needed for removal
- Pogo pins on keyboard body mate with gold pad array on calculator module PCB edge when docked
- Module has its own bottom plate/feet for standalone desktop use
- Case material: aluminum top + polycarbonate bottom, or 3D-printed prototype first
- 6×6mm tactile switch + keycap: ~8–10mm total height above PCB

---

## Open Questions

- [ ] Single unified PCB vs. two PCBs connected by flex/cable?
- [ ] Wired only, or add BLE (nice!nano / nRF52840) to keyboard half?
- [ ] Scientific calculator functions (trig, log) or basic four-function?
- [ ] Expression history scroll on display?
- [ ] Backlighting on calculator keys? (6×6mm TACT switches not RGB-compatible; would need separate LEDs)
- [ ] Pogo-pin count and pinout (minimum 4: GND, VBUS, D+, D−; add 2 more for UART/I²C if desired)
- [ ] Keycap material and legend method (snap-on cap, 3D-printed SLS nylon, laser-cut acrylic)?
- [ ] Specific 6×6mm TACT switch series selection
- [ ] Display tilt angle: fixed (molded into case) or adjustable hinge?
- [ ] Display size: confirm 2.4" ILI9341 or go larger to 2.8"?